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
//!It is armed with memory protection. Hash is always checked so every simple operation is escorted
//!with invoking tCheckAll() function which is O(n).
template<typename T, int size, int (*hash)(char*, int) = tDefaultHash> class tContainer {

private:
	int hash_value;

	char mem[size * sizeof(T) + 2 * total_canaries];

	void* operator new(size_t s, char *p) {
		return p;
	}
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
	//!Fills canaries in memory with value from constant.
	void tRefillCanaries() {
		for (unsigned i = 0; i < total_canaries; i++) {
			mem[i + sizeof(T) * size + total_canaries] = canary_value;
			mem[i] = canary_value;
		}
	}
	//!Invoke this function if something went wrong!
	void tMemBroken() {
		std::cerr << "Memory broken through!\n";
		assert(false);
	}
	//!This function changes hash_value variable using hash function.
	void tUpdateHash() {
		hash_value = hash(mem + total_canaries, size * sizeof(T));
	}

	//!Writes char bits from [var] into [to].
	void tWriteBits(char var, char *to) {
		for (int byte_i = 0; byte_i < size * 8; byte_i++) {
			to[byte_i] = (char) (!!((var << byte_i) & 0x80)) + '0';
		}
	}

public:
	//! Invokes tGetFrom function.
	T operator[](std::size_t idx) {
		return tGetFrom(idx);
	}
	//!This function returns very nice looking string that fully describes this containers memory.
	char* tSeeBits() {
		tCheckAll();

		int total_bytes = (total_canaries * 2 + size * sizeof(T) //Total bytes
		) * 8 // Mul 8 to convert into bits
		+ (1 + total_canaries) // Quantity of brakets to ensure that canaries look like this: |canary1||canary2|...|
		* 2 // We have 2 canary groups
		+ (size + 1)  // Same as for canaries we make for every element
				+ 1; // One char saved for end line symbol to make correct string.

		char *res = (char*) calloc(total_bytes, sizeof(char));
		char *tmp = res;

		//Mini function that place braket into tmp.
		auto br = [](char *&tmp) {
			*tmp = '|';
			tmp++;
		};

		// Starting canaries
		br(tmp);
		for (int i = 0; i < total_canaries; i++) {
			tWriteBits(mem[i], tmp);
			tmp += 8;
			br(tmp);
		}

		// Bytes of data
		br(tmp);
		for (int i = 0; i < size; i++) {
			for (unsigned byte_i = 0; byte_i < sizeof(T); byte_i++) {
				tWriteBits(
						mem[total_canaries + (i + 1) * sizeof(T) - 1 - byte_i],
						tmp);
				tmp += 8;

			}
			br(tmp);
		}

		// Ending canaries
		br(tmp);
		for (int i = 0; i < total_canaries; i++) {
			tWriteBits(mem[i + total_canaries + size * sizeof(T)], tmp);
			tmp += 8;
			br(tmp);
		}

		// Creating end of this line.
		*tmp = '\0';

		return res;
	}

	//!This function writes element T to the given byte in containers memory.
	void tWriteTo_b(unsigned adress_b, const T &el) {
		tCheckAll();
		assert(
				adress_b >= 0
						&& adress_b < size * sizeof(T) + 2 * total_canaries);
		new (mem + adress_b) T(el);
		tUpdateHash();
	}
	//!Returns object from given byte.
	T tGetFrom_b(unsigned adress_b) {
		tCheckAll();
		assert(
				adress_b >= 0
						&& adress_b < size * sizeof(T) + 2 * total_canaries);
		T res = (*(T*) (mem + adress_b));
		return res;
	}
	//!This function writes element T to the given cell.
	void tWriteTo(unsigned adress, const T &el) {
		tWriteTo_b(adress * sizeof(T) + total_canaries, el);
	}
	//!Returns object from given cell.
	T tGetFrom(unsigned adress) {
		return tGetFrom_b(adress * sizeof(T) + total_canaries);
	}
	//!This function cleans (places zeros) in this segment [a, b);
	void tCleanMemoryFromTo(unsigned a, unsigned b) {
		tCheckAll();
		assert(a < b && b < size * sizeof(T) + 2 * total_canaries);
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

};

}

#endif
