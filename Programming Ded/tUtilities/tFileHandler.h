#ifndef T_FILE_HANDLER
#define T_FILE_HANDLER

#include <windows.h>
#include <bits/stdc++.h>
#include "tUtilities.h"
#include "tString.h"

using namespace tUtilities;

namespace tFileHandler {

void tCreateDotImg(tString fileName, tString imgName) {
	tString result = tString("dot -Tpng ") + fileName + tString(" -o ")
			+ imgName + tString(".png");
	char *command = result.tToPlainArray();
	system(command);
}

bool tFileExists(tString file) {
	WIN32_FIND_DATA FindFileData;
	char *arr = file.tToPlainArray();
	HANDLE handle = FindFirstFile(arr, &FindFileData);
	bool found = (handle != INVALID_HANDLE_VALUE );
	if (found) {
		FindClose(handle);
	}
	return found;
}

class tFile {

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

	//!Closes file.
	void tCloseFile() {
		CloseHandle(file_handle);
	}

public:
	~tFile() {
		tStopMapping();
		tCloseFile();
	}

	const char* tGetName() {
		return name;
	}

	template<typename T> void tWriteNum(T num) {
		std::sprintf(curr_map_byte, "%d", num);
		while (*curr_map_byte != '\0') {
			curr_map_byte++;
		}
	}

	template<typename T> void tWriteNum_fl(T num) {
		tString str = { num };
		str = str.tRemoveFractTail();
		tWrite(str);
	}

	//! Writes given line.
	//! If length is initialized then uses only first [length] symbols.
	//! If not then uses tStrlen() function.
	void tWriteLine(tString line) {
		if (mapped_buffer + sizeBytes < curr_map_byte + line.size()) {
			tThrowException("Can not write line!");
		}
		tCopyBuffers(line.__array(), curr_map_byte, line.size());
		curr_map_byte += line.size();

		tWritec('\n');
	}

	//! Writes given line. But without '\n'
	//! If length is initialized then uses only first [length] symbols.
	//! If not then uses tStrlen() function.
	void tWrite(tString line) {
		if (mapped_buffer + sizeBytes < curr_map_byte + line.size()) {
			tThrowException("Can not write line!");
		}
		tCopyBuffers(line.__array(), curr_map_byte, line.size());
		curr_map_byte += line.size();
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

	tFile(tString name_) :
			name(name_.tToPlainArray()), file_handle(INVALID_HANDLE_VALUE), file_mapping_handle(
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

	unsigned tGetSize() {
		return sizeBytes;
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

	tString tReadAll() {
		tString res = { };
		while (tHasMoreSymbs()) {
			res += (tReadLine() + '\n');
		}
		return res;
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

		//std::cout << "new symbol: " << (int)(result) << "\n";
		return result;
	}

	char* tGetCurrentPointer() {
		return curr_map_byte;
	}

	unsigned tGetCurrentByte() {
		return curr_map_byte - mapped_buffer;
	}

	void tSetCarriagePosition(unsigned pos) {
		if (mapped_buffer == NULL) {
			tThrowException("File is not mapped!");
		}
		curr_map_byte = mapped_buffer + pos;
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

	void tFill(char pat) {
		for (unsigned i = 0; i < sizeBytes; i++) {
			mapped_buffer[i] = pat;
		}
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

	//! This function gives quantity of new line symbol that was passed on last tReadLine function call.
	unsigned tGetPassedLines() {
		return lines_passed;
	}

	//! This function returns pointer to begin of this line which is located in mapped memory.
	//! And it gives it length.
	tString tReadLine() {
		if (!tHasMoreSymbs()) {
			tThrowException("Out of file range!");
		}

		lines_passed = 0;
		while (curr_map_byte < mapped_buffer + sizeBytes
				&& (*curr_map_byte == '\r' || *curr_map_byte == '\n'
						|| *curr_map_byte == '\0')) {
			if (*curr_map_byte == '\n') {
				lines_passed++;
			}
			curr_map_byte++;
		}

		char *beg = curr_map_byte;
		unsigned curlength = 0;

		for (;
				*curr_map_byte != '\n' && *curr_map_byte != '\0'
						&& *curr_map_byte != '\r'; curr_map_byte++, curlength++)
			;

		while (curr_map_byte < mapped_buffer + sizeBytes
				&& (*curr_map_byte == '\r' || *curr_map_byte == '\n'
						|| *curr_map_byte == '\0')) {
			if (*curr_map_byte == '\n') {
				lines_passed++;
			}
			curr_map_byte++;
		}

		tString result = { };

		result.tSet(beg, curlength);

		return result;
	}
};

void tShrink(tFile *&fl) {
	unsigned len = fl->tGetCurrentByte();
	char *buf = new char[len];
	tCopyBuffers(fl->tGetBuffer(), buf, len);
	tString name = fl->tGetName();
	delete fl;
	char *name_tmp = name.tToPlainArray();
	DeleteFileA(name_tmp);
	fl = new tFile(name);
	fl->tStartMapping(len);
	tCopyBuffers(buf, fl->tGetBuffer(), len);
	fl->tStopMapping();
}

}

#endif
