#ifndef KIRAN_KDTREE_HDR
#define KIRAN_KDTREE_HDR

#include <vector>

#include "../math/vec3fwd.hpp"
#include "../math/vec3.hpp"
#include "./NodeVolumeQuery.hpp"

template<typename T> class KDTree {
public:
	KDTree<T>(int numNodes): mins(KDTREE_MINS), maxs(KDTREE_MAXS) { nodes.resize(numNodes + 1, T()); }
	~KDTree() { nodes.clear(); }

	void Balance(bool complete) {
		if (!complete) {
			// photon-map was not filled to capacity;
			// this means we must now get rid of the
			// excess reserved NULL nodes
			size_t nullNodeIdx = 1;

			// find the first node that is still NULL
			while (nullNodeIdx < nodes.size() && nodes[nullNodeIdx] != NULL) {
				nullNodeIdx++;
			}

			// shrink the vector
			if (nullNodeIdx < nodes.size()) {
				nodes.resize(nullNodeIdx);
			}
		}

		if (nodes.size() > 1) {
			// use two temporary buffers for balancing
			//
			// at every level of the recursion, these
			// represent and store the left and right
			// subtrees with respect to the new median
			// point along the splitting axis (subtrees
			// are delineated by start- and end-indices)
			//
			// note: could we do this in-place instead?
			std::vector<T> lftSegment(nodes.size(), NULL);
			std::vector<T> rgtSegment(nodes.size(), NULL);

			for (size_t i = 0; i < nodes.size(); i++) {
				lftSegment[i] = NULL;
				rgtSegment[i] = nodes[i];
			}

			BalanceSegment(lftSegment, rgtSegment, 1, 1, (nodes.size() - 1));
			BuildHeap(lftSegment);
		}
	}



	// return all nodes matching the volume-query
	// description, starting search at the subtree
	// with node-index <nodeNum>
	//
	// NOTE: do not call before balancing the tree
	void GetNodes(NodeVolumeQuery<T>* query, size_t nodeNum, unsigned int depth = 0) {
		if (nodeNum >= nodes.size()) {
			return;
		}

		const T nodeInst = nodes[nodeNum];
		float nodeDist = 0.0f;

		if ((nodeNum << 1) < nodes.size()) {
			// if in the left half of the array, we can examine child nodes
			nodeDist = query->GetPos()[nodeInst->GetAxis()] - nodeInst->GetPos()[nodeInst->GetAxis()];

			if (nodeDist > 0.0f) {
				// search right of the axis-plane first
				GetNodes(query, (nodeNum << 1) + 1, depth + 1);

				if ((nodeDist * nodeDist) < query->GetMaxNodeDist()) {
					GetNodes(query, (nodeNum << 1), depth + 1);
				}
			} else {
				// search left of the axis-plane first
				GetNodes(query, (nodeNum << 1), depth + 1);

				if ((nodeDist * nodeDist) < query->GetMaxNodeDist()) {
					GetNodes(query, (nodeNum << 1) + 1, depth + 1);
				}
			}
		}

		query->AddNode(nodeInst);
	}



	void SetNode(unsigned int nodeNum, T node) {
		nodes[nodeNum] = node;
	}

	void SetMins(const T node) {
		mins.x = std::min(mins.x, node->GetPos().x);
		mins.y = std::min(mins.y, node->GetPos().y);
		mins.z = std::min(mins.z, node->GetPos().z);
	}
	void SetMaxs(const T node) {
		maxs.x = std::max(maxs.x, node->GetPos().x);
		maxs.y = std::max(maxs.y, node->GetPos().y);
		maxs.z = std::max(maxs.z, node->GetPos().z);
	}

private:
	// re-organizes the balanced kd-tree into a max-heap
	void BuildHeap(std::vector<T>& segment) {
		size_t nodeNum = 1, nodeIdx = 1, nodeDiff = 0;

		T tmpNode = nodes[nodeIdx];

		for (size_t i = 1; i < nodes.size(); i++) {
			assert(nodeNum < segment.size());
			assert(segment[nodeNum] != NULL);

			// segment[nodeNum] is an object of type T* (nodes[123] == &photonArray[123])
			// and nodes[0] is also an object of type T* (nodes[0] == &photonArray[0])
			//
			// therefore <d> represents the number of nodes between
			// nodes[0] and segment[nodeNum], ie. the array index of
			// *(segment[nodeNum])
			//
			// mark the segment node as visited by setting it to NULL
			nodeDiff = segment[nodeNum] - nodes[0];
			segment[nodeNum] = NULL;

			assert(nodeDiff < nodes.size());

			if (nodeDiff != nodeIdx) {
				nodes[nodeNum] = nodes[nodeDiff];
			} else {
				nodes[nodeNum] = tmpNode;

				if (i < nodes.size()) {
					// find the first still-unprocessed segment node
					for (nodeIdx = 1; nodeIdx <= nodes.size(); nodeIdx++) {
						if (segment[nodeIdx] != NULL) { break; }
					}

					tmpNode = nodes[nodeIdx];
					nodeNum = nodeIdx;
				}

				continue;
			}

			nodeNum = nodeDiff;
		}
	}

	void MedianSplitSegment(std::vector<T>& segment, int sNodeNum, int eNodeNum, int newMedianIdx, int splitAxisIdx) {
		int sIdx = sNodeNum;
		int eIdx = eNodeNum;

		assert(sIdx >=                  0);
		assert(eIdx < int(segment.size()));

		#define SWAP_NODES(segment, idxA, idxB) { T n = segment[idxA]; segment[idxA] = segment[idxB]; segment[idxB] = n; }

		while (eIdx > sIdx) {
			const float v = segment[eIdx]->GetPos()[splitAxisIdx];

			// make sure (++i == sIdx) in the first iteration
			int i = sIdx - 1;
			int j = eIdx;

			for (;;) {
				// increase i until we find a farther node along splitAxisIdx
				// decrease j until we find a closer node along splitAxisIdx
				// swap the nodes at indices i and j unless they are in-order
				while (segment[++i]->GetPos()[splitAxisIdx] < v) {}
				while (segment[--j]->GetPos()[splitAxisIdx] > v && (j > sIdx)) {}

				if (i >= j) {
					break;
				}

				SWAP_NODES(segment, i, j);
			}

			SWAP_NODES(segment, i, eIdx);

			if (i >= newMedianIdx) { eIdx = i - 1; }
			if (i <  newMedianIdx) { sIdx = i + 1; }
		}

		#undef swap
	}

	void BalanceSegment(
		std::vector<T>& lftSegment,
		std::vector<T>& rgtSegment,
		const int rNodeNum, // index of node representing root of segment
		const int sNodeNum, // index of node representing start of segment
		const int eNodeNum  // index of node representing end of segment
	) {
		assert(sNodeNum >=                     0);
		assert(eNodeNum < int(lftSegment.size()));

		int newMedianIdx = 1;
		int splitAxisIdx = 2;

		{
			// compute index of new median node based on start- and end-node indices (?)
			while ((4 * newMedianIdx) <= (eNodeNum - sNodeNum + 1)) {
				newMedianIdx <<= 1;
			}

			if ((3 * newMedianIdx) <= (eNodeNum - sNodeNum + 1)) {
				newMedianIdx <<= 1;
				newMedianIdx += (sNodeNum - 1);
			} else {
				newMedianIdx = eNodeNum - newMedianIdx + 1;
			}
		}

		{
			// find the axis to split along
			if (((maxs.x - mins.x) > (maxs.y - mins.y)) && ((maxs.x - mins.x) > (maxs.z - mins.z))) {
				splitAxisIdx = 0;
			} else {
				if ((maxs.y - mins.y) > (maxs.z - mins.z)) {
					splitAxisIdx = 1;
				}
			}
		}

		// partition block of nodes around the new median
		MedianSplitSegment(rgtSegment, sNodeNum, eNodeNum, newMedianIdx, splitAxisIdx);

		lftSegment[rNodeNum] = rgtSegment[newMedianIdx];
		lftSegment[rNodeNum]->SetAxis(splitAxisIdx);

		{
			if (newMedianIdx > sNodeNum) {
				// recursively balance the left block
				if (sNodeNum < (newMedianIdx - 1)) {
					const float v = maxs[splitAxisIdx];

					maxs[splitAxisIdx] = lftSegment[rNodeNum]->GetPos()[splitAxisIdx];
					BalanceSegment(lftSegment, rgtSegment, (rNodeNum << 1), sNodeNum, newMedianIdx - 1);
					maxs[splitAxisIdx] = v;
				} else {
					lftSegment[(rNodeNum << 1)] = rgtSegment[sNodeNum];
				}
			}

			if (newMedianIdx < eNodeNum) {
				// recursively balance the right block
				if ((newMedianIdx + 1) < eNodeNum) {
					const float v = mins[splitAxisIdx];

					mins[splitAxisIdx] = lftSegment[rNodeNum]->GetPos()[splitAxisIdx];
					BalanceSegment(lftSegment, rgtSegment, (rNodeNum << 1) + 1, newMedianIdx + 1, eNodeNum);
					mins[splitAxisIdx] = v;
				} else {
					lftSegment[(rNodeNum << 1) + 1] = rgtSegment[eNodeNum];
				}
			}
		}
	}


	static math::vec3f KDTREE_MINS;
	static math::vec3f KDTREE_MAXS;

	// represents a heap in flat array-form
	// NOTE: first element ([0]) is unused
	std::vector<T> nodes;

	math::vec3f mins;     // bounding-box minima
	math::vec3f maxs;     // bounding-box maxima
};

template<typename T> math::vec3f KDTree<T>::KDTREE_MINS = math::vec3f( 1e12,  1e12,  1e12);
template<typename T> math::vec3f KDTree<T>::KDTREE_MAXS = math::vec3f(-1e12, -1e12, -1e12);

#endif
