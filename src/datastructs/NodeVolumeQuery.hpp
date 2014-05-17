#ifndef KIRAN_NODEVOLUMEQUERY_HDR
#define KIRAN_NODEVOLUMEQUERY_HDR

#include "../math/vec3fwd.hpp"
#include "../math/vec3.hpp"
#include "./Heap.hpp"

template<typename T> struct NodeVolumeQuery {
public:
	NodeVolumeQuery(unsigned int maxNodes, const math::vec3f& _pos, const math::vec3f& _nrm, float _dst) {
		pos = _pos;
		nrm = _nrm;
		dst = _dst;

		heap = new Heap<float, T, MaxHeapNode<float, T> >(maxNodes);
	}
	~NodeVolumeQuery() {
		delete heap;
	}

	const math::vec3f& GetPos() const { return pos; }
	const math::vec3f& GetNrm() const { return nrm; }
	float GetDst() const { return dst; }

	unsigned int GetNumNodes() const { return heap->size(); }
	float GetMaxNodeDist() const { return (!heap->empty())? (heap->top()).key: (dst * dst); }

	T GetNode(unsigned int i) const { return (heap->get(i)).val; }
	void AddNode(T nodeInst) {
		// take the squared (!) Euclidean distance
		const float nodeDist = (GetPos() - nodeInst->GetPos()).sqLen3D();

		// reject nodes outside the search-range or exactly at the search-position
		if ((nodeDist > (dst * dst)) || (nodeDist <= 0.0f)) {
			return;
		}

		// note: to also take curved and faceted surfaces into account, we would need
		//     1) the query-position normal and the normal of the surface struck by the photon
		//     2) the material ID at the query position and the material ID hit by the photon
		// but this would destroy the decoupling of the geometry from the photon-map
		//
		#if (USE_SPHERE_COMPRESSION == 1)
		if ((nodeInst->GetNrm()).dot3D(nrm) < SPHERE_COMPRESSION_RATIO) {
			return;
		}
		#endif

		MaxHeapNode<float, T> node(nodeDist, nodeInst);

		if (!heap->push(node.key, node.val)) {
			// heap is filled to capacity; remove the old
			// top-node (which has greatest distance) *if
			// and only if* the replacement is closer
			//
			// after popping, whichever node gets promoted
			// to the root position narrows the search in
			// KDTree::GetNodes()
			if (nodeDist < GetMaxNodeDist()) {
				heap->pop();
				heap->push(node.key, node.val);
			}
		}
	}


private:
	Heap<float, T, MaxHeapNode<float, T> >* heap;

	math::vec3f pos;
	math::vec3f nrm;

	float dst;
};

#endif
