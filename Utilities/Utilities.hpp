#pragma once

// Includes
//===========================================
#include <stddef.h>
#include <cstdlib>
#include <winbase.h>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <cstring>
//===========================================


// Defines
//===========================================
#define ERROR_MESSAGE_LENGTH 100
#define NOT_NULL(A) do {if((A) == NULL) {throw new tException(__LINE__, __FILE__, "Null pointer exception!");}} while(0);
#define THROW_T_EXCEPTION(A) do {throw new tException(__LINE__, __FILE__, A);}while(0);
#define ENSURE(A, B) do {if (!(A)) {THROW_T_EXCEPTION(B)}} while(0);
#define CHECK(A, B) do {if ((A)) {THROW_T_EXCEPTION(B)}} while(0);
#define GUARD(A) do { try { A ;} catch (tException * ex) { ex->print();}; } while(0);
//===========================================


// Structures
//===========================================
struct tException {
	int lineNumber;
	const char * fileName;
	char message[ERROR_MESSAGE_LENGTH];
public:
	tException(int ln, const char * fn, const char * mes);
	void print();
};
template<typename A, typename B> struct tPair {
	A x;
	B y;
};
class _MEM_ {
public:
	_MEM_();
	void* operator new(size_t s, char *p);
	template<typename T> void __tWriteBytes(const T &elem, char *dest) {
		new (dest) T(elem);
	}
	template<typename T> void __tWriteBytes_rvalue(T &&elem, char *dest) {
		char *beg = (char*) (&elem);
		for (unsigned i = 0; i < sizeof(T); i++) {
			dest[i] = beg[i];
		}
	}
};
//===========================================


// Global functions
//===========================================
char tReadCharFromLine();
double tFloor(double val);
double tCeiling(double val);
void tSwapBuffers(char *a, char *b, unsigned len);
//===========================================


// Global functions with templates
//===========================================
template<typename T = int> T tUp(T a, T b) {
	return a / b + (a % b == 0 ? 0 : 1);
}

//! Returns minimum of a and b.
template<typename T = int> T tMin(T a, T b) {
	return (a < b ? a : b);
}

//! Returns minimum of a and b.
template<typename T = int> T tMax(T a, T b) {
	return (b < a ? a : b);
}
//! a^n
template<typename T = int, typename K = int> T tBinpow(K a, T n) {
	T res = 1;
	while (n) {
		if (n & 1)
			res *= a;
		a *= a;
		n >>= 1;
	}
	return res;
}
//! Fills array
template<typename T> void tFill(T *arr, T templ, unsigned len) {
	for (unsigned i = 0; i < len; i++) {
		arr[i] = templ;
	}
}
//! Swaps two elements
template<typename T> void tSwap(T &a, T &b) {
	T tmp = a;
	a = b;
	b = tmp;
}
//! Writes element into given bytes array.
template<typename T> void tWriteBytes(const T &elem, char *dest) {
	_MEM_ a = {};
	a.__tWriteBytes(elem, dest);
}
//! Writes element into given bytes array.
template<typename T> void tWriteBytes_rvalue(T &&elem, char *dest) {
	_MEM_ a = {};
	a.__tWriteBytes_rvalue(elem, dest);
}
//! Converts data from byte array into any element.
template<typename T> T& tConvertBytes(char *data) {
	return (*(T*) (data));
}
//! Copies one buffer to another.
template<typename T> void tCopyBuffers(const T *from, T *to, unsigned length) {
	NOT_NULL(from);
	NOT_NULL(to);
	for (unsigned i = 0; i < length; i++) {
		to[i] = from[i];
	}
}
//! Compare arrays.
template<typename T> bool tCompare(const T *a, const T *b, unsigned len) {
	for (unsigned i = 0; i < len; i++) {
		if (a[i] != b[i]) {
			return false;
		}
	}

	return true;
}
//===========================================
