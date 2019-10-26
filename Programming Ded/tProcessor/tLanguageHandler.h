#ifndef T_LANGUAGE_HANDLER_V2
#define T_LANGUAGE_HANDLER_V2

#include "../TXLIB.h"
#include "../tUtilities/tFileHandler.h"
#include "../tStorage/tStack.h"
#include "bits/stdc++.h"

using namespace tFileHandler;
using namespace tStorage;

using std::cout;
using std::cin;
using std::map;
using std::string;
using std::list;
using std::pair;

namespace tLanguageHandler {

// 4 byte variable type
typedef int PROCESSOR_TYPE;

// Symbol that will be ignored
const unsigned SRC_IGNORE_SYMBS_QUANT = 1;
char SRC_IGNORE_SYMBS[SRC_IGNORE_SYMBS_QUANT] = { ' ' };

struct tProcessor;

// Prototype of function
typedef void (*tProcFunction)(PROCESSOR_TYPE**, unsigned, tProcessor*);

// SIZES
const unsigned STACK_SIZE = 10000;
const unsigned RAM_SIZE = 10000;
const unsigned REGS_SIZE = 5; //ax, bx, cx, dx, bp
const unsigned MAX_ARGS = 10;

// VRAM
const unsigned SCREEN_WIDTH = 300;
const unsigned SCREEN_HEIGHT = 300;
const unsigned BYTES_PER_PIXEL = 3;
const unsigned VRAM_SIZE = SCREEN_WIDTH * SCREEN_HEIGHT * BYTES_PER_PIXEL;
const unsigned PIXEL_SIDE = 5;

// Gives address for given symbolic value of the register.
char tGetRegisterIndex(const char *args, unsigned len) {
	if (args == NULL || len < 2) {
		tThrowException("Arguments are NULL!");
	}

	char s1 = args[0];
	char s2 = args[1];

	// If bp is register
	if (s1 == 'b' && s2 == 'p') {
		return 1;
	}

	if (s2 != 'x') {
		return -1;
	}

	char index = (s1 - 'a' + 1);

	if (index < 1 || (unsigned) index > REGS_SIZE) {
		return -1;
	}

	return index + 1;
}

struct tProcessor {

	// RAM
	char *ram;
	char *carriage;
	char *vram;
	unsigned code_size;

	// Stack
	tStack<PROCESSOR_TYPE, STACK_SIZE> mem_stack;

	// Registers
	PROCESSOR_TYPE *regs;

private:

	// Invokation
	bool invoking = false;

	// Window
	bool windowCreated;

public:

	tProcessor(tFile *exeFile_) :
			ram(NULL), carriage(NULL), vram(NULL), code_size(0), windowCreated(
					0), mem_stack( { }), regs(NULL) {
		regs = (PROCESSOR_TYPE*) calloc(REGS_SIZE, sizeof(PROCESSOR_TYPE));
		ram = (char*) calloc(RAM_SIZE + VRAM_SIZE, sizeof(char));
		vram = ram + RAM_SIZE;

		exeFile_->tStartMapping();
		tCopyBuffers(exeFile_->tGetBuffer(), ram, code_size =
				exeFile_->tGetSize());
		exeFile_->tStopMapping();

		carriage = ram;
	}

	// Puts value to given place in the memory.
	void putToRam(unsigned place, PROCESSOR_TYPE val) {
		tWriteBytes<PROCESSOR_TYPE>(val,
				ram + code_size + sizeof(PROCESSOR_TYPE) * place);
	}

	// Gives element located in memory.
	PROCESSOR_TYPE getFromRam(unsigned place) {
		PROCESSOR_TYPE v = tConvertBytes<PROCESSOR_TYPE>(
				ram + code_size + sizeof(PROCESSOR_TYPE) * place);
		return v;
	}

	// Sets execution carriage to given byte.
	void tSetCarriage(unsigned byte) {
		if (!invoking) {
			tThrowException("Program is not invoked!");
		}

		carriage = ram + byte;
	}

	// Reads symbol from execution carriage. And moves it further.
	char tGetc() {
		if (invoking) {
			char res = *carriage;
			carriage++;
			return res;
		} else {
			tThrowException("Program is not invoked!");
			return -1;
		}
	}

	// Gives current byte where carriage stands in the RAM.
	unsigned tGetCurrentByte() {
		return carriage - ram;
	}

	~tProcessor() {
		delete regs;
		delete ram;
	}

	// Sets carriage to the end of the code.
	// So if it is invoking right now then invokation will stops.
	void tStop() {
		if (invoking) {
			carriage = ram + code_size;
		} else {
			tThrowException("Program is not invoked!");
		}
	}

