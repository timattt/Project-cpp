#include "FileHandler.hpp"

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

//!Closes file.
void tFile::tCloseFile() {
	CloseHandle(file_handle);
}

tFile::~tFile() {
	tStopMapping();
	tCloseFile();
}

const char* tFile::tGetName() {
	return name;
}

//! Writes given line.
//! If length is initialized then uses only first [length] symbols.
//! If not then uses tStrlen() function.
void tFile::tWriteLine(tString line) {
	CHECK(mapped_buffer + sizeBytes < curr_map_byte + line.tSize(), "Can not write line!");

	tCopyBuffers(line.__tArray(), curr_map_byte, line.tSize());
	curr_map_byte += line.tSize();

	tWritec('\n');
}

//! Writes given line. But without '\n'
//! If length is initialized then uses only first [length] symbols.
//! If not then uses tStrlen() function.
void tFile::tWrite(tString line) {
	CHECK(mapped_buffer + sizeBytes < curr_map_byte + line.tSize(), "Can not write line!");

	tCopyBuffers(line.__tArray(), curr_map_byte, line.tSize());
	curr_map_byte += line.tSize();
}

//! Writes any symbol to current byte.
void tFile::tWritec(char c) {
	CHECK(curr_map_byte >= mapped_buffer + sizeBytes, "No more space in file!");

	*curr_map_byte = c;
	curr_map_byte++;
}

//! Cleares buffer.
void tFile::tClearBuffer() {
	NOT_NULL(mapped_buffer);

	for (unsigned i = 0; i < sizeBytes; i++) {
		mapped_buffer[i] = 0;
	}
}

tFile::tFile(const char *name_) :
		name(name_), file_handle(INVALID_HANDLE_VALUE), file_mapping_handle(
		INVALID_HANDLE_VALUE), mapped_buffer(NULL), curr_map_byte(NULL), sizeBytes(
				0) {
	NOT_NULL(name);

	file_handle = CreateFileA(name, GENERIC_READ | GENERIC_WRITE,
	FILE_SHARE_WRITE, NULL,
	OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	CHECK (file_handle == INVALID_HANDLE_VALUE, "Invalid file handle!");
}

tFile::tFile(tString name_) :
		name(name_.tToPlainArray()), file_handle(INVALID_HANDLE_VALUE), file_mapping_handle(
		INVALID_HANDLE_VALUE), mapped_buffer(NULL), curr_map_byte(NULL), sizeBytes(
				0) {
	NOT_NULL(name);

	file_handle = CreateFileA(name, GENERIC_READ | GENERIC_WRITE,
	FILE_SHARE_WRITE, NULL,
	OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	CHECK (file_handle == INVALID_HANDLE_VALUE, "Invalid file handle!");
}

unsigned tFile::tGetSize() {
	return sizeBytes;
}

//! Returns mapped buffer.
char* tFile::tGetBuffer() {
	NOT_NULL(mapped_buffer);
	return mapped_buffer;
}

//! Returns pointer in file to start position.
void tFile::tFlip() {
	curr_map_byte = mapped_buffer;
}

tString tFile::tReadAll() {
	tString res = { };
	while (tHasMoreSymbs()) {
		res += (tReadLine() + '\n');
	}
	return res;
}

//! Returns next char in file.
char tFile::tGetc() {
	NOT_NULL(mapped_buffer);
	CHECK (curr_map_byte >= mapped_buffer + sizeBytes,"Out of file range!");

	char result = *curr_map_byte;
	curr_map_byte++;

	//std::cout << "new symbol: " << (int)(result) << "\n";
	return result;
}

char* tFile::tGetCurrentPointer() {
	return curr_map_byte;
}

unsigned tFile::tGetCurrentByte() {
	return curr_map_byte - mapped_buffer;
}

void tFile::tSetCarriagePosition(unsigned pos) {
	NOT_NULL(mapped_buffer);
	curr_map_byte = mapped_buffer + pos;
}

void tFile::tMovePointer(int dx) {
	curr_map_byte += dx;
}

//! Starts file mapping. Use this before using tGetc(), tReadLine()...
//! sz - ensures that file at least have sz bytes.
//! If sz is zero then sizeBytes is initialized with tGetFileSize() function.
char* tFile::tStartMapping(unsigned sz) {
	tStopMapping();

	file_mapping_handle = CreateFileMappingA(file_handle, NULL,
	PAGE_READWRITE, 0, sz, NULL);
	CHECK (file_mapping_handle == INVALID_HANDLE_VALUE,"Invalid mapping handle!");

	mapped_buffer = (char*) MapViewOfFile(file_mapping_handle,
	FILE_MAP_ALL_ACCESS, 0, 0, sz);
	NOT_NULL(mapped_buffer);

	tFlip();

	if (sz != 0) {
		sizeBytes = sz;
	} else {
		sizeBytes = tGetFileSize(name);
	}

	return mapped_buffer;
}

//!Stops mapping
void tFile::tStopMapping() {
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
bool tFile::tHasMoreSymbs() {
	NOT_NULL(mapped_buffer);
	return curr_map_byte < mapped_buffer + sizeBytes;
}

void tFile::tFill(char pat) {
	for (unsigned i = 0; i < sizeBytes; i++) {
		mapped_buffer[i] = pat;
	}
}

void tFile::tPrintBytes() {
	NOT_NULL(mapped_buffer);
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
unsigned tFile::tGetPassedLines() {
	return lines_passed;
}

//! This function returns pointer to begin of this line which is located in mapped memory.
//! And it gives it length.
tString tFile::tReadLine() {
	CHECK (!tHasMoreSymbs(),"Out of file range!");

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


int tGetFileSize(const char *name) {
	NOT_NULL(name);
	WIN32_FIND_DATA data = { };
	FindFirstFileA(name, &data);
	return data.nFileSizeLow;
}

