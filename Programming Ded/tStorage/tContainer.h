#ifndef T_CONTAINER
#define T_CONTAINER

#include "../tUtilities/tUtilities.h"

using namespace tUtilities;

namespace tStorage {

const unsigned total_canaries = 2;
const char canary_value = 255;

//!This constant is used in default hashing algorithm
const unsigned P = 31;

unsigned tDefaultHash(char *arg, const unsigned sz) {
	unsigned result = 0;

	unsigned pow = 1;
	for (unsigned i = 0; i < sz; i++) {
		result += pow * arg[i];
		pow *= P;
	}

	return result;
}

const unsigned default_parameters_quantity = sizeof(unsigned);

//!Stack created by timattt
//!It is armed with memory protection. Hash is always checked so every simple operation is escorted
//!with invoking tCheckAll() function which is O(n).
template<typename T, unsigned size, unsigned (*hash)(char*,
		unsigned) = tDefaultHash, unsigned bytes_for_utilities = 0>
class tContainer {

private:

	//!This function gives quantity of bytes for container fields.
	unsigned tGetBytesForUtilities() {
		return bytes_for_utilities + default_parameters_quantity;
	}

	//! Memory itself.
	char mem[size * sizeof(T) + 2 * total_canaries + bytes_for_utilities
			+ default_parameters_quantity];

	//!This function is not visible. It checks that memory is safe.
	bool tCheckHash() {
		bool result = tGetCurrentHash()
				!= hash(mem + total_canaries + tGetBytesForUtilities(),
						size * sizeof(T));
		return result;
	}

	void tError() {
		std::cerr << "Something went wrong, printing tContainer\n";
		std::cerr << tToString() << "\n";
		std::cerr.flush();
		exit(-200);
	}

	void tUpdateHash() {
		tSetCurrentHash(hash(mem + tGetElementAddress_b(0), size * sizeof(T)));
	}

	bool tCheckCanaries() {
		bool result = 0;

		for (unsigned i = 0; i < total_canaries; i++) {
			if (mem[i] != canary_value
					|| *(tEndingCanaries_p() + i) != canary_value) {
				result = 1;
			}
		}

		return result;
	}

	//! Returns last hash value.
	unsigned tGetCurrentHash() {
		return tGetParameter_b<unsigned>(0);
	}

	//! Sets hash value to be current.
	void tSetCurrentHash(unsigned val) {
		tSetParameter_b<unsigned>(0, val);
	}

	//!This function checks that current function hash is equals to saved hash.
	//!This means that container is not broken.
	//!If something is not correct then tMemBroken() function is used.
	//!Fills canaries in memory with value from constant.
	void tRefillCanaries() {
		for (unsigned i = 0; i < total_canaries; i++) {
			*(tEndingCanaries_p() + i) = canary_value;
			mem[i] = canary_value;
		}
	}
	//!Invoke this function if something went wrong!
	//!This function changes hash_value() variable using hash function.
	char* tParameters_p() {
		return mem + total_canaries;
	}

	//! Returns pointer to the first end canary.
	char* tEndingCanaries_p() {
		return mem
				+ (total_canaries + size * sizeof(T) + tGetBytesForUtilities());
	}
	//!Writes char bits from [var] unsignedo [to].
	void tWriteBits(char var, char *to) {
		for (unsigned byte_i = 0; byte_i < 8; byte_i++) {
			to[byte_i] = (char) (!!((var << byte_i) & 0x80)) + '0';
		}
	}

	//!This function writes element T to the given byte in containers memory.
	template<typename K = T> void tWriteTo_b(unsigned adress_b, const K &el) {
		tAssert(adress_b >= 0 && adress_b < tTotalSize_b());
		tWriteBytes(el, mem + adress_b);
		tUpdateHash();
	}
	//!Returns object from given byte.
	template<typename K = T> K& tGetFrom_b(unsigned adress_b) {
		tAssert(adress_b >= 0 && adress_b < tTotalSize_b());
		K &res = (*(K*) (mem + adress_b));
		return res;
	}

public:

protected:
	//! Set bytes from locAddress starting byte into value of el.
	//! @param locAddres - local utilities box byte address.
	template<typename K> void tSetParameter_b(unsigned locAddress,
			const K &el) {
		char *pars = tParameters_p();
		pars += locAddress;
		new (pars) K(el);
	}

	//! Forms K type object from bytes that starts with address byte.
	template<typename K> K tGetParameter_b(unsigned address) {
		return tGetFrom_b<K>(tParameters_b() + address);
	}

