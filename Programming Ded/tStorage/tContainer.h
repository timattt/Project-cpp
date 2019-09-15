#ifndef T_CONTAINER
#define T_CONTAINER

#include <bits/stdc++.h>

namespace tStorage {

const int total_canaries = 2;
const char canary_value = 111;

//!This constant is used in default hashing algorithm
const int P = 31;

int tDefaultHash(char *arg, const int sz) {
	int result = 0;

	int pow = 1;
	for (int i = 0; i < sz; i++) {
		result += pow * arg[i];
		pow *= P;
	}

	return 0;
}

//!Stack created by timattt
//!It is armed with simple memory protection.
template<typename T, int size, int (*hash)(char*, int) = tDefaultHash> class tContainer {
private:
	int hash_value;

	char mem[size * sizeof(T) + 2 * total_canaries];

	void* operator new(size_t s, char *p) {
		return p;
	}
public:
	//!This function writes element T to the given cell in containers memory.
	void tWriteTo(int adress, const T &el) {
		tCheckAll();
		assert(adress >= 0 && adress < size);
		new (mem + (adress * sizeof(T) + total_canaries)) T(el);
		tUpdateHash();
	}
	T tGetFrom(int adress) {
		tCheckAll();
		assert(adress >= 0 && adress < size);
		T res = (*(T*) (mem + (adress * sizeof(T) + total_canaries)));
		return res;
	}
	//!This function cleans (places zeros) in this segment [a, b);
	void tCleanMemoryFromTo(int a, int b) {
		tCheckAll();
		for (unsigned i = a * sizeof(T) + total_canaries;
				i < b * sizeof(T) + total_canaries; i++) {
			mem[i] = 0;
		}
		tUpdateHash();
	}

	tContainer() :
			hash_value(0) {
		tUpdateHash();
		tRefillCanaries();
		tCleanMemoryFromTo(0, size);
	}
protected:
	char* tGetBegin() {
		tCheckAll();
		return mem;
	}
	char* tGetEnd() {
		tCheckAll();
		return (mem + 2 * total_canaries + size * sizeof(T));
	}
	void tCheckAll() {
		if (tCheckCanaries() || tCheckHash()) {
			tMemBroken();
		}
	}

private:
	//!This function is not visible. It checks that memory is safe.
	bool tCheckCanaries() {
		bool result = 0;

		for (int i = 0; i < total_canaries; i++) {
			if (mem[i] != canary_value
					|| mem[i + size * sizeof(T) + total_canaries]
							!= canary_value) {
				result = 1;
			}
		}

		return result;
	}
	//!This function checks that current function hash is equals to saved hash.
	//!This means that container is not broken.
	//!If something is not correct then tMemBroken() function is used.
	bool tCheckHash() {
		bool result = hash_value
				!= hash(mem + total_canaries, size * sizeof(T));
		return result;
	}
	void tRefillCanaries() {
		for (unsigned i = 0; i < total_canaries; i++) {
			mem[i + sizeof(T) * size + total_canaries] = canary_value;
			mem[i] = canary_value;
		}
	}
	void tMemBroken() {
		std::cerr << "Memory broken through!\n";
		assert(false);
	}
	//!This function changes hash_value variable using hash function.
	void tUpdateHash() {
		hash_value = hash(mem + total_canaries, size * sizeof(T));
	}

};

}

#endif
