#ifndef PE_EDITOR
#define PE_EDITOR

#include "../tUtilities/tUtilities.h"
#include "../tUtilities/tFileHandler.h"
#include "../tStorage/tList.h"
#include <winnt.h>

using namespace tFileHandler;
using namespace tStorage;

using std::map;
using std::cout;
using std::string;

#define MAX_IMPORT_FILES 4
#define MAX_IMPORT_FUNCTIONS 1
#define MAX_IMPORT_NAME_LEN 25

class tPE_Maker {
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
	map<string, int> import_funcs;

	// Writing
	char *code_carriage;
	char *data_carriage;

public:
	// first name in file array is file name.
	tPE_Maker(tFile *file_, int code_sz, int data_sz, int total_import_files,
			  char import[MAX_IMPORT_FILES][MAX_IMPORT_FUNCTIONS + 2][MAX_IMPORT_NAME_LEN]) {
		if (!file) {
			tThrowException("No file for PE creator!");
		}

		dest = file_;

		// not aligned sizes here
		code_size = code_sz;
		data_size = data_sz;
		import_size = calcImportSize();

		// aligning sizes
		code_size = alignFile(code_size);
		import_size = alignFile(import_size);
		data_size = alignFile(data_size);

		// creating other sizes
		initSizes();

		// finding locations for sections
		code_locat = alignFile(calcCodeSectionLocation());
		import_locat = alignFile(code_locat + code_size);
		data_locat = alignFile(import_locat + import_size);

		// calculating file size
		file_size = data_locat + data_size;

		// init file
		dest->tStartMapping(file_size);
		dest->tFill(0);
		this->file = dest->tGetBuffer();

		// init other
		initHeaders();
		initSectionsTable();

		// init one section
		initImportSection(total_import_files, import);

		// Writing init
		code_carriage = file + code_locat;
		data_carriage = file + data_locat;

		constructDefaultFunctions();
		info();
	}

	int getCodeSize() {
		return code_size;
	}

	int getDataSize() {
		return data_size;
	}

	char* getPointerInFile(int addr) {
		return (file + addr);
	}

	void constructDefaultFunctions() {
		code_carriage += (code_size - 0x150);

		// IO STRING
		io_string_va = getDataCarriageVA();
		for (int i = 0; i < max_io_string_length; i++) {
			wcb(0x00);
		}
		wdb(0x0D);

		// Default functions init
		int startCar = code_carriage - file;

		buildCharOut();
		buildOut();
		buildStringIn();
		buildIn();

		int end_car = code_carriage - file;

		cout << "Default function initialized! Total size for them: " << (end_car - startCar) << "\n";

		code_carriage = file + code_locat;
	}

	void buildStringIn() {
		in_string_function_va = getCodeCarriageVA();

		wcb(0x60);
		// pusha

		wcb(0x68);
		wcdw(-10);
		// push -10

		wcb(0x31);
		wcb(0xc0);
		// xor eax, eax

		wcif("GetStdHandle");

		wcb(0x68);
		wcdw(0);
		// push 0

		wcb(0x68);
		wcdw(io_string_va + 20);
		// push 0

		wcb(0x68);
		wcdw(max_io_string_length);
		// push max length

		wcb(0x68);
		wcdw(io_string_va);
		// push string pointer

		wcb(0x50);
		// push eax

		wcif("ReadConsoleA");

		wcb(0x61);
		// popa

		wcb(0xc3);
		// ret
	}

	void buildIn() {
		in_function_va = getCodeCarriageVA();

		callDefaultInStringFunction();

		// First code part
		const char code1[] = "\x53"						// push ebx
							 "\x51"						// push ecx
							 "\x52"						// push edx
							 "\x31\xC0"					// xor eax, eax
							 "\x31\xDB"					// xor ebx, ebx
							 "\xB9\x0A\x00\x00\x00"		// mov ecx, 0xA
							 "\xBA\x00\x00\x00\x00"		// mov edx, 0x0
							 "\x8A\x9A";				// mov bl, [io_buf]
		int sz = sizeof(code1) - 1;
		tCopyBuffers(code1, code_carriage, sz);
		code_carriage += sz;

		// Paste pointer
		wcdw(io_string_va);

		// Second code part
		const char code2[] = "\x80\xFB\x0D" // cmp    bl, 0xd
							 "\x74\x0F"     // je     0x14
							 "\x80\xFB\x0A" // cmp    bl, 0xa
							 "\x74\x0A"     // je     0x14
							 "\x83\xEB\x30" // sub    ebx, 0x30
							 "\xF6\xE1"     // mul    cl
							 "\x01\xD8"     // add    eax, ebx
							 "\x42"         // inc    edx
							 "\xEB\xE6"     // jmp    0xfffffffa
							 "\x5A"   		// pop    edx
							 "\x59"			// pop    ecx
							 "\x5B"			// pop    ebx
							 "\xC3";		// ret
		sz = sizeof(code2) - 1;
		tCopyBuffers(code2, code_carriage, sz);
		code_carriage += sz;

	}

