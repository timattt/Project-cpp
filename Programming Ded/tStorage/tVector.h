#ifndef T_VECTOR
#define T_VECTOR

#include "../tUtilities/tString.h"

using namespace tUtilities;

namespace tStorage {

template<typename T> class tVector {
private:
	char *mem;
	// actual size of the array
	unsigned size;
	// actual size of the array
	unsigned capacity;

public:
	tVector() :
			mem(NULL), size(0), capacity(0) {

	}

	// pushes back element lvalue reference.
	// there will be made copy actually.
	void tPush_back(const T &elem) {
		change_size(size + 1);
		// simply copies
		tWriteBytes(elem, mem + (size - 1) * sizeof(T));
	}

	// pushes back element rvalue reference.
	// in this way function steals data from given reference
	void tPush_back(T &&elem) {
		change_size(size + 1);

		char * beg = (char*) (&elem);
		for (unsigned i = 0; i < sizeof(T); i++) {
			// steals data
			*(mem + (size - 1) * sizeof(T) + i) = beg[i];

			// and destroys it in its owner
			beg[i] = 0;
		}
	}

	T& operator[](unsigned ind) {
		if (ind >= size) {
			tThrowException("Out of vector bounds!");
		}
		return tConvertBytes<T>(mem + ind * sizeof(T));
	}

	void tSwap(unsigned a, unsigned b) {
		for (unsigned i = 0; i < sizeof(T); i++) {
			char tmp = mem[a * sizeof(T) + i];
			mem[a * sizeof(T) + i] = mem[b * sizeof(T) + i];
			mem[b * sizeof(T) + i] = tmp;
		}
	}

	void tRemoveLast() {
		if (size == 0) {
			tThrowException("Can not shrink empty vector!");
		}
		change_size(size - 1);
	}

	T& tGetFirst() {
		return (*this)[0];
	}

	T& tGetLast() {
		return (*this)[size - 1];
	}

	tString tToString() {
		tString str = "tVector [";
		for (unsigned i = 0; i < capacity; i++) {
			if (i < size) {
				T elem = tConvertBytes<T>(mem + i * sizeof(T));
				str += elem;
			} else {
				str += "_";
			}
			if (i + 1 < capacity) {
				str += ", ";
			}
		}
		str += "]. size = ";
		str += size;
		str += ", capacity = ";
		str += capacity;
		return str;
	}

	void tQsort() {
		tQsortRange(0, size - 1);
	}

	//!Sorts using quick sort algorithm array of lines.
	void tQsortRange(unsigned a, unsigned b) {
		int left = a, right = b;
		T &piv = (*this)[(left + right) / 2];
		while (left <= right) {
			//arr[l] < piv
			while ((*this)[left] < piv) {
				left++;
			}
			//arr[right] > piv
			while (piv < (*this)[right]) {
				right--;
			}
			if (left <= right) {
				tSwap(left++, right--);
			}
		}
		if (a < right)
			tQsortRange(a, right);
		if (b > left)
			tQsortRange(left, b);
	}
private:

	void change_capacity(unsigned new_cap) {
		ensure_not_null();
		mem = (char*) realloc(mem, new_cap * sizeof(T));
		capacity = new_cap;
	}

	void change_size(unsigned new_sz) {
		if (new_sz > capacity) {
			change_capacity(new_sz * 2);
		}
		size = new_sz;
	}

	void ensure_not_null() {
		if (!mem) {
			mem = new char[0];
		}
	}

};

}

#endif
