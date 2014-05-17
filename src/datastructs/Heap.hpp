#ifndef KIRAN_HEAP_HDR
#define KIRAN_HEAP_HDR

#include <cassert>
#include <vector>

template<typename K, typename V> struct MinHeapNode {
	MinHeapNode(): key(K(0)), val(V(0)) {}
	MinHeapNode(K k, V v): key(k), val(v) {}

	bool operator == (const MinHeapNode<K, V>& n) const { return (key == n.key); }
	bool operator < (const MinHeapNode<K, V>& n) const { return (key < n.key); }
	bool operator <= (const MinHeapNode<K, V>& n) const { return ((*this) < n || (*this) == n); } 

	K key;
	V val;
};

template<typename K, typename V> struct MaxHeapNode {
	MaxHeapNode(): key(K(0)), val(V(0)) {}
	MaxHeapNode(K k, V v): key(k), val(v) {}

	bool operator == (const MaxHeapNode<K, V>& n) const { return (key == n.key); }
	bool operator < (const MaxHeapNode<K, V>& n) const { return (key > n.key); } //! reversed cmp.
	bool operator <= (const MaxHeapNode<K, V>& n) const { return ((*this) < n || (*this) == n); } 

	K key;
	V val;
};



// represents a simple min- or max-heap (priority-queue ADT)
template<typename K, typename V, typename N> class Heap {
public:
	Heap(unsigned int numNodes): maxNodes(numNodes), lastNodeIdx(0), maxNodeIdx(0) {
		nodes.resize(numNodes + 1, N());
	}
	~Heap() { nodes.clear(); }

	unsigned int size() const { return lastNodeIdx; }
	bool empty() const { return (lastNodeIdx == 0); }

	// return the top-node
	const N& top() {
		assert(!empty());
		return nodes[1];
	}

	// return an arbitrary node
	const N& get(unsigned int idx) {
		if (idx < nodes.size()) { return nodes[idx]; }
		return nodes[0];
	}

	bool push(K key, V val) {
		// add new last element to heap, then
		// move it to the right array position
		if (lastNodeIdx < maxNodes) {
			nodes[++lastNodeIdx] = N(key, val);

			// in case the newly pushed node does not
			// need bubbling, SwapNodes() will not be
			// called and we need to update the index
			// of the max. node ourselves
			// if (nodes[lastNodeIdx] > nodes[maxNodeIdx]) {
			//     maxNodeIdx = lastNodeIdx;
			// }

			UpBubbleNode(lastNodeIdx);
			return true;
		}

		return false;
	}

	// remove the top-node
	void pop() {
		if (lastNodeIdx == 1) {
			lastNodeIdx = 0;
		} else if (lastNodeIdx > 1) {
			// swap root and last nodes
			SwapNodes(1, lastNodeIdx);

			// "erase" the old root
			lastNodeIdx -= 1;

			// move the last node into position
			DownBubbleNode(1);
		}
	}

	#ifdef HEAP_DEBUG
	void Print(unsigned int idx, const std::string& tabs) {
		if (idx == 1) {
			printf("\n");
		}
		if (idx > lastNodeIdx) {
			return;
		}

		// right sub-tree
		Print((idx << 1) + 1, tabs + "\t");

		// current node
		printf("%s%f (%d)\n", tabs.c_str(), nodes[idx].key, idx);

		// left sub-tree
		Print((idx << 1) + 0, tabs + "\t");
	}
	#endif

private:
	bool HasLeftChild(unsigned int idx) const { return ((idx << 1) <= (lastNodeIdx)); }
	bool HasRightChild(unsigned int idx) const { return (((idx << 1) + 1) <= (lastNodeIdx)); }

	// MIN-heap (smallest element at root)
	//    UP-bubble: stop when parent < child
	//    DOWN-bubble: stop when parent < child
	// MAX-heap (largest element at root)
	//    UP-bubble: stop when parent > child
	//    DOWN-bubble: stop when parent > child

	// perform up-bubbling of node at index <cidx>
	void UpBubbleNode(unsigned int cidx) {
		unsigned int pidx = 0;

		while (cidx > 1) {
			pidx = cidx >> 1;

			if (nodes[pidx] <= nodes[cidx]) {
				break;
			}

			SwapNodes(pidx, cidx); cidx = pidx;
		}
	}

	// perform down-bubbling of node at index <pidx>
	void DownBubbleNode(unsigned int pidx) {
		unsigned int cidx = 0;

		while (HasLeftChild(pidx) || HasRightChild(pidx)) {
			if (!HasRightChild(pidx)) {
				// no right child at all
				cidx = (pidx << 1);
			} else if (nodes[pidx << 1] <= nodes[(pidx << 1) + 1]) {
				// left child is smaller or equal to right child
				// according to the node total ordering, pick it
				cidx = (pidx << 1);
			} else {
				// pick the right child
				cidx = (pidx << 1) + 1;
			}

			if (nodes[pidx] <= nodes[cidx]) {
				// parent is smaller (according to the
				// node total ordering) than the child
				break;
			}

			SwapNodes(pidx, cidx); pidx = cidx;
		}
	}

	void SwapNodes(unsigned int idx1, unsigned int idx2) {
		//      if (idx1 == maxNodeIdx) { maxNodeIdx = idx2; }
		// else if (idx2 == maxNodeIdx) { maxNodeIdx = idx1; }

		N n1 = nodes[idx1];
		N n2 = nodes[idx2];

		nodes[idx1] = n2;
		nodes[idx2] = n1;
	}

	// first element is dummy
	std::vector<N> nodes;

	// maximum number of nodes this heap is allowed to hold;
	// index of last array position that is part of the heap;
	// index of node with largest key
	unsigned int maxNodes;
	unsigned int lastNodeIdx;
	unsigned int maxNodeIdx;
};

#endif
