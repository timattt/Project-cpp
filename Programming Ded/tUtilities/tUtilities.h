#ifndef T_UTILITIES
#define T_UTILITIES

#include <windows.h>

namespace tUtilities {

void tAssert(bool val);
template<typename T> void tWriteBytes(const T &elem, char *dest);
void tThrowException(const char *message);
template<typename T> void tCopyBuffers(const T *from, T *to, unsigned length);
int tSymbolsCount(const char *line, char symb);
int tGetFileSize(const char *name);
int tStrlen(const char *str);

namespace {

// For the encapsulation!
class _MEM_ {
public:
	void* operator new(size_t s, char *p) {
		return p;
	}
	template<typename T> void __tWriteBytes(const T &elem, char *dest) {
		new (dest) T(elem);
	}
} __mem__;
}
//

//! Returns minimum of a and b.
int tMin(int a, int b) {
	return (a < b ? a : b);
}

template<typename T>bool tCompare(const T * a, const T * b, unsigned len) {
	for (unsigned i = 0; i < len; i++) {
		if (a[i] != b[i]) {
			return false;
		}
	}

	return true;
}

//! My own realization of assert function.
void tAssert(bool val) {
	if (!val) {
		tThrowException("Assertion error!");
	}
}

//!Compares two strings if variable size is initialized then comparation will be done size times.
//!Else comparation is continue until one string reachs its end.
int tStrcmp(const char *str1, const char *str2, int size = 0) {
	tAssert(str1 != NULL && str2 != NULL);
	if (size < 0) {
		size = tMin(tStrlen(str1), tStrlen(str2));
	}

	for (unsigned i = 0; i < (unsigned) size; i++) {
		if (str1[i] < str2[i]) {
			return -1;
		}
		if (str1[i] > str2[i]) {
			return 1;
		}
	}

	return 0;
}

//! Writes element into given bytes array.
template<typename T> void tWriteBytes(const T &elem, char *dest) {
	__mem__.__tWriteBytes(elem, dest);
}

//! Converts data from byte array into any element.
template<typename T> T& tConvertBytes(const char *data) {
	return (*(T*) (data));
}

//! Just throws exception and tells some information about this class.
void tThrowException(const char *message = "tException") {
	throw message;
}

//! Copies one buffer to another.
template<typename T> void tCopyBuffers(const T *from, T *to, unsigned length) {
	tAssert(from != NULL && to != NULL);
	for (unsigned i = 0; i < length; i++) {
		to[i] = from[i];
	}
}

//!Gives quantity of symbols in this text.
int tSymbolsCount(const char *line, char symb) {
	tAssert(line != NULL);
	if (!line) {
		return 0;
	}

	int result = 0;
	for (int i = 0; line[i]; i++) {
		if (line[i] == symb) {
			result++;
		}
	}

	return result;
}

//!Gives file size in bytes.
int tGetFileSize(const char *name) {
	tAssert(name != NULL);
	WIN32_FIND_DATA data = { };
	FindFirstFileA(name, &data);
	return data.nFileSizeLow;
}

//!Gives quantity of chars in this char array.
int tStrlen(const char *str) {
	tAssert(str != NULL);
	const char *s = str;
	for (; *s; s++)
		;
	return s - str;
}

}

#endif
