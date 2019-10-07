#ifndef T_FILE_HANDLER
#define T_FILE_HANDLER

#include <windows.h>
#include <bits/stdc++.h>
#include "tUtilities.h"

using namespace tUtilities;

namespace tFileHandler {

class tFile {

	// Name
	const char *name;

	// File handle
	HANDLE file_handle;

	// Mapping
	HANDLE file_mapping_handle;
	char *mapped_buffer;

	// File reading
	char *curr_map_byte;

	// Size
	//! Quantity of bytes in this file. After these bytes goes [\0].
	unsigned sizeBytes;

	~tFile() {
		tStopMapping();
		tCloseFile();
	}

	//!Closes file.
	void tCloseFile() {
		CloseHandle(file_handle);
	}

public:

	//! Writes given line.
	//! If length is initialized then uses only first [length] symbols.
	//! If not then uses tStrlen() function.
	void tWriteLine(const char *line, unsigned length = 0) {
		unsigned len = 0;
		if (line == NULL
				|| mapped_buffer + sizeBytes
						< curr_map_byte
								+ (len = (length == 0 ? tStrlen(line) : length))) {
			tThrowException("Can not write line!");
		}
		tCopyBuffers(line, curr_map_byte, len);
		curr_map_byte += len;

		tWritec('\n');
	}

	//! Writes any symbol to current byte.
	void tWritec(char c) {
		if (curr_map_byte >= mapped_buffer + sizeBytes) {
			tThrowException("No more space in file!");
		}
		*curr_map_byte = c;
		curr_map_byte++;
	}

	//! Cleares buffer.
	void tClearBuffer() {
		if (mapped_buffer == NULL) {
			tThrowException("Not mapped!");
		}
		for (unsigned i = 0; i < sizeBytes; i++) {
			mapped_buffer[i] = 0;
		}
	}

	tFile(const char *name_) :
			name(name_), file_handle(INVALID_HANDLE_VALUE), file_mapping_handle(
			INVALID_HANDLE_VALUE), mapped_buffer(NULL), curr_map_byte(NULL), sizeBytes(
					0) {
		if (name == NULL) {
			tThrowException("Name is NULL!");
		}

		file_handle = CreateFileA(name, GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_WRITE, NULL,
		OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

		if (file_handle == INVALID_HANDLE_VALUE) {
			tThrowException("Invalid file handle!");
		}

	}

	//! Returns mapped buffer.
	char* tGetBuffer() {
		if (mapped_buffer == NULL) {
			tThrowException("File is not mapped!");
		}
		return mapped_buffer;
	}

	//! Returns pointer in file to start position.
	void tFlip() {
		curr_map_byte = mapped_buffer;
	}

	//! Returns next char in file.
	char tGetc() {
		if (mapped_buffer == NULL) {
			tThrowException("Not mapped!");
		}
		if (curr_map_byte >= mapped_buffer + sizeBytes) {
			tThrowException("Out of file range!");
		}

		char result = *curr_map_byte;
		curr_map_byte++;

		return result;
	}

	char* tGetCurrentPointer() {
		return curr_map_byte;
	}

	void tMovePointer(int dx) {
		curr_map_byte += dx;
	}

	//! Starts file mapping. Use this before using tGetc(), tReadLine()...
	//! sz - ensures that file at least have sz bytes.
	//! If sz is zero then sizeBytes is initialized with tGetFileSize() function.
	char* tStartMapping(unsigned sz = 0) {
		tStopMapping();

		file_mapping_handle = CreateFileMappingA(file_handle, NULL,
		PAGE_READWRITE, 0, sz, NULL);
		if (file_mapping_handle == INVALID_HANDLE_VALUE) {
			tThrowException("Invalid mapping handle!");
		}

		mapped_buffer = (char*) MapViewOfFile(file_mapping_handle,
		FILE_MAP_ALL_ACCESS, 0, 0, sz);
		if (mapped_buffer == NULL) {
			tThrowException("Mapped buffer is NULL!");
		}

		tFlip();

		if (sz != 0) {
			sizeBytes = sz;
		} else {
			sizeBytes = tGetFileSize(name);
		}

		return mapped_buffer;
	}

	//!Stops mapping
	void tStopMapping() {
		if (mapped_buffer != NULL) {
			UnmapViewOfFile(mapped_buffer);
		}
		if (file_mapping_handle != INVALID_HANDLE_VALUE) {
			CloseHandle(file_mapping_handle);
		}

		mapped_buffer = NULL;
		file_mapping_handle = INVALID_HANDLE_VALUE;
	}

	//! True if has any more symbol to read.
	bool tHasMoreSymbs() {
		if (mapped_buffer == NULL) {
			tThrowException("Not mapped!");
		}
		return curr_map_byte < mapped_buffer + sizeBytes;
	}

	void tPrintBytes() {
		if (mapped_buffer == NULL) {
			tThrowException("Not mapped!");
		}
		std::cout << "[";
		for (unsigned i = 0; i < sizeBytes; i++) {
			std::cout << (int) (tGetBuffer()[i]);
			if (i + 1 < sizeBytes) {
				std::cout << " ";
			}
		}
		std::cout << "]\n";
	}

	//! This function returns pointer to begin of this line which is located in mapped memory.
	//! And it gives it length.
	char* tReadLine(unsigned &length) {
		if (!tHasMoreSymbs()) {
			tThrowException("Out of file range!");
		}

		char *beg = curr_map_byte;
		unsigned curlength = 0;

		for (;
				*curr_map_byte != '\n' && *curr_map_byte != '\0'
						&& *curr_map_byte != '\r'; curr_map_byte++, curlength++)
			;

		length = curlength;
		while (curr_map_byte < mapped_buffer + sizeBytes
				&& (*curr_map_byte == '\r' || *curr_map_byte == '\n'
						|| *curr_map_byte == '\0')) {
			curr_map_byte++;
		}

		return beg;
	}
};

}

#endif