	//!Gives total size in bytes of this container
	const char* tSeeBits(unsigned *length = NULL) {
		tCheckValid();

		unsigned total_bytes = (tTotalSize_b() //Total bytes
		) * 8                  // Mul 8 to convert unsignedo bits
		+ (1 + total_canaries) // Quantity of brackets to ensure that canaries look like this: |canary1||canary2|...|
		* 2                    // We have 2 canary groups
		+ (size + 1)           // Same as for canaries we make for every element
				+ 1 // One char saved for end line symbol to make correct string.
				+ 2;

		if (length != NULL) {
			*length = total_bytes;
		}

		const char *res = (const char*) calloc(total_bytes, sizeof(char));
		if (res == NULL) {
			exit(-100);
		}
		char *tmp = (char*) res;

		//Mini function that place bracket unsignedo tmp.
		auto br = [](char *&tmp) {
			*tmp = '|';
			tmp++;
		};

		// Starting canaries
		br(tmp);
		for (unsigned i = 0; i < total_canaries; i++) {
			tWriteBits(mem[i], tmp);
			tmp += 8;
			br(tmp);
		}

		// Utilities
		br(tmp);
		for (unsigned i = 0; i < tGetBytesForUtilities(); i++) {
			tWriteBits(mem[tParameters_b() + i], tmp);
			tmp += 8;
		}
		br(tmp);

		// Bytes of data
		br(tmp);
		for (unsigned i = 0; i < size; i++) {
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
		for (unsigned i = 0; i < total_canaries; i++) {
			tWriteBits(
					mem[i + total_canaries + tGetBytesForUtilities()
							+ size * sizeof(T)], tmp);
			tmp += 8;
			br(tmp);
		}

		// Creating end of this line.
		*tmp = '\0';

		return res;
	}
	unsigned tTotalSize_b() {
		return 2 * total_canaries + size * sizeof(T) + tGetBytesForUtilities();
	}

	//! Return byte where this element is starting in the memory.
	unsigned tGetElementAddress_b(unsigned number) {
		return total_canaries + sizeof(T) * number + tGetBytesForUtilities();
	}

	//! Checks all security features.
	void tCheckValid() {
		if (!tIsValid()) {
			tThrowException();
		}
	}

	//! Returns byte from where parameters starts.
	unsigned tParameters_b() {
		return total_canaries;
	}

	//! Returns byte from where ending canaries starts.
	unsigned tEndingCanaries_b() {
		return (total_canaries + size * sizeof(T) + tGetBytesForUtilities());
	}

public:
	//!This function returns very nice looking string that fully describes this containers memory.
	//!Returns byte address of object with given number.
	//!This function writes element T to the given cell.
	void tWriteTo_e(unsigned adress, const T &el) {
		tCheckValid();
		tWriteTo_b<T>(tGetElementAddress_b(adress), el);
	}

	bool tIsValid() {
		return !(tCheckCanaries() || tCheckHash());
	}

	//!Gives base information about this stack.
	const char* tToString() {
		unsigned length = 0;
		const char *bits = tSeeBits(&length);
		const char *result = (char*) calloc(length + 16, sizeof(char));
		strcpy((char*) result, "tContainer:bits:");
		strcpy((char*) result + 16, bits);
		return result;
	}

	//!Returns object from given cell.
	T& tGetFrom_e(unsigned adress) {
		tCheckValid();
		return tGetFrom_b<T>(tGetElementAddress_b(adress));
	}
	//!This function cleans (places zeros) in this segment [a, b);
	void tCleanMemoryFromTo_b(unsigned a, unsigned b) {
		tCheckValid();
		tAssert(a < b && b <= size);
		for (unsigned i = tGetElementAddress_b(a); i < tGetElementAddress_b(b);
				i++) {
			mem[i] = 0;
		}
		tUpdateHash();
	}
	//!This function can quickly pass all elements in this container. It can be overrided.
	void tForEach(void (*consumer)(const T&)) {
		tAssert(consumer != NULL);
		tCheckValid();
		for (unsigned i = 0; i < size; i++) {
			consumer(tGetFrom_e(i));
		}
	}
	void tCopyTo(tContainer<T, size> &to) {
		tCheckValid();
		tCopyBuffers(mem, to.mem, tTotalSize_b());
	}
	tContainer() {
		tUpdateHash();
		tRefillCanaries();
		tCleanMemoryFromTo_b(0, size);
	}
};

}

#endif