	void buildOut() {
		out_function_va = getCodeCarriageVA();

		// First code part
		const char code1[] = "\x50"					// push   eax
							 "\x52"					// push   edx
							 "\x51"					// push   ecx
							 "\xB9\x0A\x00\x00\x00"	// mov    ecx, 0xa
							 "\x31\xD2"				// xor    edx, edx
							 "\xF7\xF1"				// div    ecx
							 "\x85\xC0"				// test   eax, eax
							 "\x74\x05"				// je     0x15
							 "\xE8\xEB\xFF\xFF\xFF"	// call   0x0
							 "\x89\xD0"				// mov    eax, edx
							 "\x83\xF8\x0A"			// cmp    eax, 0xa
							 "\x7C\x03"				// jl     0x1f
							 "\x83\xC0\x07"			// add    eax, 0x7
							 "\x83\xC0\x30";		// add    eax, 0x30
		int sz = sizeof(code1) - 1;
		tCopyBuffers(code1, code_carriage, sz);
		code_carriage += sz;

		// Call function
		callDefaultOutCharFunction();

		// Second part
		const char code2[] = "\xB8\x0A\x00\x00\x00" // mov    eax,0xa
							 "\x59"					// pop    ecx
							 "\x5A"					// pop    edx
							 "\x58"					// pop    eax
							 "\xC3";				// ret
		sz = sizeof(code2) - 1;

		tCopyBuffers(code2, code_carriage, sz);
		code_carriage += sz;
	}

	void buildCharOut() {
		out_char_function_va = getCodeCarriageVA();

		wcb(0x60);
		// pusha

		wcb(0xa2);
		wcdw(io_string_va);
		// mov [little_string_va], al

		wcb(0x31);
		wcb(0xc0);
		// xor eax, eax

		wcb(0x68);
		wcdw(-11);
		//push -11

		wcif("GetStdHandle");
		// call GetStdHandle

		wcb(0x31);
		wcb(0xd2);
		// xor edx, edx

		wcb(0x52);
		// push edx

		wcb(0x52);
		// push edx

		wcb(0x68);
		wcdw(1);
		// push 4

		wcb(0x68);
		wcdw(io_string_va);
		//push string pointer

		wcb(0x50);
		// push eax

		wcif("WriteConsoleA");
		// call WriteConsoleA

		wcb(0x61);
		// popa

		wcb(0xc3);
		// ret

	}

	int getCodeCarriageLocation() {
		return (code_carriage - file);
	}

	int getDataCarriageLocation() {
		return (data_carriage - file);
	}

	int getCodeVA() {
		return IMAGE_BASE + CODE_RVA;
	}

	int getDataVA() {
		return IMAGE_BASE + DATA_RVA;
	}

	int getImportVA() {
		return IMAGE_BASE + IMPORT_RVA;
	}

	int getCodeLocation() {
		return code_locat;
	}

	int getDataLocation() {
		return data_locat;
	}

	void flipCodeCarriage() {
		code_carriage = file + code_locat;
	}

	int getCodeCarriageVA() {
		return -code_locat + (int) (code_carriage - file) + getCodeVA();
	}

	int getDataCarriageVA() {
		return -data_locat + (int) (data_carriage - file) + getDataVA();
	}

	// writes import function into code carriage. Moves code carriage
	int wcif(const char *name) {
		int va = getCodeCarriageVA();
		callImportedFunction(name, code_carriage);
		code_carriage += 6;
		return va;
	}

	// write to code dword. Moves code carriage
	int wcdw(int va) {
		int result = getCodeCarriageVA();
		tWriteBytes(va, code_carriage);
		code_carriage += 4;
		return result;
	}

	// write to data dword. Moves code carriage
	int wddw(int va) {
		int result = getDataCarriageVA();
		tWriteBytes(va, data_carriage);
		data_carriage += 4;
		return result;
	}

	// write to code byte. Moves code carriage
	int wcb(char op) {
		int result = getCodeCarriageVA();
		code_carriage[0] = op;
		code_carriage++;
		return result;
	}

