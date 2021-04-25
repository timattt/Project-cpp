#pragma once

// Includes
//===================================================
#include <windef.h>
#include <winnt.h>
#include <cstring>
#include <iostream>
#include <map>
#include <string>

#include "../Storage/tList.hpp"
#include "../Utilities/FileHandler.hpp"
#include "../Utilities/Utilities.hpp"
//===================================================

using std::cout;
using std::map;

// Defines
//===================================================
#define MAX_IMPORT_FILES 4
#define MAX_IMPORT_FUNCTIONS 1
#define MAX_IMPORT_NAME_LEN 25
//===================================================


// Structures
//===================================================
class PE_Maker {
	// File
	tFile *dest;
	char *file;
	int file_size;

	// Sections
	int code_size;
	int data_size;
	int import_size;

	int code_locat;
	int import_locat;
	int data_locat;

	// Headers
	_IMAGE_DOS_HEADER *dos_h;
	_IMAGE_NT_HEADERS *pe_h;
	_IMAGE_SECTION_HEADER *code_h;
	_IMAGE_SECTION_HEADER *import_h;
	_IMAGE_SECTION_HEADER *data_h;

	// Alignment
	const int fileAl = 0x200;
	const int sectAl = 0x1000;

	const int IMAGE_BASE = 0x400000;

	// Default functions
	int out_char_function_va;
	int out_function_va;
	int in_string_function_va;
	int in_function_va;

	const int max_io_string_length = 5;
	int io_string_va = 0;

	// RVA
	int CODE_RVA = 0;
	int IMPORT_RVA = 0;
	int DATA_RVA = 0;

	int CODE_SIZE_RVA = 0;
	int DATA_SIZE_RVA = 0;
	int IMPORT_SIZE_RVA = 0;

	// Import
	map<tString, int> import_funcs;

	// Writing
	char *code_carriage;
	char *data_carriage;

public:

	// first name in file array is file name.
	PE_Maker(tFile *file_, int code_sz, int data_sz, int total_import_files,
			 const char import[MAX_IMPORT_FILES][MAX_IMPORT_FUNCTIONS + 2][MAX_IMPORT_NAME_LEN]);
	~PE_Maker();
	int getCodeSize();
	int getDataSize();
	char* getPointerInFile(int addr);
	void constructDefaultFunctions();
	void buildStringIn();
	void buildIn();
	void buildOut();
	void buildCharOut();
	int getCodeCarriageLocation();
	int getDataCarriageLocation();
	int getCodeVA();
	int getDataVA();
	int getImportVA();
	int getCodeLocation();
	int getDataLocation();
	void flipCodeCarriage();
	int getCodeCarriageVA();
	int getDataCarriageVA();
	int wcif(const char *name);//! writes import function into code carriage. Moves code carriage
	int wcdw(int va);//! write to code dword. Moves code carriage
	int wddw(int va);//! write to data dword. Moves code carriage
	int wcb(char op);//! write to code byte. Moves code carriage
	int wdb(char dt);//! write to data byte. Moves data carriage
	void pushEAX();
	void popEAX();
	void pushRegs();
	void popRegs();
	void callDefaultOutFunction();//! places default out function in code section on the carriage position.
	void callDefaultInFunction();
	void callDefaultOutCharFunction();
	void callDefaultInStringFunction();
	void callImportedFunction(const char *name, char *place);
	void info();

private:

	int alignFile(int val);
	int alignRVA(int val);
	void initSizes();
	void initImportSection(int total_import_files,
			const char import[MAX_IMPORT_FILES][MAX_IMPORT_FUNCTIONS + 2][MAX_IMPORT_NAME_LEN]);
	void initHeaders();
	void initSectionsTable();
	void initSection(_IMAGE_SECTION_HEADER *sec, const char *name,
					 int virt_size, int virt_addr, int size_of_raw_data,
					 int pointer_to_raw, int charact);
	int calcImportSize();
	int calcCodeSectionLocation();
	int calcPEFileSize(int code_size, int data_size);

};
//===================================================
