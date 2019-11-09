#ifndef T_STRING
#define T_STRING

#include <bits/stdc++.h>
#include "tUtilities.h"
#include "../tStorage/tList.h"

using std::map;
using tStorage::tList;

namespace tUtilities {

//!Gives quantity of chars in this char array.
int __tStrlen(const char *str) {
	if (str == NULL) {
		tThrowException("Line is NULL!");
	}
	const char *s = str;
	for (; *s; s++)
		;
	return s - str;
}

unsigned free_pointer = 1;
tList<map<unsigned, char*>*> __branches;

map<unsigned, char*>* __tCurrentBranch() {
	return __branches.tGetLastElement();
}

//! Creates new branch where all new tString objects will be stored.
void __tNewBranch() {
	__branches.tAddLast(new map<unsigned, char*>());
}

//! Deletes current branch and frees all tString object called after last __tNewBranch call.
void __tFlushBranch() {
	map<unsigned, char*> *curr = __tCurrentBranch();
	for (auto i = curr->begin(); i != curr->end(); i++) {
		delete i->second;
	}

	__branches.tRemoveLast();
	delete curr;
}

//! Creates char array and store it in current branch. Gives local pointer.
unsigned __tAllocStr(unsigned sz) {
	char *str = new char[sz];

	unsigned pointer = free_pointer;
	free_pointer++;

	(*__tCurrentBranch())[pointer] = str;
	return pointer;
}

//! Changes size to existed tString.
void __tReallocStr(unsigned ptr, unsigned len) {
	char *str = (*__tCurrentBranch())[ptr];
	str = (char*) realloc(str, len);
	(*__tCurrentBranch())[ptr] = str;
}

//! Gives array of this pointer.
char* __tGetArray(unsigned ptr) {
	char *str = (*__tCurrentBranch())[ptr];
	return str;
}

class tString {
public:
	// Branch system pointer.
	unsigned tPointer;
	// Size of this string.
	unsigned size;
	// Pointer to char array.
	char *string;

	template<typename T> tString(T number) :
			tPointer(__tAllocStr(0)), size(0), string(__tGetArray(tPointer)) {
		const char *arr = std::to_string(number).c_str();
		unsigned len = __tStrlen(arr);
		tSet(arr, len);
	}

	//! Creates new string. Determine size using __tStrlen function.
	tString(const char *str) :
			tPointer(__tAllocStr(__tStrlen(str))), size(__tStrlen(str)), string(
					__tGetArray(tPointer)) {
		tCopyBuffers(str, string, size);
	}

	//! Creates empty string.
	tString() :
			tPointer(__tAllocStr(0)), size(0), string(__tGetArray(tPointer)) {
	}

	tString(char c) :
			tPointer(__tAllocStr(1)), size(1), string(__tGetArray(tPointer)) {
		string[0] = c;
	}

	//! Creates new string and copies data from given pointer.
	tString(char *str, unsigned len) :
			tPointer(__tAllocStr(len)), size(len), string(__tGetArray(tPointer)) {
		tCopyBuffers(str, string, len);
	}

	//! Resizes this string to new size and copies data from given array.
	void tSet(const char *arr, unsigned len) {
		if (arr == NULL) {
			tThrowException("Can not set NULL to tString!");
		}
		tResize(len);
		tCopyBuffers(arr, string, len);
	}

	//! Returns string with deleted first symbol.
	tString tCropFirst() const {
		return tSubstring(1, size - 1);
	}

	//! Returns string with deleted last symbol.
	tString tCropLast() const {
		return tSubstring(0, size - 2);
	}

	//! Changes size of this string char array.
	void tResize(unsigned sz) {
		size = sz;
		__tReallocStr(tPointer, sz);
		string = __tGetArray(tPointer);
	}

	//! Writes this string to console.
	void tWrite() const {
		for (unsigned i = 0; i < size; i++) {
			std::cout << string[i];
		}
	}

	tString operator+(tString other) const {
		tString result = { };
		result.tResize(size + other.size);
		for (unsigned i = 0; i < size; i++) {
			result[i] = string[i];
		}
		for (unsigned i = 0; i < other.size; i++) {
			result[size + i] = other[i];
		}

		return result;
	}

	//! Writes this string in the new line. And flushes stream.
	void out() const {
		tWrite();
		std::cout << "\n";
		std::cout.flush();
	}

	//! Converts this string to int.
	int tToInt() const {
		return std::atoi(string);
	}

	//! Converts this string to float.
	float tToFloat() const {
		return std::atof(string);
	}