	// write to data byte. Moves data carriage
	int wdb(char dt) {
		int result = getDataCarriageVA();
		data_carriage[0] = dt;
		data_carriage++;

		return result;
	}

	void pushEAX() {
		wcb(0x50);
	}

	void popEAX() {
		wcb(0x58);
	}

	void pushRegs() {
		wcb(0x60);
	}

	void popRegs() {
		wcb(0x61);
	}

	// places default out function in code section on the carriage position.
	void callDefaultOutFunction() {
		wcb(0xe8);
		wcdw(out_function_va - getCodeCarriageVA() - 4);
	}

	void callDefaultInFunction() {
		wcb(0xe8);
		wcdw(in_function_va - getCodeCarriageVA() - 4);
	}

	void callDefaultOutCharFunction() {
		wcb(0xe8);
		wcdw(out_char_function_va - getCodeCarriageVA() - 4);
	}

	void callDefaultInStringFunction() {
		wcb(0xe8);
		wcdw(in_string_function_va - getCodeCarriageVA() - 4);
	}

	void callImportedFunction(const char *name, char *place) {
		char *ptr = place;
		ptr[0] = 0xff;
		ptr[1] = 0x15;
		tWriteBytes<DWORD>(import_funcs[name] + IMAGE_BASE, ptr + 2);
	}

	void info() {
		cout << "Creating PE file...\n";
		cout << "code size: " << code_size << ", imports size: " << import_size
			 << ", data size: " << data_size << "\n";
		cout << "code location: " << code_locat << ", imports location: "
			 << import_locat << ", data location: " << data_locat << "\n";
		cout << "rva code size: " << CODE_SIZE_RVA << ", rva imports size: "
			 << IMPORT_SIZE_RVA << ", rva data size: " << DATA_SIZE_RVA
			 << "\n";
		cout << "rva code location: " << CODE_RVA << ", rva imports location: "
			 << IMPORT_RVA << ", rva data location: " << DATA_RVA << "\n";
	}

	~tPE_Maker() {
		dest->tStopMapping();
	}

private:

	int alignFile(int val) {
		return tup(val, fileAl) * fileAl;
	}

	int alignRVA(int val) {
		return tup(val, sectAl) * sectAl;
	}

	void initSizes() {
		CODE_SIZE_RVA   = alignRVA(code_size);
		IMPORT_SIZE_RVA = alignRVA(import_size);
		DATA_SIZE_RVA   = alignRVA(data_size);

		CODE_RVA   = sectAl;
		IMPORT_RVA = CODE_RVA   + CODE_SIZE_RVA;
		DATA_RVA   = IMPORT_RVA + IMPORT_SIZE_RVA;
	}

	void initImportSection(int total_import_files,
			const char import[MAX_IMPORT_FILES][MAX_IMPORT_FUNCTIONS + 2][MAX_IMPORT_NAME_LEN]) {

		_IMAGE_IMPORT_DESCRIPTOR *imprt_file = (_IMAGE_IMPORT_DESCRIPTOR*) (file + import_locat);
		char *after = (file + import_locat + sizeof(_IMAGE_IMPORT_DESCRIPTOR) * (total_import_files + 1));
		int after_rva = IMPORT_RVA + sizeof(_IMAGE_IMPORT_DESCRIPTOR) * (total_import_files + 1);

		for (int i = 0; i < total_import_files; i++) {
			const char *file_name = import[i][0];
			int file_name_len = strlen(file_name);

			// File name
			imprt_file->Name = after_rva;
			tCopyBuffers(file_name, after, file_name_len);
			after += file_name_len;
			after[0] = 0;
			after++;
			after_rva += (file_name_len + 1);

			// List of names
			int total_func = 0;
			tList<int> funcs;
			for (; import[i][total_func + 1][0] != 0; total_func++) {
				const char *func_name = import[i][total_func + 1];
				int func_name_len = strlen(func_name);
				funcs.tAddLast(after_rva);

				tWriteBytes<WORD>(total_func, after);
				after += 2;
				after_rva += 2;

				tCopyBuffers(func_name, after, func_name_len);
				after += func_name_len;
				after[0] = 0;
				after++;
				after_rva += (func_name_len + 1);
			}

			// List 1
			imprt_file->OriginalFirstThunk = after_rva;
			for (int j = 0; j < total_func; j++) {
				tWriteBytes<DWORD>(funcs.tGet(j), after);
				after += 4;
				tWriteBytes<DWORD>(0, after);
				after += 4;
				after_rva += 8;
			}
			after_rva++;
			after[0] = 0;
			after++;

			// List 2
			imprt_file->FirstThunk = after_rva;
			for (int j = 0; j < total_func; j++) {
				cout << "Function: " << import[i][j + 1] << " located at "
					 << after_rva << "\n";
				import_funcs[import[i][j + 1]] = after_rva;
				tWriteBytes<DWORD>(funcs.tGet(j), after);
				after += 4;
				tWriteBytes<DWORD>(0, after);
				after += 4;
				after_rva += 8;
			}
			after_rva++;
			after[0] = 0;
			after++;
			imprt_file++;
		}
	}

