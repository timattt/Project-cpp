#ifndef T_LIST
#define T_LIST

#include "../tUtilities/tUtilities.h"

using namespace tUtilities;

namespace tStorage {

template<typename T> class tList {

	// Array with memory
	// Node : [prev, next, element, usedFlag]
	char *mem;

	// Size of this array
	unsigned currentCapacity;
	unsigned freeElement;

	// First empty
	unsigned size;

	void tChangeCapacity(unsigned newLen) {
		mem = (char*) realloc(mem, newLen * tNodeSize());
		//mem = tRealloc(mem, currentCapacity, newLen * tNodeSize());
		currentCapacity = newLen;
	}

	void tEnsureCapacity(unsigned cap) {
		if (currentCapacity < cap) {
			tChangeCapacity(cap * 2);
		}
	}

	bool tIsUsed(unsigned pt) {
		return *(mem + (pt * tNodeSize() + 8 + sizeof(T))) == 'a';
	}

	void tSetUsed(unsigned pt, bool val) {
		*(mem + (pt * tNodeSize() + 8 + sizeof(T))) = (val ? 'a' : 'b');
	}

	unsigned tGetFirst() {
		return tGetNext(0);
	}

	unsigned tGetLast() {
		return tGetPrev(0);
	}

	unsigned tNodeSize() {
		return 8 + sizeof(T) + 1;
	}

	unsigned tGetPrev(unsigned node_i) {
		return tConvertBytes<unsigned>(mem + node_i * tNodeSize());
	}

	void tSetPrev(unsigned node_i, unsigned v) {
		tWriteBytes(v, mem + node_i * tNodeSize());
	}

	unsigned tGetNext(unsigned node_i) {
		return tConvertBytes<unsigned>(mem + node_i * tNodeSize() + 4);
	}

	void tSetNext(unsigned node_i, unsigned v) {
		tWriteBytes(v, mem + node_i * tNodeSize() + 4);
	}

	T& tGetElement(unsigned node_i) {
		return tConvertBytes<T>(mem + 8 + node_i * tNodeSize());
	}

	void tSetElement(unsigned node_i, const T &v) {
		tWriteBytes(v, mem + 8 + node_i * tNodeSize());
	}

	void tClearNode(unsigned a) {
		for (unsigned i = 0; i < tNodeSize(); i++) {
			mem[a * tNodeSize() + i] = 0;
		}
	}

	// Creates node and returns pointer to it.
	unsigned tCreateNode() {
		tEnsureCapacity(freeElement + 1);
		unsigned result = freeElement;
		freeElement++;

		tClearNode(result);
		tSetUsed(result, 1);
		return result;
	}

	void tDeleteNode(unsigned node) {
		tClearNode(node);
	}

	// Converts position from start into pointer to cell in the memory.
	unsigned tGetPointer(unsigned pos) {
		unsigned node = tGetFirst();
		for (unsigned i = 0; i < pos; i++) {
			node = tGetNext(node);
		}
		return node;
	}

	void tMove_to_empty(unsigned from, unsigned to) {
		unsigned prev = tGetPrev(from);
		unsigned next = tGetNext(from);

		tSetNext(prev, to);
		tSetPrev(next, to);

		tCopyBuffers(mem + from * tNodeSize(), mem + to * tNodeSize(),
				tNodeSize());
		tClearNode(from);
		tSetUsed(to, 1);
	}

public:

	tList() :
			mem(NULL), currentCapacity(1), freeElement(1), size(0) {
		mem = new char[tNodeSize()];
		tClearNode(0);
		tSetUsed(0, 1);
	}

	tList(T &elem) :
			mem(NULL), currentCapacity(1), freeElement(1), size(0) {
		mem = new char[tNodeSize()];
		tClearNode(0);
		tSetUsed(0, 1);
		tAddLast(elem);
	}

	// Removes holes from this list. And makes it uses minimum memory.
	void tShrink() {
		unsigned end = freeElement - 1;
		for (unsigned start = 1; start < end;) {
			while (tIsUsed(start) && start < end) {
				start++;
			}
			while (!tIsUsed(end) && start < end) {
				end--;
			}

			if (start >= end) {
				break;
			}

			// start is empty
			// end is filled
			tMove_to_empty(end, start);
		}

		tChangeCapacity(end + 1);
		freeElement = end + 1;
	}

	// Gives element located in node described by pt.
	T& tGet_p(unsigned pt) {
		return tGetElement(pt);
	}

	// Gives element from pos from start.
	T& tGet(unsigned pos) {
		return tGet_p(tGetPointer(pos));
	}

	T& tGetFirstElement() {
		return tGetElement(tGetFirst());
	}

	T& tGetLastElement() {
		return tGetElement(tGetLast());
	}

	// Gives quantity of elements stored in this list.
	unsigned tGetSize() {
		return size;
	}

	// Adds element elem into pos position from first.
	// And returns pointer to cell where element is stored in memory.
	unsigned tAdd(const T &elem, unsigned pos) {
		if (pos > size) {
			tThrowException("Out of bounds!");
		}
		unsigned prevNode = 0;
		for (unsigned i = 0; i < pos; i++) {
			prevNode = tGetNext(prevNode);
		}
		unsigned nextNode = tGetNext(prevNode);

		unsigned node = tCreateNode();

		tSetNext(node, nextNode);
		tSetPrev(node, prevNode);
		tSetElement(node, elem);

		tSetNext(prevNode, node);
		tSetPrev(nextNode, node);

		size++;

		return node;
	}

	// Removes element from pos position from start.
	T tRemove(unsigned pos) {
		if (size == 0) {
			tThrowException("Size is zero!");
		}
		T elem = tGet(tGetPointer(pos));
		tRemove_p(tGetPointer(pos));
		return elem;
	}

	T tRemoveLast() {
		if (size == 0) {
			tThrowException("Out of bounds!");
		}
		T last = tGetLastElement();
		tRemove(size - 1);

		return last;
	}

	// Removes element from node with given pointer.
	void tRemove_p(unsigned node) {
		unsigned prev = tGetPrev(node);
		unsigned next = tGetNext(node);

		tSetNext(prev, next);
		tSetPrev(next, prev);

		tDeleteNode(node);

		size--;
	}

	T& operator[](unsigned i) {
		return tGet(i);
	}

	void tOut() {
		std::cout << "[";
		for (unsigned i = 0; i < size; i++) {
			std::cout << tGet(i);
			if (i + 1 < size) {
				std::cout << ", ";
			}
		}
		std::cout << "]\n";
		std::cout.flush();
	}

	bool tContains(T &elem) {
		for (unsigned i = 0; i < size; i++) {
			if (tGet(i) == elem) {
				return 1;
			}
		}

		return 0;
	}

	T* tToArray() {
		T *array = new T[size];
		for (unsigned i = 0; i < size; i++) {
			array[i] = tGet(i);
		}
		return array;
	}

	void tForEach(void (*consumer)(const T &elem)) {
		for (unsigned node = tGetFirst(); node != 0; node = tGetNext(node)) {
			const T &elem = tGetElement(node);
			consumer(elem);
		}
	}

	// Adds element to be the first int his list.
	void tAddFirst(const T &elem) {
		tAdd(elem, 0);
	}

	// Adds element to be last in this list.
	void tAddLast(const T &elem) {
		tAdd(elem, size);
	}
};

}

#endif
