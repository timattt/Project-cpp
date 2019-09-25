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

void tAssert(bool val) {
	if (!val) {
		tThrowException("Assertion error!");
	}
}

template<typename T> void tWriteBytes(const T &elem, char *dest) {
	__mem__.__tWriteBytes(elem, dest);
}

//! Just throws exception and tells some information about this class.
void tThrowException(const char *message = "tException") {
	throw message;
}

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
