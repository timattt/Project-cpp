#pragma once

// Includes
//======================================================
#include <map>
#include <stdio.h>

#include "../Storage/tList.hpp"
#include "Utilities.hpp"
//======================================================


using std::map;


// Global functions
//======================================================
int 									__tStrlen		(const char *str); //!Gives quantity of chars in this char array.
map<unsigned, tPair<char*, unsigned>>* 	__tCurrentBranch();
char* 									__tGetArray		(unsigned ptr);//! Gives array of this pointer.
void 									__tNewBranch	(); //! Creates new branch where all new tString objects will be stored.
bool 									__tIsRunning	();
void 									__tFlushBranch	(); //! Deletes current branch and frees all tString object called after last __tNewBranch call.
unsigned 								__tAllocStr		(unsigned sz); //! Creates char array and store it in current branch. Gives local pointer.
unsigned 								__tStrSize		(unsigned ptr);
void 									__tReallocStr	(unsigned ptr, unsigned size_old, unsigned size_new);//! Changes tSize to existed tString.
//======================================================


// Structures
//======================================================
class tString {
private:
	// Fields
	//-------------------------
	unsigned tPointer; // Branch system pointer.
	//-------------------------

public:

	// Constructors
	//-------------------------
	tString(float number);
	tString(double number);
	tString(int number);
	tString(unsigned number);
	tString(const char *str);//! Creates new string. Determine tSize using __tStrlen function.
	tString();//! Creates empty string.
	tString(char c);
	tString(char *str, unsigned len);//! Creates new string and copies data from given pointer.
	//-------------------------


	// Const
	//-------------------------
	char* 		__tArray		() 								const;//! Link to array
	unsigned	tSize			() 								const;//! Link to tSize
	tString 	tCropFirst		() 								const;//! Returns string with deleted first symbol.
	tString 	tCropLast		() 								const;//! Returns string with deleted last symbol.
	void 		tWrite			() 								const;//! Writes this string to console.
	tString 	operator+		(tString other) 				const;//! Generates new string with concatenation.
	void 		tOut			() 								const;//! Writes this string in the new line. And flushes stream.
	int 		tToInt			() 								const;//! Converts this string to int.
	float 		tToFloat		() 								const;//! Converts this string to float.
	tString 	tSubstring		(unsigned start, unsigned end) 	const;//! Gives substring [start, end].
	bool 		operator==		(char c) 						const;//! True if this string consists of one given symbol.
	bool 		tHasOnlyNumbers	() 								const;//! Return true if string has only numbers AND POINT AND MINUS!
	bool 		operator!=		(char c) 						const;//! False if this string consists of one given symbol.
	bool 		operator==		(const tString &a) 				const;//! True if this string equals to given.
	tString 	tCrop			(char a, char b) 				const;//! Return everything that lies between symbol a and symbol b. It takes FIRST position of a and LAST position of b. If there is no such symbols then no changes will be made.
	tString 	tReplace		(char a, char b) 				const;//! Replaces all old chars with new.
	char* 		tToPlainArray	() 								const;//! Generate new array as copy of this string array. Its last symbol is zero.
	unsigned 	tFirstPosition	(char c) 						const;//! Gives index of first occurrence of given symbol.
	unsigned 	tLastPosition	(char c) 						const;//! Gives index of last occurrence of given symbol.
	int 		tCount			(char symb) 					const;//! Gives quantity of symbols in this text.
	bool 		tContains		(tString pattern) 				const;//! True if contains given string.
	bool 		operator<		(const tString &a) 				const;//! Compare by alphabet.
	char& 		operator[]		(int i) 						const;//! Gives i char.
	tString* 	tDivide			(unsigned pos) 					const;//! Divides this string into two by symbol in position pos. SYMBOL IS NOT INCLUDED!
	tString 	tDeleteSymbol	(char c) 						const;//! Deletes first copy of given symbols.
	unsigned 	tFirstPositionStr(tString pattern) 				const;//! Gives index of first occurrence.
	bool 		tIsPrefix		(tString pattern) 				const;//! True if this string has given prefix.
	char& 		tGetc			(unsigned i) 					const;//! Gives link to char in the array.
	tString 	tCastofSymbol	(char c) 						const;//! Deletes given symbol.
	tString 	tInsertSymbol	(char c, unsigned pos) 			const;//! [0, pos] + c + [pos, tSize)
	//-------------------------


	// Other functions
	//-------------------------
	void 		tSet			(const char *arr, unsigned len);//! Resizes this string to new tSize and copies data from given array.
	void 		tResize			(unsigned sz);//! Changes tSize of this string char array.
	void 		operator+=		(tString other);
	void 		operator+=		(char c);
	void 		operator+=		(int c);
	void 		operator+=		(unsigned c);
	void 		tReadLine		();//! Reads line and store it in this string.
	tString* 	tParse			(const char *bad_symbols, unsigned total_bad_symbols, unsigned &total);//! Divides char array by bad symbols.
	//-------------------------

};
//======================================================
