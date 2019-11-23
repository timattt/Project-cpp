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
tList<map<unsigned, tPair<char*, unsigned> >*> __branches;

map<unsigned, tPair<char*, unsigned>>* __tCurrentBranch() {
	return __branches.tGetLastElement();
}

bool __tIsRunning() {
	return __branches.tGetSize() > 0;
}

//! Creates new branch where all new tString objects will be stored.
void __tNewBranch() {
	__branches.tAddLast(new map<unsigned, tPair<char*, unsigned>>());
}

//! Deletes current branch and frees all tString object called after last __tNewBranch call.
void __tFlushBranch() {
	map<unsigned, tPair<char*, unsigned>> *curr = __tCurrentBranch();
	for (auto i = curr->begin(); i != curr->end(); i++) {
		delete i->second.x;
	}

	__branches.tRemoveLast();
	delete curr;
}

//! Creates char array and store it in current branch. Gives local pointer.
unsigned __tAllocStr(unsigned sz) {
	if (!__tIsRunning()) {
		tThrowException("NO BRANCHES!");
	}
	char *str = new char[sz];

	unsigned pointer = free_pointer;
	free_pointer++;

	(*__tCurrentBranch())[pointer].x = str;
	(*__tCurrentBranch())[pointer].y = sz;
	return pointer;
}

unsigned __tStrSize(unsigned ptr) {
	return (*__tCurrentBranch())[ptr].y;
}

//! Changes size to existed tString.
void __tReallocStr(unsigned ptr, unsigned size_old, unsigned size_new) {
	char *str = (*__tCurrentBranch())[ptr].x;
	str = (char*) realloc(str, size_new);
	(*__tCurrentBranch())[ptr].x = str;
	(*__tCurrentBranch())[ptr].y = size_new;
}

//! Gives array of this pointer.
char* __tGetArray(unsigned ptr) {
	char *str = (*__tCurrentBranch())[ptr].x;
	return str;
}

class tString {
public:
	// Link to array
	char* __array() const {
		return __tGetArray(tPointer);
	}

	// Branch system pointer.
	unsigned tPointer;

	// Link to size
	unsigned size() const {
		return __tStrSize(tPointer);
	}

	template<typename T> tString(T number) :
			tPointer(__tAllocStr(0)) {

		// Create an output string stream
		std::ostringstream streamObj;
		//Add double to stream
		streamObj << std::scientific << number;

		const char *arr = streamObj.str().c_str();
		unsigned len = __tStrlen(arr);
		tSet(arr, len);
	}

	//! Creates new string. Determine size using __tStrlen function.
	tString(const char *str) :
			tPointer(__tAllocStr(__tStrlen(str))) {
		tCopyBuffers(str, __array(), size());
	}

	//! Creates empty string.
	tString() :
			tPointer(__tAllocStr(0)) {
	}

	tString(char c) :
			tPointer(__tAllocStr(1)) {
		__array()[0] = c;
	}

	//! Creates new string and copies data from given pointer.
	tString(char *str, unsigned len) :
			tPointer(__tAllocStr(len)) {
		tCopyBuffers(str, __array(), len);
	}

	tString tRemoveFractTail() {
		tString str = { *this };
		bool minus = 0;
		if (str.__array()[0] == '-') {
			minus = 1;
			str = str.tCropFirst();
		}

		tString e = str.tSubstring(str.size() - 5, str.size() - 1);
		str = str.tSubstring(0, str.size() - 6);

		//str.out();
		//e.out();

		tString num = e.tSubstring(2, e.size() - 1);
		unsigned n = num.tToInt();
		int sign = (e.tCount('+') > 0 ? 1 : -1);

		str = str.deleteSymbol('.');

		if (sign > 0) {
			for (unsigned i = 0; i < n; i++) {
				str += '0';
			}
			str = str.insertSymbol('.', n);
		} else {
			for (unsigned i = 0; i < n; i++) {
				str = tString('0') + str;
			}
			str = str.insertSymbol('.', 0);
		}

		while (str[str.size() - 1] == '0' || str[str.size() - 1] == '.') {
			if (str[str.size() - 1] == '.') {
				str = str.tCropLast();
				break;
			}
			str = str.tCropLast();
		}

		if (minus) {
			str = tString('-') + str;
		}

		return str;

	}