	// Returns pixel with given index.
	// There will be 3 bytes for each color.
	char* getPixel(unsigned ind) {
		return vram + ind * BYTES_PER_PIXEL;
	}

	// x = floor(ind / WIDTH)
	// y = (ind % WIDTH)
	void convertPixelCoords(unsigned ind, unsigned &x, unsigned &y) {
		x = (ind / SCREEN_WIDTH);
		y = (ind % SCREEN_WIDTH);
	}

	// Returns pixel with given coords.
	// There will be 3 bytes for each color.
	char* getPixel(unsigned col, unsigned lin) {
		return vram + (col + lin * SCREEN_WIDTH) * BYTES_PER_PIXEL;
	}

	// y - строка - line
	// x - столбец - column
	// y = floor(i / WIDTH);
	// x = i % WIDTH;
	void repaint() {
		if (!windowCreated) {
			return;
		}
		for (unsigned i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
			char *pix = getPixel(i);
			unsigned x = 0;
			unsigned y = 0;
			convertPixelCoords(i, x, y);

			if (pix[0] != 0 || pix[1] != 0 || pix[2] != 0) {
				txSetColor(RGB(pix[0], pix[1], pix[2]));
				txSetFillColor(RGB(pix[0], pix[1], pix[2]));
				txRectangle(x, y, x + PIXEL_SIDE, y + PIXEL_SIDE);
			}
		}

		txRedrawWindow();
	}

	void initTX() {
		windowCreated = 1;
		txCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, 1);
	}

	void cleanupTX() {
		if (windowCreated) {
			windowCreated = 0;
			txDestroyWindow();
		}
	}

	// Invokes program.
	void tInvoke() {
		// Update state
		invoking = true;

		// Pointers for arguments
		PROCESSOR_TYPE *arg_buf[MAX_ARGS];
		PROCESSOR_TYPE constant_pointers[MAX_ARGS];

		// Setting BP register to be right after code in RAM and ensure there will be space for 2 parameters.
		regs[(unsigned) tGetRegisterIndex("bp", 2)] = 0;

		// Starting main loop of the invocation
		// Invocation will be stopped ONLY WHEN CARRIAGE IS STANDING RIGHT IN THE END OF THE CODE.
		// So invocation can continue even if carriage is not executing code.
		while (carriage != ram + code_size) {
			// Reading id of function.
			char id = tGetc();
			// Generation of normal c++ function.
			tProcFunction func = NULL;

#define T_PROC_FUNC(NAME, ID, CODE) if (id == ID) {func = [](PROCESSOR_TYPE**args, unsigned totalArgs, tProcessor*proc) CODE;}
#include "tStandartDefs.h"
#include "cmd.tlang"
#include "tStandartUndefs.h"
#undef T_PROC_FUNC

			if (func == NULL) {
				continue;
			}

			// Reading byte where quantity of arguments is stored.
			unsigned total_args = (unsigned) (tGetc());

			// Parsing every argument and preparing argument array.
			for (unsigned i = 0; i < total_args; i++) {
				// Reading flag that says if this parameter lives in RAM.
				char isRam = tGetc();
				// Reading argument parameter.
				// If it is zero then next 4 bytes will be used.
				// If it is not zero then we will use
				char arg_param = tGetc();

				PROCESSOR_TYPE *arg_p = NULL;

				// Arguments are 4 bytes after
				if (arg_param == (char) 0) {
					constant_pointers[i] = tConvertBytes<PROCESSOR_TYPE>(
							carriage);
					carriage += 4;
					arg_p = &constant_pointers[i];
				}
				// Argument is register
				else {
					arg_p = &(regs[(unsigned) arg_param]);
				}

				// If ram flag is not zero then value will be used as address for RAM
				if (isRam == 0) {
					*(arg_buf + i) = arg_p;
				} else {
					*(arg_buf + i) = (PROCESSOR_TYPE*) (ram
							+ (unsigned) (*arg_p));
				}
			}

			// Launch function
			func(arg_buf, total_args, this);
		}

		invoking = false;
	}

};

