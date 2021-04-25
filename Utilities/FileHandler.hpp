#pragma once

// Includes
//======================================================
#include <winnt.h>
#include <winbase.h>
#include <cstdlib>
#include <iostream>

#include "String.hpp"
#include "Utilities.hpp"
//======================================================


// Structures
//======================================================
class tFile {
	// Fields
	//--------------------------------
	// Name
	const char *name;

	// File handle
	HANDLE file_handle;

	// Mapping
	HANDLE file_mapping_handle;
	char *mapped_buffer;

	// Read line utility
	unsigned lines_passed = 0;

	// File reading
	char *curr_map_byte;

	// Size
	//! Quantity of bytes in this file. After these bytes goes [\0].
	unsigned sizeBytes;
	//--------------------------------

private:

	//!Closes file.
	void tCloseFile();

public:

	~tFile();

	const char* tGetName();

	template<typename T> void tWriteNum(T num) {
		std::sprintf(curr_map_byte, "%d", num);
		while (*curr_map_byte != '\0') {
			curr_map_byte++;
		}
	}

	template<typename T> void tWriteNum_fl(T num) {
		tString str = { num };
		tWrite(str);
	}

	//! Writes given line.
	//! If length is initialized then uses only first [length] symbols.
	//! If not then uses tStrlen() function.
	void tWriteLine(tString line);

	//! Writes given line. But without '\n'
	//! If length is initialized then uses only first [length] symbols.
	//! If not then uses tStrlen() function.
	void tWrite(tString line);

	//! Writes any symbol to current byte.
	void tWritec(char c);

	//! Cleares buffer.
	void tClearBuffer();

	tFile(const char *name_);

	tFile(tString name_);

	unsigned tGetSize();

	//! Returns mapped buffer.
	char* tGetBuffer();

	//! Returns pointer in file to start position.
	void tFlip();

	tString tReadAll();

	//! Returns next char in file.
	char tGetc();

	char* tGetCurrentPointer();

	unsigned tGetCurrentByte();

	void tSetCarriagePosition(unsigned pos);

	void tMovePointer(int dx);

	//! Starts file mapping. Use this before using tGetc(), tReadLine()...
	//! sz - ensures that file at least have sz bytes.
	//! If sz is zero then sizeBytes is initialized with tGetFileSize() function.
	char* tStartMapping(unsigned sz = 0);

	//!Stops mapping
	void tStopMapping();

	//! True if has any more symbol to read.
	bool tHasMoreSymbs();

	void tFill(char pat);

	void tPrintBytes();

	//! This function gives quantity of new line symbol that was passed on last tReadLine function call.
	unsigned tGetPassedLines();

	//! This function returns pointer to begin of this line which is located in mapped memory.
	//! And it gives it length.
	tString tReadLine();
};
//======================================================


// Global functions
//======================================================
void tCreateDotImg(tString fileName, tString imgName);
bool tFileExists(tString file);
void tShrink(tFile *&fl);
int tGetFileSize(const char *name);//!Gives file size in bytes.
//======================================================
