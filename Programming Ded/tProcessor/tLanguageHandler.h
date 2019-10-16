#ifndef T_LANGUAGE_HANDLER_V2
#define T_LANGUAGE_HANDLER_V2

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

const unsigned STACK_SIZE = 10000;
const unsigned RAM_SIZE = 10000;
const unsigned REGS_SIZE = 5; //ax, bx, cx, dx, bp
const unsigned MAX_ARGS = 10;

char tGetRegisterIndex(const char *args, unsigned len) {
	if (args == NULL || len < 2) {
		tThrowException("Arguments are NULL!");
	}

	char s1 = args[0];
	char s2 = args[1];

	// If bp register
	if (s1 == 'b' && s2 == 'p') {
		return 0;
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

	unsigned code_size;

	// Stack
	tStack<PROCESSOR_TYPE, STACK_SIZE> mem_stack;

	// Registers
	PROCESSOR_TYPE *regs;

private:
	// Invokation
	bool invoking = false;

public:
	tProcessor(tFile *exeFile_) :
			ram(NULL), carriage(NULL), code_size(0), mem_stack( { }), regs(NULL) {
		regs = (PROCESSOR_TYPE*) calloc(REGS_SIZE, sizeof(PROCESSOR_TYPE));
		ram = (char*) calloc(RAM_SIZE, sizeof(char));

		exeFile_->tStartMapping();
		tCopyBuffers(exeFile_->tGetBuffer(), ram, code_size =
				exeFile_->tGetSize());
		exeFile_->tStopMapping();

		carriage = ram;
	}

	void tMoveExeCarriage(unsigned byte) {
		if (!invoking) {
			tThrowException("Program is not invoked!");
		}

		carriage = ram + byte;
	}

	char tGetc() {
		char res = *carriage;
		carriage++;
		return res;
	}

	unsigned tGetCurrentByte() {
		return carriage - ram;
	}

	~tProcessor() {
		delete regs;
		delete ram;
	}

	void tInvoke() {
		invoking = true;

		PROCESSOR_TYPE *arg_buf[MAX_ARGS];
		PROCESSOR_TYPE constant_pointers[MAX_ARGS];

		while (carriage < ram + code_size) {
			char id = tGetc();

			tProcFunction func = NULL;

#define T_PROC_FUNC(NAME, ID, CODE) if (id == ID) {func = [](PROCESSOR_TYPE**args, unsigned totalArgs, tProcessor*proc) CODE;}
#include "tStandartDefs.h"
#include "cmd.tlang"
#include "tStandartUndefs.h"
#undef T_PROC_FUNC

			if (func == NULL) {
				continue;
			}

			unsigned total_args = (unsigned) (tGetc());

			for (unsigned i = 0; i < total_args; i++) {
				char isRam = tGetc();
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

				if (isRam == 0) {
					*(arg_buf + i) = arg_p;
				} else {
					*(arg_buf + i) = (PROCESSOR_TYPE*) (ram + code_size
							+ (unsigned) (*arg_p) * sizeof(PROCESSOR_TYPE));
				}
			}

			func(arg_buf, total_args, this);
		}

		invoking = false;

	}

};

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
