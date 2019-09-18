#ifndef T_CONTAINER
#define T_CONTAINER

#include <bits/stdc++.h>

namespace tStorage {

const int total_canaries = 2;
const char canary_value = 255;

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
	T operator[](std::size_t idx) {
		return tGetFrom(idx);
	}
	//!This function return string that contains 1 and 0 everything from containers memory.
	char* tSeeBits() {
		tCheckAll();
		char *res = (char*) calloc(
				4 + 2 * (8 * size * sizeof(T) + 8 * 2 * total_canaries),
				sizeof(char));
		char *tmp = res;
		*tmp = '|';
		tmp++;
		for (unsigned i = 0; i < 2 * total_canaries + size * sizeof(T); i++) {
			char add = mem[i];
			for (int j = 0; j < 8; j++) {
				(*tmp) = (char) (!!((add << j) & 0x80)) + '0';
				tmp++;
			}
			(*tmp) = '|';
			tmp++;
			if (i + 1 == total_canaries
					|| i + 1 == total_canaries + size * sizeof(T)) {
				(*tmp) = '|';
				tmp++;
				(*tmp) = '|';
				tmp++;
			}
		}

		return res;
	}
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
	//!This function can quickly pass all elements in this container. It can be overrided.
	void tForEach(void (*consumer)(const T&)) {
		assert(consumer != NULL);
		tCheckAll();
		for (int i = 0; i < size; i++) {
			consumer(tGetFrom(i));
		}
	}
	tContainer() :
			hash_value(0) {
		tUpdateHash();
		tRefillCanaries();
		tCleanMemoryFromTo(0, size);
	}
protected:
	//!Gives main start of this stack before canaries.
	char* tGetBegin() {
		tCheckAll();
		return mem;
	}
	//!Gives main end of this stack after all canaries.
	char* tGetEnd() {
		tCheckAll();
		return (mem + 2 * total_canaries + size * sizeof(T));
	}
	//!Checks all security.
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