	//! Reads line and store it in this string.
	void tReadLine() {
		char c = 0;
		std::cout.flush();
		for (unsigned i = 0;; i++) {
			c = getchar();
			if (i == size && c != '\n') {
				tResize(i + 1);
			}
			if (c == '\n') {
				break;
			}
			string[i] = c;
		}
		std::cout.flush();
	}

	//! Gives substring [start, end].
	tString tSubstring(unsigned start, unsigned end) const {
		if (start > end || end >= size) {
			tThrowException("Out of string bounds!");
		}

		tString result = { };
		result.tResize(end - start + 1);
		for (unsigned i = start; i <= end; i++) {
			result.string[i - start] = string[i];
		}

		return result;
	}

	bool operator==(char c) const {
		return size == 1 && c == string[0];
	}

	bool tIsPrefix(tString pattern) {
		if (pattern.size > size) {
			return 0;
		}
		for (unsigned i = 0; i < pattern.size; i++) {
			if (pattern.string[i] != string[i]) {
				return 0;
			}
		}
		return 1;
	}

	bool operator==(const tString &a) const {
		if (a.size != this->size) {
			return 0;
		}
		for (unsigned i = 0; i < a.size; i++) {
			if ((*this)[i] != a[i]) {
				return 0;
			}
		}
		return 1;
	}

	//! Return everything that lies between symbol a and symbol b.
	//! It takes FIRST position of a and LAST position of b.
	//! If there is no such symbols then no changes will be made.
	tString tCrop(char a, char b) const {
		if (tCount(a) > 0 && tCount(b) > 0) {
			unsigned pos1 = firstPosition(a);
			unsigned pos2 = lastPosition(b);
			tString res = tSubstring(pos1 + 1, pos2 - 1);

			return res;
		} else {
			return *this;
		}
	}

	//! Generate new array as copy of this string array. Its last symbol is zero.
	char* tToPlainArray() const {
		char *res = new char[size + 1];
		res[size] = '\0';
		tCopyBuffers(string, res, size);
		return res;
	}

	//! Gives index of first occurrence of given symbol.
	unsigned firstPosition(char c) const {
		for (unsigned i = 0; i < size; i++) {
			if (string[i] == c) {
				return i;
			}
		}
		return size;
	}

	//! Gives index of last occurrence of given symbol.
	unsigned lastPosition(char c) const {
		for (unsigned i = size - 1;; i--) {
			if (string[i] == c) {
				return i;
			}
			if (i == 0) {
				return size;
			}
		}
		return size;
	}

	//!Gives quantity of symbols in this text.
	int tCount(char symb) const {

		int result = 0;
		for (unsigned i = 0; i < size; i++) {
			if (string[i] == symb) {
				result++;
			}
		}

		return result;
	}

	//! Gives link to char in the array.
	char& tGetc(unsigned i) {
		return string[i];
	}

	bool tContains(tString pattern) const {
		for (unsigned i = 0; i < size; i++) {
			bool bad = 0;
			for (unsigned j = 0; (i + j < size) && (j < pattern.size); j++) {
				if (string[i + j] != pattern.string[j]) {
					bad = 1;
					break;
				}
			}
			if (!bad) {
				return 1;
			}
		}

		return 0;
	}

	bool operator<(const tString &a) const {
		for (unsigned i = 0; i < tMin<unsigned>(a.size, size); i++) {
			if (string[i] == a.string[i]) {
				continue;
			}
			return string[i] < a.string[i];
		}

		return 0;
	}

	char& operator[](int i) const {
		return string[i];
	}

	//! Divides this string into two by symbol in position pos. SYMBOL IS NOT INCLUDED!
	tString* tDivide(unsigned pos) const {
		tString *result = new tString[2];

		result[0] = tSubstring(0, pos - 1);
		result[1] = tSubstring(pos + 1, size - 1);

		return result;
	}

	//! Divides char array by bad symbols.
	tString* tParse(const char *bad_symbols, unsigned total_bad_symbols,
			unsigned &total) {
		tString *result = (tString*) calloc(size, sizeof(tString));
		unsigned total_ = 0;

		auto is_bad = [](char symb, const char *bad, unsigned tbs) {
			for (unsigned i = 0; i < tbs; i++) {
				if (symb == bad[i]) {
					return 1;
				}
			}

			return 0;
		};

		for (unsigned beg = 0, end = 0; beg < size;
				beg++, end = std::max(end, beg)) {

			if (is_bad(tGetc(beg), bad_symbols, total_bad_symbols)) {
				continue;
			}

			while (end < size
					&& !is_bad(tGetc(end), bad_symbols, total_bad_symbols)) {
				end++;
			}

			tString tmp = { this->string + beg, end - beg };

			result[total_] = (tmp);

			total_++;
			beg = end;
		}

		total = total_;
		return result;
	}

};

}

#endif
