#ifndef T_LIST
#define T_LIST

#include "../tUtilities/tUtilities.h"

using std::cout;
using namespace tUtilities;

namespace tStorage {

const unsigned NO_INDEX = 123456789;

template<typename T> class tList {

	// Array with memory
	// Node : [prev, next, element, usedFlag]
	char *mem;

	// Size of this array
	unsigned currentCapacity;
	unsigned freeElement;

	// First empty
	unsigned size;

	void changeCapacity(unsigned newLen) {
		mem = (char*) realloc(mem, (newLen) * nodeSize());
		currentCapacity = newLen;
	}

	void ensureCapacity(unsigned cap) {
		if (currentCapacity < cap) {
			changeCapacity(cap * 2);
		}
	}

	bool isUsed(unsigned pt) {
		return *(mem + (pt * nodeSize() + 8 + sizeof(T))) == 'a';
	}

	void setUsed(unsigned pt, bool val) {
		*(mem + (pt * nodeSize() + 8 + sizeof(T))) = (val ? 'a' : 'b');
	}

	unsigned getFirst() {
		return getNext(0);
	}

	unsigned getLast() {
		return getPrev(0);
	}

	unsigned nodeSize() {
		return 8 + sizeof(T) + 1;
	}

	unsigned getPrev(unsigned node_i) {
		return tConvertBytes<unsigned>(mem + node_i * nodeSize());
	}

	void setPrev(unsigned node_i, unsigned v) {
		tWriteBytes(v, mem + node_i * nodeSize());
	}

	unsigned getNext(unsigned node_i) {
		return tConvertBytes<unsigned>(mem + node_i * nodeSize() + 4);
	}

	void setNext(unsigned node_i, unsigned v) {
		tWriteBytes(v, mem + node_i * nodeSize() + 4);
	}

	T& getElement(unsigned node_i) {
		return tConvertBytes<T>(mem + 8 + node_i * nodeSize());
	}

	void setElement(unsigned node_i, const T &v) {
		tWriteBytes(v, mem + 8 + node_i * nodeSize());
	}

	void clearNode(unsigned a) {
		for (unsigned i = 0; i < nodeSize(); i++) {
			mem[a * nodeSize() + i] = 0;
		}
	}

	// Creates node and returns pointer to it.
	unsigned createNode() {
		ensureCapacity(freeElement + 1);
		unsigned result = freeElement;
		freeElement++;

		clearNode(result);
		setUsed(result, 1);
		return result;
	}

	void deleteNode(unsigned node) {
		clearNode(node);
	}

	// Converts position from start into pointer to cell in the memory.
	unsigned getPointer(unsigned pos) {
		unsigned node = getFirst();
		for (unsigned i = 0; i < pos; i++) {
			node = getNext(node);
		}
		return node;
	}

	void move_to_empty(unsigned from, unsigned to) {
		unsigned prev = getPrev(from);
		unsigned next = getNext(from);

		setNext(prev, to);
		setPrev(next, to);

		tCopyBuffers(mem + from * nodeSize(), mem + to * nodeSize(), nodeSize());
		clearNode(from);
		setUsed(to, 1);
	}

public:

	tList() :
			mem(NULL), currentCapacity(1), freeElement(1), size(0) {
		mem = new char[nodeSize()];
		clearNode(0);
		setUsed(0, 1);
	}

	void shrink() {
		unsigned end = freeElement - 1;
		for (unsigned start = 1; start < end; ) {
			while (isUsed(start) && start < end) {
				start++;
			}
			while (!isUsed(end) && start < end) {
				end--;
			}

			if (start >= end) {
				break;
			}

			// start is empty
			// end is filled
			move_to_empty(end, start);
		}

		changeCapacity(end + 1);
		freeElement = end + 1;
	}

	T& get_p(unsigned pt) {
		return getElement(pt);
	}

	T& get(unsigned pos) {
		return get_p(getPointer(pos));
	}

	unsigned getSize() {
		return size;
	}

	// Adds element elem into pos position from first.
	// And returns pointer to cell where element is stored in memory.
	unsigned add(const T &elem, unsigned pos) {
		unsigned prevNode = 0;
		for (unsigned i = 0; i < pos; i++) {
			prevNode = getNext(prevNode);
		}
		unsigned nextNode = getNext(prevNode);

		unsigned node = createNode();

		setNext(node, nextNode);
		setPrev(node, prevNode);
		setElement(node, elem);

		setNext(prevNode, node);
		setPrev(nextNode, node);

		size++;

		return node;
	}

	void remove(unsigned pos) {
		remove_p(getPointer(pos));
	}

	void remove_p(unsigned node) {
		unsigned prev = getPrev(node);
		unsigned next = getNext(node);

		setNext(prev, next);
		setPrev(next, prev);

		deleteNode(node);

		size--;
	}

	void addFirst(const T &elem) {
		add(elem, 0);
	}

	void addLast(const T &elem) {
		add(elem, size);
	}

	void seeElements() {
		cout << size << " " << currentCapacity << "\n";
		return;
		unsigned node = getFirst();
		cout << "[";
		for (unsigned i = 0; i < size; i++, node = getNext(node)) {
			cout << getElement(node);
			if (i + 1 < size) {
				cout << " ";
			}
		}
		cout << "]\n";
	}

	void info() {
		cout << "cap: " << currentCapacity << "\n";
		cout << "used:[";
		for (unsigned i = 0; i < currentCapacity; i++) {
			if (1) {
				cout << isUsed(i);
				if (i + 1 < currentCapacity) {
					cout << " ";
				}
			}
		}
		cout << "]\n";
		cout << "prev:[";
		for (unsigned i = 0; i < currentCapacity; i++) {
			if (1) {
				cout << getPrev(i);
				if (i + 1 < currentCapacity) {
					cout << " ";
				}
			}
		}
		cout << "]\n";
		cout << " ind:[";
		for (unsigned i = 0; i < currentCapacity; i++) {
			if (1) {
				cout << (i);
				if (i + 1 < currentCapacity) {
					cout << " ";
				}
			}
		}
		cout << "]\n";
		cout << "next:[";
		for (unsigned i = 0; i < currentCapacity; i++) {
			if (1) {
				cout << getNext(i);
				if (i + 1 < currentCapacity) {
					cout << " ";
				}
			}
		}
		cout << "]\n";
	}

};

}

#endif