// Generates map where all targets in this source file are stored.
map<string, unsigned>* tGenJmpsLib(tFile *src) {
	if (src == NULL) {
		tThrowException("Source code file is NULL!");
	}
	map<string, unsigned> *result = new map<string, unsigned>;

	unsigned line_length = 0;
	char *line = NULL;

	while (src->tHasMoreSymbs()) {
		line = src->tReadLine(line_length);

		for (unsigned len = 0; len < line_length; len++) {
			if (line[len] == ':') {
				unsigned total_divs = 0;
				char **divs = tParse(line, line_length, SRC_IGNORE_SYMBS,
						SRC_IGNORE_SYMBS_QUANT, total_divs);

				if (divs == NULL || total_divs < 1
						|| tStrlen(divs[total_divs - 1]) == 0) {
					tThrowException(
							"Compilation error! Something is wrong with jmps!");
				}

				divs[total_divs - 1][tStrlen(divs[total_divs - 1]) - 1] = '\0';
				(*result)[divs[total_divs - 1]] = src->tGetCurrentByte();

				break;
			}
		}

	}

	return result;
}

void tCompile(tFile *source, tFile *exe) {
	if (source == NULL || exe == NULL) {
		tThrowException("Something went wrong!");
	}

	// Line
	unsigned line_length = 0;
	char *line = NULL;

	// File init
	source->tStartMapping();
	exe->tStartMapping(2 * tGetFileSize(source->tGetName()));

	// Jumps library
	list<pair<string, unsigned>> jmp_later;
	map<string, unsigned> *jmp_points = tGenJmpsLib(source);

	// Ensuring end. target is in this file.
	(*jmp_points)["end."] = 1;

	source->tFlip();

	while (source->tHasMoreSymbs()) {
		line = source->tReadLine(line_length);

		unsigned total_divs = 0;

		char **divs = tParse(line, line_length, SRC_IGNORE_SYMBS,
				SRC_IGNORE_SYMBS_QUANT, total_divs);

		// Checking if it is jump
		for (unsigned i = 0; i < total_divs; i++) {
			// double dot index :
			unsigned dd_index = tStrlen(divs[i]) - 1;
			if (divs[i][dd_index] == ':') {
				divs[i][dd_index] = '\0';

				(*jmp_points)[string(divs[i])] = exe->tGetCurrentByte();

				//delete divs;
				total_divs = 0;
				break;
			}
		}

		if (total_divs < 1) {
			continue;
		}

		if (tStrlen(divs[0]) > 1 && divs[0][0] == '/' && divs[0][1] == '/') {
			delete divs;
			continue;
		}
		int id = -1;

#define T_PROC_FUNC(NAME, ID, CODE) if (!tStrcmp(divs[0], #NAME, tStrlen(#NAME))) {id = ID;}
#include "tStandartDefs.h"
#include "cmd.tlang"
#include "tStandartUndefs.h"
#undef T_PROC_FUNC

		if (id == -1 || total_divs - 1 > MAX_ARGS) {
			delete divs;
			tThrowException("Compilation error!");
		}

		// Writing id of this command
		exe->tWritec(id);

		// Writing quantity of parameters
		exe->tWritec((char) (total_divs - 1));

		for (unsigned i = 1; i < total_divs; i++) {
			char isRam = 0;
			if (divs[i][0] == '[' && divs[i][tStrlen(divs[i]) - 1] == ']') {
				isRam = (char) (127);

				divs[i][tStrlen(divs[i]) - 1] = '\0';
				divs[i]++;
			}

			char address = (
					(tStrlen(divs[i]) > 1) ? tGetRegisterIndex(divs[i], 2) : -1);

			exe->tWritec(isRam);

			if (address != -1) {
				// Writing type of argument. In this case it will be register.
				exe->tWritec(address);
			} else {
				// Writing type of argument. In this case there will be 4 bytes after.
				exe->tWritec((char) (0));

				if ((*jmp_points).count((string) divs[i]) > 0) {
					pair<string, unsigned> p = { (string) divs[i],
							exe->tGetCurrentByte() };
					jmp_later.push_back(p);
					exe->tMovePointer(4);
					continue;
				}
				PROCESSOR_TYPE value = 0;
				if (typeid(PROCESSOR_TYPE) == typeid(int)) {
					value = std::atoi(divs[i]);
				}
				if (typeid(PROCESSOR_TYPE) == typeid(float)) {
					value = std::atof(divs[i]);
				}
				tWriteBytes<PROCESSOR_TYPE>(value, exe->tGetCurrentPointer());
				exe->tMovePointer(4);

			}

		}
		//delete divs;
	}

	(*jmp_points)["end."] = exe->tGetCurrentByte();
	exe->tWritec((char) 0);

	while (!jmp_later.empty()) {
		pair<string, unsigned> p = jmp_later.front();
		jmp_later.pop_front();

		int address = (*jmp_points)[p.first];
		unsigned byte = p.second;

		tWriteBytes<PROCESSOR_TYPE>(address, exe->tGetBuffer() + byte);
	}

	source->tStopMapping();
	exe->tStopMapping();

}

}

#endif