	//! Resizes this string to new size and copies data from given array.
	void tSet(const char *arr, unsigned len) {
		if (arr == NULL) {
			tThrowException("Can not set NULL to tString!");
		}
		tResize(len);
		tCopyBuffers(arr, __array(), len);
	}

	//! Returns string with deleted first symbol.
	tString tCropFirst() const {
		return tSubstring(1, size() - 1);
	}

	//! Returns string with deleted last symbol.
	tString tCropLast() const {
		return tSubstring(0, size() - 2);
	}

	//! Changes size of this string char array.
	void tResize(unsigned sz) {
		__tReallocStr(tPointer, size(), sz);
	}

	//! Writes this string to console.
	void tWrite() const {
		if (__array() == NULL) {
			tThrowException("STRING IS NULL!");
		}
		for (unsigned i = 0; i < size(); i++) {
			std::cout << __array()[i];
		}
		std::cout.flush();
	}

	void operator+=(tString other) {
		(*this) = (*this) + other;
	}

	void operator+=(char c) {
		(*this) = (*this) + tString(c);
	}

	tString operator+(tString other) const {
		tString result = { };
		result.tResize(size() + other.size());
		for (unsigned i = 0; i < size(); i++) {
			result[i] = __array()[i];
		}
		for (unsigned i = 0; i < other.size(); i++) {
			result[size() + i] = other[i];
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
		if (!tHasOnlyNumbers()) {
			tThrowException("CAN NOT CAST TO INT!");
		}
		char *arr = tToPlainArray();
		int result = std::atoi(arr);
		delete arr;
		return result;
	}

	//! Converts this string to float.
	float tToFloat() const {
		if (!tHasOnlyNumbers()) {
			tThrowException("CAN NOT CAST TO FLOAT!");
		}

		char *arr = tToPlainArray();
		float result = std::atof(arr);
		delete[] arr;

		return result;
	}

	//! Reads line and store it in this string.
	void tReadLine() {
		char c = 0;
		std::cout.flush();
		for (unsigned i = 0;; i++) {
			c = getchar();
			if (i == size() && c != '\n') {
				tResize(i + 1);
			}
			if (c == '\n') {
				break;
			}
			__array()[i] = c;
		}
		std::cout.flush();
	}

	//! Gives substring [start, end].
	tString tSubstring(unsigned start, unsigned end) const {
		if (start > end || end >= size()) {
			tThrowException("Out of string bounds!");
		}

		tString result = { };
		result.tResize(end - start + 1);
		for (unsigned i = start; i <= end; i++) {
			result.__array()[i - start] = __array()[i];
		}

		return result;
	}

	bool operator==(char c) const {
		return size() == 1 && c == __array()[0];
	}

	bool tIsPrefix(tString pattern) {
		if (pattern.size() > size()) {
			return 0;
		}
		for (unsigned i = 0; i < pattern.size(); i++) {
			if (pattern.__array()[i] != __array()[i]) {
				return 0;
			}
		}
		return 1;
	}

	//! Return true if string has only numbers AND POINT AND MINUS!
	bool tHasOnlyNumbers() const {
		if (tCount('.') > 1) {
			return 0;
		}
		for (unsigned i = 0; i < size(); i++) {
			if (!((__array()[i] >= '0' && __array()[i] <= '9')
					|| __array()[i] == '.' || __array()[i] == '-')) {
				return 0;
			}
		}

		return 1;
	}

	bool operator!=(char c) const {
		if (size() != 1) {
			return 1;
		}
		return c != __array()[0];
	}

	bool operator==(const tString &a) const {
		if (a.size() != size()) {
			return 0;
		}
		for (unsigned i = 0; i < a.size(); i++) {
			if (__array()[i] != a.__array()[i]) {
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
		char *res = new char[size() + 1];
		res[size()] = '\0';
		tCopyBuffers(__array(), res, size());
		return res;
	}

	//! Gives index of first occurrence of given symbol.
	unsigned firstPosition(char c) const {
		for (unsigned i = 0; i < size(); i++) {
			if (__array()[i] == c) {
				return i;
			}
		}
		return size();
	}

	unsigned firstPositionStr(tString pattern) {
		for (unsigned i = 0; i < size(); i++) {
			bool bad = 0;
			for (unsigned j = 0; (i + j < size()) && (j < pattern.size());
					j++) {
				if (__array()[i + j] != pattern.__array()[j]) {
					bad = 1;
					break;
				}
			}
			if (!bad) {
				return i;
			}
		}

		return size();
	}

	//! Gives index of last occurrence of given symbol.
	unsigned lastPosition(char c) const {
		for (unsigned i = size() - 1;; i--) {
			if (__array()[i] == c) {
				return i;
			}
			if (i == 0) {
				return size();
			}
		}
		return size();
	}

	//!Gives quantity of symbols in this text.
	int tCount(char symb) const {

		int result = 0;
		for (unsigned i = 0; i < size(); i++) {
			if (__array()[i] == symb) {
				result++;
			}
		}

		return result;
	}

	//! Gives link to char in the array.
	char& tGetc(unsigned i) {
		return __array()[i];
	}

	bool tContains(tString pattern) const {
		for (unsigned i = 0; i < size(); i++) {
			bool bad = 0;
			for (unsigned j = 0; (i + j < size()) && (j < pattern.size());
					j++) {
				if (__array()[i + j] != pattern.__array()[j]) {
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

	tString tCastofSymbol(char c) {
		tString res = { };
		for (unsigned i = 0; i < size(); i++) {
			if (__array()[i] != c) {
				res = res + __array()[i];
			}
		}
		return res;
	}

	bool operator<(const tString &a) const {
		for (unsigned i = 0; i < tMin<unsigned>(a.size(), size()); i++) {
			if (__array()[i] == a.__array()[i]) {
				continue;
			}
			return __array()[i] < a.__array()[i];
		}
		return size() < a.size();
	}

	char& operator[](int i) const {
		return __array()[i];
	}

	//! Divides this string into two by symbol in position pos. SYMBOL IS NOT INCLUDED!
	tString* tDivide(unsigned pos) const {
		tString *result = new tString[2];

		if (0 < pos && pos < size() - 1) {
			result[0] = tSubstring(0, pos - 1);
			result[1] = tSubstring(pos + 1, size() - 1);
		}
		if (pos == 0) {
			result[1] = this->tCropFirst();
		}
		if (pos == size() - 1) {
			result[0] = this->tCropLast();
		}

		return result;
	}

	// [0, pos] + c + [pos, size)
	tString insertSymbol(char c, unsigned pos) {
		char del = __array()[pos];
		tString *divs = tDivide(pos);
		tString res = divs[0] + del + c + divs[1];
		delete divs;

		return res;
	}

	tString deleteSymbol(char c) const {
		unsigned pos = firstPosition(c);
		tString *divs = tDivide(pos);
		tString res = divs[0] + divs[1];
		delete divs;

		return res;
	}

//! Divides char array by bad symbols.
	tString* tParse(const char *bad_symbols, unsigned total_bad_symbols,
			unsigned &total) {
		tString *result = (tString*) calloc(size(), sizeof(tString));
		unsigned total_ = 0;

		auto is_bad = [](char symb, const char *bad, unsigned tbs) {
			for (unsigned i = 0; i < tbs; i++) {
				if (symb == bad[i]) {
					return 1;
				}
			}

			return 0;
		};

		for (unsigned beg = 0, end = 0; beg < size();
				beg++, end = std::max(end, beg)) {

			if (is_bad(tGetc(beg), bad_symbols, total_bad_symbols)) {
				continue;
			}

			while (end < size()
					&& !is_bad(tGetc(end), bad_symbols, total_bad_symbols)) {
				end++;
			}

			tString tmp = { this->__array() + beg, end - beg };

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
