#ifndef KIRAN_UNIFORM_GRID_HDR
#define KIRAN_UNIFORM_GRID_HDR

#include <list>
#include <vector>

#include "../math/vec3fwd.hpp"
#include "../math/vec3.hpp"
#include "./NodeVolumeQuery.hpp"

#define INDEX_1D(idx, gsize) ((idx.z) * (gsize.y * gsize.x) + (idx.y) * (gsize.x) + (idx.x))

template<typename T> class UniformGrid {
public:
	struct GridCell {
		// these fields are needed by CScene
		math::vec3f pos;
		math::vec3i idx;

		std::list<T> nodes;
	};

	UniformGrid<T>(const math::vec3i& v): gsize(v), mins(GRID_MINS), maxs(GRID_MAXS) {
		cells.resize(gsize.x * gsize.y * gsize.z, GridCell());
	}
	~UniformGrid() {
		cells.clear();
	}

	void GetNodes(NodeVolumeQuery<T>* query) {
		// 1. find the (index of) the grid-cell encompassing <pos>
		// 2. find the number of grid-cells corresponding to <radius>
		//    (note: we still search the immediate neighbor cells even
		//    if <radius> is less than one cell along some dimension)
		// 3. search in the cube (!) of cells for the closest photons
		const float radius = query->GetDst();
		const math::vec3i& cellIdx = GetCellIdx(query->GetPos(), true);
		const math::vec3f& cellSize = GetCellSize();
		const math::vec3i numCells((radius / cellSize.x) + 1, (radius / cellSize.y) + 1, (radius / cellSize.z) + 1);

		for (int x = cellIdx.x - numCells.x; x <= cellIdx.x + numCells.x; x++) {
			for (int y = cellIdx.y - numCells.y; y <= cellIdx.y + numCells.y; y++) {
				for (int z = cellIdx.z - numCells.z; z <= cellIdx.z + numCells.z; z++) {
					const math::vec3i idx(x, y, z);

					if (!IdxInBounds(idx)) {
						continue;
					}

					const GridCell& cell = GetCell(idx);

					for (typename std::list<T>::const_iterator it = cell.nodes.begin(); it != cell.nodes.end(); ++it) {
						T node = *it;

						#if (USE_SPHERE_COMPRESSION == 1)
						/*
						const math::vec3f& pp = node->GetPos();
						const math::vec3f transformedPoint = ((pp - searchPos).rotateZ(zAngleDeg)).rotateX(xAngleDeg);

						if (((pp.x * pp.x) / (rad) + (pp.y * pp.y) / (rad * (1.0f - SPHERE_COMPRESSION_RATIO)) + (pp.z * pp.z) / (rad)) < 1.0f) {
							query->AddNode(node);
						}
						*/
						query->AddNode(node);
						#else
						query->AddNode(node);
						#endif
					}
				}
			}
		}
	}


	// called from PhotonMap::AddPhoton (which does
	// not add the photon to the grid, it only sets
	// the new spatial extends)
	void SetMins(T node) {
		mins.x = std::min(mins.x, node->GetPos().x);
		mins.y = std::min(mins.y, node->GetPos().y);
		mins.z = std::min(mins.z, node->GetPos().z);

		SetCellSize();
	}
	void SetMaxs(T node) {
		maxs.x = std::max(maxs.x, node->GetPos().x);
		maxs.y = std::max(maxs.y, node->GetPos().y);
		maxs.z = std::max(maxs.z, node->GetPos().z);

		SetCellSize();
	}


	// add a node-object to the cell it is located in
	void AddNode(T node) {
		const math::vec3f& pos = node->GetPos();
		const math::vec3i& idx = GetCellIdx(pos, true);

		GridCell& cell = GetCell(idx); cell.nodes.push_back(node);
	}


	bool IdxInBounds(const math::vec3i& idx) const {
		return
			(idx.x >= 0 && idx.x < gsize.x) &&
			(idx.y >= 0 && idx.y < gsize.y) &&
			(idx.z >= 0 && idx.z < gsize.z) &&
			(INDEX_1D(idx, gsize) < int(cells.size()));
	}
	bool PosInBounds(const math::vec3f& pos) const {
		return (IdxInBounds(GetCellIdx(pos, false)));
	}

	// return the index of the cell that
	// contains spatial position <pos>
	//
	// note: SUBTRACT <mins> because it
	// can be negative (indices must be
	// non-negative)
	//
	// note: lack of numerical accuracy
	// can cause the indices to be OOB,
	// so optionally clamp them
	math::vec3i GetCellIdx(const math::vec3f& pos, bool clamp = false) const {
		math::vec3i idx;
			idx.x = int(((pos.x - mins.x) / (maxs.x - mins.x)) * gsize.x);
			idx.y = int(((pos.y - mins.y) / (maxs.y - mins.y)) * gsize.y);
			idx.z = int(((pos.z - mins.z) / (maxs.z - mins.z)) * gsize.z);

		if (clamp) {
			idx.x = std::max(0, std::min(gsize.x - 1, idx.x));
			idx.y = std::max(0, std::min(gsize.y - 1, idx.y));
			idx.z = std::max(0, std::min(gsize.z - 1, idx.z));
		}

		return idx;
	}


	// return the cell containing spatial position <pos>
	// (auto-clamps the indices corresponding to <pos>)
	GridCell& GetCell(const math::vec3f& pos) {
		return (GetCell(GetCellIdx(pos, true)));
	}

	// return the cell at grid index <idx>; uses
	// indexing scheme z * (W * H) + y * (W) + x
	// note: assumes index-vector is in-bounds
	GridCell& GetCell(const math::vec3i& idx) {
		return (cells[ INDEX_1D(idx, gsize) ]);
	}

	std::vector<GridCell>& GetCells() { return cells; }

	void SetCellPositions() {
		for (int x = 0; x < gsize.x; x++) {
			for (int y = 0; y < gsize.y; y++) {
				for (int z = 0; z < gsize.z; z++) {
					// use the cell's geometric center
					math::vec3f pos;
						pos.x = (x * csize.x) + mins.x + (csize.x * 0.5f);
						pos.y = (y * csize.y) + mins.y + (csize.y * 0.5f);
						pos.z = (z * csize.z) + mins.z + (csize.z * 0.5f);
					math::vec3i idx = GetCellIdx(pos, false);

					// get the index without clamping, but sanity-check it
					assert(IdxInBounds(idx));

					GridCell& cell = GetCell(idx);

					cell.pos = pos;
					cell.idx = idx;
				}
			}
		}
	}


	const math::vec3i& GetGridSize() const { return gsize; }
	const math::vec3f& GetCellSize() const { return csize; }

private:
	void SetCellSize() {
		csize.x = (maxs.x - mins.x) / gsize.x;
		csize.y = (maxs.y - mins.y) / gsize.y;
		csize.z = (maxs.z - mins.z) / gsize.z;
	}

	std::vector<GridCell> cells;

	// number of cells along each dimension
	math::vec3i gsize;
	// spatial size per dimension of each cell
	math::vec3f csize;

	// spatial extends of the grid
	math::vec3f mins;
	math::vec3f maxs;

	static math::vec3f GRID_MINS;
	static math::vec3f GRID_MAXS;
};

template<typename T> math::vec3f UniformGrid<T>::GRID_MINS = math::vec3f( 1e12,  1e12,  1e12);
template<typename T> math::vec3f UniformGrid<T>::GRID_MAXS = math::vec3f(-1e12, -1e12, -1e12);

#endif
