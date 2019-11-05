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

void __tNewBranch() {
	__branches.tAddLast(new map<unsigned, char*>());
}

void __tFlushBranch() {
	map<unsigned, char*> *curr = __tCurrentBranch();
	for (auto i = curr->begin(); i != curr->end(); i++) {
		delete i->second;
	}

	__branches.tRemoveLast();
	delete curr;
}

unsigned __tAllocStr(unsigned sz) {
	char *str = new char[sz];

	unsigned pointer = free_pointer;
	free_pointer++;

	(*__tCurrentBranch())[pointer] = str;
	return pointer;
}

void __tReallocStr(unsigned ptr, unsigned len) {
	char *str = (*__tCurrentBranch())[ptr];
	str = (char*) realloc(str, len);
	(*__tCurrentBranch())[ptr] = str;
}

char* __tGetArray(unsigned ptr) {
	char *str = (*__tCurrentBranch())[ptr];
	return str;
}

class tString {
public:
	unsigned tPointer;
	unsigned size;

	char *string;

	tString(const char *str) :
			tPointer(__tAllocStr(__tStrlen(str))), size(__tStrlen(str)), string(
					__tGetArray(tPointer)) {
		tCopyBuffers(str, string, size);
	}

	tString() :
			tPointer(__tAllocStr(0)), size(0), string(__tGetArray(tPointer)) {
	}

	tString(char *str, unsigned len) :
			tPointer(__tAllocStr(len)), size(len), string(__tGetArray(tPointer)) {
		tCopyBuffers(str, string, len);
	}

	void tSet(char *arr, unsigned len) {
		if (arr == NULL) {
			tThrowException("Can not set NULL to tString!");
		}
		tResize(len);
		tCopyBuffers(arr, string, len);
	}

	void tResize(unsigned sz) {
		size = sz;
		__tReallocStr(tPointer, sz);
		string = __tGetArray(tPointer);
	}

	void tWrite() const {
		for (unsigned i = 0; i < size; i++) {
			cout << string[i];
		}
	}

	void out() const {
		for (unsigned i = 0; i < size; i++) {
			cout << string[i];
		}
		std::cout << "\n";
		std::cout.flush();
	}

	int tToInt() {
		return std::atoi(string);
	}

	float tToFloat() {
		return std::atof(string);
	}

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
	tString tSubstring(unsigned start, unsigned end) {
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

	bool operator==(const tString &a) {
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

	char* tToPlainArray() {
		char *res = new char[size + 1];
		res[size] = '\0';
		tCopyBuffers(string, res, size);
		return res;
	}

	unsigned firstPosition(char c) {
		for (unsigned i = 0; i < size; i++) {
			if (string[i] == c) {
				return i;
			}
		}
		return size;
	}

	//!Gives quantity of symbols in this text.
	int tCount(char symb) {

		int result = 0;
		for (int i = 0; i < size; i++) {
			if (string[i] == symb) {
				result++;
			}
		}

		return result;
	}

	char& tGetc(unsigned i) {
		return string[i];
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

	//! Divides char array.
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