	void initHeaders() {
		dos_h    = (_IMAGE_DOS_HEADER*) file;
		pe_h     = (_IMAGE_NT_HEADERS*) (file + sizeof(_IMAGE_DOS_HEADER));
		code_h   = (_IMAGE_SECTION_HEADER*) (file + sizeof(_IMAGE_DOS_HEADER) + sizeof(_IMAGE_NT_HEADERS));
		import_h = code_h + 1;
		data_h   = import_h + 1;

		// Dos header
		dos_h->e_magic  = *(WORD*) ("MZ");
		dos_h->e_lfanew = 0x40;

		// PE header
		pe_h->Signature                            = *(WORD*) ("PE\0\0");
		pe_h->FileHeader.Machine                   = 0x14C;
		pe_h->FileHeader.NumberOfSections          = 3;
		pe_h->FileHeader.SizeOfOptionalHeader      = 0xE0;
		pe_h->FileHeader.Characteristics           = 0x102;

		// Optional header
		pe_h->OptionalHeader.Magic                 = 0x10B;
		pe_h->OptionalHeader.AddressOfEntryPoint   = CODE_RVA;
		pe_h->OptionalHeader.ImageBase             = IMAGE_BASE;
		pe_h->OptionalHeader.SectionAlignment      = sectAl;
		pe_h->OptionalHeader.FileAlignment         = fileAl;
		pe_h->OptionalHeader.MajorSubsystemVersion = 4;
		pe_h->OptionalHeader.SizeOfImage           = sectAl + CODE_SIZE_RVA + DATA_SIZE_RVA + IMPORT_SIZE_RVA;
		pe_h->OptionalHeader.SizeOfHeaders         = fileAl;
		pe_h->OptionalHeader.Subsystem             = 3;
		pe_h->OptionalHeader.NumberOfRvaAndSizes   = 16;

		// Data directories
		pe_h->OptionalHeader.DataDirectory[1].VirtualAddress = IMPORT_RVA;
	}

	void initSectionsTable() {
		initSection(code_h,
					".text",
					CODE_SIZE_RVA,
					CODE_RVA,
					code_size,
					code_locat,
					IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE);
		initSection(import_h,
					".rdata",
					IMPORT_SIZE_RVA,
					IMPORT_RVA,
					import_size,
					import_locat,
					*(DWORD*)("\x40\x00\x00\x40"));
		initSection(data_h,
					".data",
					DATA_SIZE_RVA,
					DATA_RVA,
					data_size,
					data_locat,
					*(DWORD*)("\x40\x00\x00\xC0"));
	}

	void initSection(_IMAGE_SECTION_HEADER *sec, const char *name,
					 int virt_size, int virt_addr, int size_of_raw_data,
					 int pointer_to_raw, int charact) {
		if (!sec) {
			tThrowException("Section pointer is null!");
		}
		for (int i = 0, sz = strlen(name); i < sz; i++) {
			sec->Name[i] = name[i];
		}
		sec->Misc.VirtualSize = virt_size;
		sec->VirtualAddress   = virt_addr;
		sec->SizeOfRawData    = size_of_raw_data;
		sec->PointerToRawData = pointer_to_raw;
		sec->Characteristics  = charact;
	}

	int calcImportSize() {
		return + MAX_IMPORT_FILES * (20 + MAX_IMPORT_NAME_LEN + 1
			   + 2 * (MAX_IMPORT_FUNCTIONS * (MAX_IMPORT_NAME_LEN + 2) + 1));
	}

	int calcCodeSectionLocation() {
		return sizeof(_IMAGE_DOS_HEADER) + sizeof(_IMAGE_NT_HEADERS) + 3 * sizeof(_IMAGE_SECTION_HEADER);
	}

	int calcPEFileSize(int code_size, int data_size) {
		return alignFile(sizeof(_IMAGE_DOS_HEADER) + sizeof(_IMAGE_NT_HEADERS)
						                           + 3 * sizeof(_IMAGE_SECTION_HEADER) + code_size
						                           + data_size + calcImportSize());
	}

};

#endif

/*
 * I serve the Soviet Union  ☭
 */
