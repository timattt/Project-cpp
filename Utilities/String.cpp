#include "String.hpp"

tString::tString(float number) :
		tPointer(__tAllocStr(0)) {
	char tmp[50] = { 0 };
	sprintf(tmp, "%.2f", number);
	tSet(tmp, strlen(tmp));
}
tString::tString(double number) :
		tPointer(__tAllocStr(0)) {
	char tmp[50] = { 0 };
	sprintf(tmp, "%.2f", number);
	tSet(tmp, strlen(tmp));
}
tString::tString(int number) :
		tPointer(__tAllocStr(0)) {
	char tmp[50] = { 0 };
	sprintf(tmp, "%d", number);
	tSet(tmp, strlen(tmp));
}
tString::tString(unsigned number) :
		tPointer(__tAllocStr(0)) {
	char tmp[50] = { 0 };
	sprintf(tmp, "%d", number);
	tSet(tmp, strlen(tmp));
}

//!Gives quantity of chars in this char array.
int __tStrlen(const char *str) {
	NOT_NULL(str);
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

//! Creates new branch where all new tString objects will be stored.
void __tNewBranch() {
	__branches.tAddLast(new map<unsigned, tPair<char*, unsigned>>());
}

bool __tIsRunning() {
	if (__branches.tGetSize() == 0) {
		// Default branch
		__tNewBranch();
	}
	return __branches.tGetSize() > 0;
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
	ENSURE (__tIsRunning(), "NO BRANCHES!");

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

//! Changes tSize to existed tString.
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

	// Link to array
	char* tString::__tArray() const {
		return __tGetArray(tPointer);
	}


	// Link to tSize
	unsigned tString::tSize() const {
		return __tStrSize(tPointer);
	}

	//! Creates new string. Determine tSize using __tStrlen function.
	tString::tString(const char *str) :
			tPointer(__tAllocStr(__tStrlen(str))) {
		tCopyBuffers(str, __tArray(), tSize());
	}

	//! Creates empty string.
	tString::tString() :
			tPointer(__tAllocStr(0)) {
	}

	tString::tString(char c) :
			tPointer(__tAllocStr(1)) {
		__tArray()[0] = c;
	}

	//! Creates new string and copies data from given pointer.
	tString::tString(char *str, unsigned len) :
			tPointer(__tAllocStr(len)) {
		tCopyBuffers(str, __tArray(), len);
	}

	//! Resizes this string to new tSize and copies data from given array.
	void tString::tSet(const char *arr, unsigned len) {
		NOT_NULL(arr);
		tResize(len);
		tCopyBuffers(arr, __tArray(), len);
	}

	//! Returns string with deleted first symbol.
	tString tString::tCropFirst() const {
		return tSubstring(1, tSize() - 1);
	}

	//! Returns string with deleted last symbol.
	tString tString::tCropLast() const {
		return tSubstring(0, tSize() - 2);
	}

	//! Changes tSize of this string char array.
	void tString::tResize(unsigned sz) {
		__tReallocStr(tPointer, tSize(), sz);
	}

	//! Writes this string to console.
	void tString::tWrite() const {
		NOT_NULL(__tArray());
		for (unsigned i = 0; i < tSize(); i++) {
			std::cout << __tArray()[i];
		}
		std::cout.flush();
	}

	void tString::operator+=(tString other) {
		(*this) = (*this) + other;
	}

	void tString::operator+=(char c) {
		(*this) = (*this) + tString(c);
	}

	tString tString::tReplace(char a, char b) const {
		tString res = {};
		for (unsigned i = 0; i < tSize(); i++) {
			char c = __tArray()[i];
			if (c == a) {
				res += b;
			} else {
				res += c;
			}
		}
		return res;
	}

	void tString::operator+=(int c) {
		(*this) = (*this) + tString(c);
	}

	void tString::operator+=(unsigned c) {
		(*this) = (*this) + tString(c);
	}

	tString tString::operator+(tString other) const {
		tString result = { };
		result.tResize(tSize() + other.tSize());
		for (unsigned i = 0; i < tSize(); i++) {
			result[i] = __tArray()[i];
		}
		for (unsigned i = 0; i < other.tSize(); i++) {
			result[tSize() + i] = other[i];
		}

		return result;
	}

	//! Writes this string in the new line. And flushes stream.
	void tString::tOut() const {
		tWrite();
		std::cout << "\n";
		std::cout.flush();
	}

	//! Converts this string to int.
	int tString::tToInt() const {
		CHECK (!tHasOnlyNumbers(), "CAN NOT CAST TO INT!");

		char *arr = tToPlainArray();
		int result = std::atoi(arr);
		return result;
	}

	//! Converts this string to float.
	float tString::tToFloat() const {
		CHECK (!tHasOnlyNumbers(), "CAN NOT CAST TO INT!");

		char *arr = tToPlainArray();
		float result = std::atof(arr);

		return result;
	}

	//! Reads line and store it in this string.
	void tString::tReadLine() {
		char c = 0;
		std::cout.flush();
		for (unsigned i = 0;; i++) {
			c = getchar();
			if (i == tSize() && c != '\n') {
				tResize(i + 1);
			}
			if (c == '\n') {
				break;
			}
			__tArray()[i] = c;
		}
		std::cout.flush();
	}

	//! Gives substring [start, end].
	tString tString::tSubstring(unsigned start, unsigned end) const {
		CHECK (start > end || end >= tSize(), "Out of string bounds!");

		tString result = { };
		result.tResize(end - start + 1);
		for (unsigned i = start; i <= end; i++) {
			result.__tArray()[i - start] = __tArray()[i];
		}

		return result;
	}

	bool tString::operator==(char c) const {
		return tSize() == 1 && c == __tArray()[0];
	}

	bool tString::tIsPrefix(tString pattern) const {
		if (pattern.tSize() > tSize()) {
			return 0;
		}
		for (unsigned i = 0; i < pattern.tSize(); i++) {
			if (pattern.__tArray()[i] != __tArray()[i]) {
				return 0;
			}
		}
		return 1;
	}

	//! Return true if string has only numbers AND POINT AND MINUS!
	bool tString::tHasOnlyNumbers() const {
		if (tCount('.') > 1) {
			return 0;
		}
		for (unsigned i = 0; i < tSize(); i++) {
			if (!((__tArray()[i] >= '0' && __tArray()[i] <= '9')
					|| __tArray()[i] == '.' || __tArray()[i] == '-')) {
				return 0;
			}
		}

		return 1;
	}

	bool tString::operator!=(char c) const {
		if (tSize() != 1) {
			return 1;
		}
		return c != __tArray()[0];
	}

	bool tString::operator==(const tString &a) const {
		if (a.tSize() != tSize()) {
			return 0;
		}
		for (unsigned i = 0; i < a.tSize(); i++) {
			if (__tArray()[i] != a.__tArray()[i]) {
				return 0;
			}
		}
		return 1;
	}

	//! Return everything that lies between symbol a and symbol b.
	//! It takes FIRST position of a and LAST position of b.
	//! If there is no such symbols then no changes will be made.
	tString tString::tCrop(char a, char b) const {
		if (tCount(a) > 0 && tCount(b) > 0) {
			unsigned pos1 = tFirstPosition(a);
			unsigned pos2 = tLastPosition(b);
			tString res = tSubstring(pos1 + 1, pos2 - 1);

			return res;
		} else {
			return *this;
		}
	}

	//! Generate new array as copy of this string array. Its last symbol is zero.
	char* tString::tToPlainArray() const {
		tString str = *this + '\0';
		return str.__tArray();
	}

	//! Gives index of first occurrence of given symbol.
	unsigned tString::tFirstPosition(char c) const {
		for (unsigned i = 0; i < tSize(); i++) {
			if (__tArray()[i] == c) {
				return i;
			}
		}
		return tSize();
	}

	unsigned tString::tFirstPositionStr(tString pattern) const {
		for (unsigned i = 0; i < tSize(); i++) {
			bool bad = 0;
			for (unsigned j = 0; (i + j < tSize()) && (j < pattern.tSize());
					j++) {
				if (__tArray()[i + j] != pattern.__tArray()[j]) {
					bad = 1;
					break;
				}
			}
			if (!bad) {
				return i;
			}
		}

		return tSize();
	}

	//! Gives index of last occurrence of given symbol.
	unsigned tString::tLastPosition(char c) const {
		for (unsigned i = tSize() - 1;; i--) {
			if (__tArray()[i] == c) {
				return i;
			}
			if (i == 0) {
				return tSize();
			}
		}
		return tSize();
	}

	//!Gives quantity of symbols in this text.
	int tString::tCount(char symb) const {

		int result = 0;
		for (unsigned i = 0; i < tSize(); i++) {
			if (__tArray()[i] == symb) {
				result++;
			}
		}

		return result;
	}

	//! Gives link to char in the array.
	char& tString::tGetc(unsigned i) const {
		return __tArray()[i];
	}

	bool tString::tContains(tString pattern) const {
		for (unsigned i = 0; i < tSize(); i++) {
			bool bad = 0;
			for (unsigned j = 0; (j < pattern.tSize());
					j++) {
				if (i + j >= tSize()) {
					bad = 1;
					break;
				}
				if (__tArray()[i + j] != pattern.__tArray()[j]) {

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

	tString tString::tCastofSymbol(char c) const {
		tString res = { };
		for (unsigned i = 0; i < tSize(); i++) {
			if (__tArray()[i] != c) {
				res = res + __tArray()[i];
			}
		}
		return res;
	}

	bool tString::operator<(const tString &a) const {
		for (unsigned i = 0; i < tMin<unsigned>(a.tSize(), tSize()); i++) {
			if (__tArray()[i] == a.__tArray()[i]) {
				continue;
			}
			return __tArray()[i] < a.__tArray()[i];
		}
		return tSize() < a.tSize();
	}

	char& tString::operator[](int i) const {
		return __tArray()[i];
	}

	//! Divides this string into two by symbol in position pos. SYMBOL IS NOT INCLUDED!
	tString* tString::tDivide(unsigned pos) const {
		tString *result = new tString[2];

		if (0 < pos && pos < tSize() - 1) {
			result[0] = tSubstring(0, pos - 1);
			result[1] = tSubstring(pos + 1, tSize() - 1);
		}
		if (pos == 0) {
			result[1] = this->tCropFirst();
		}
		if (pos == tSize() - 1) {
			result[0] = this->tCropLast();
		}

		return result;
	}

	// [0, pos] + c + [pos, tSize)
	tString tString::tInsertSymbol(char c, unsigned pos) const {
		char del = __tArray()[pos];
		tString *divs = tDivide(pos);
		tString res = divs[0] + del + c + divs[1];
		delete divs;

		return res;
	}

	tString tString::tDeleteSymbol(char c) const {
		unsigned pos = tFirstPosition(c);
		tString *divs = tDivide(pos);
		tString res = divs[0] + divs[1];
		delete divs;

		return res;
	}

	bool is_bad(char symb, const char *bad, unsigned tbs) {
			for (unsigned i = 0; i < tbs; i++) {
				if (symb == bad[i]) {
					return 1;
				}
			}

			return 0;
	}

	//! Divides char array by bad symbols.
	tString* tString::tParse(const char *bad_symbols, unsigned total_bad_symbols,
			unsigned &total) {
		tString *result = (tString*) calloc(tSize(), sizeof(tString));
		unsigned total_ = 0;



		for (unsigned beg = 0, end = 0; beg < tSize();
				beg++, end = std::max(end, beg)) {

			if (is_bad(tGetc(beg), bad_symbols, total_bad_symbols)) {
				continue;
			}

			while (end < tSize()
					&& !is_bad(tGetc(end), bad_symbols, total_bad_symbols)) {
				end++;
			}

			tString tmp = { this->__tArray() + beg, end - beg };

			result[total_] = (tmp);

			total_++;
			beg = end;
		}

		total = total_;
		return result;
	}

