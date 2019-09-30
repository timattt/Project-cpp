#ifndef T_PROCESSOR
#define T_PROCESSOR

#include <bits/stdc++.h>
#include "../tUtilities/tFileHandler.h"
#include "../tUtilities/tUtilities.h"

using namespace std;
using namespace tFileHandler;
using namespace tUtilities;

namespace tLanguageHandler {

const unsigned stack_size = 10000;
const unsigned reg_size = 32; //ax, bx, cx, dx
const unsigned utilities_buf_size = 20;

const unsigned ax = 0;
const unsigned bx = 8;
const unsigned cx = 16;
const unsigned dx = 24;

class tProcessor {
	// Stack
	char stack[stack_size];
	unsigned stack_pointer = 0;

	// Registers
	char regs[reg_size];

public:
	// Use float
	bool useFloat;

	template<typename T> void push(const T &data) {
		tWriteBytes(data, stack + stack_pointer);
		stack_pointer += sizeof(T);
	}

	template<typename T> T& pop() {
		if (stack_pointer - sizeof(T) < 0) {
			tThrowException("No such element!");
		}
		stack_pointer -= sizeof(T);
		T &res = tConvertBytes<T>((const char*) (stack + stack_pointer));

		return res;
	}

	template<typename T> void setReg(const T &data, unsigned address) {
		if (address + sizeof(T) > reg_size) {
			tThrowException("Wrong address!");
		}
		tWriteBytes<T>(data, regs + address);
	}

	template<typename T> T& getReg(unsigned address) {
		if (address + sizeof(T) > reg_size) {
			tThrowException("Wrong address!");
		}
		T &res = tConvertBytes<T>(regs + address);
		return res;
	}

	int determineAddress(const char *args, unsigned len) {
		if (args == NULL) {
			tThrowException("Arguments are NULL!");
		}
		if (len < 2) {
			return -1;
		}
		int address = -1;
		if (tCompare<char>(args, "ax", 2)) {
			address = ax;
		}
		if (tCompare<char>(args, "bx", 2)) {
			address = bx;
		}
		if (tCompare<char>(args, "cx", 2)) {
			address = cx;
		}

		return address;
	}
};

typedef void (*tProcFunction)(const char*, unsigned, tProcessor*);

class tFuncLib {

private:
	// id -> name
	map<char, string> id_name;

	// name -> id
	map<string, char> name_id;

	// name -> func
	map<string, tProcFunction> name_func;

	// id -> func
	map<char, tProcFunction> id_func;

public:

	//! Adds function into function library.
	//! @param id - identifier for this function. Will be used in compiled version.
	//! @param name - name of this function. Must be used in source code.
	void tAddFunction(char id, const char *name, tProcFunction f) {
		id_name[id] = name;
		name_id[name] = id;
		name_func[name] = f;
		id_func[id] = f;
	}

	//!
	char name__id(const char *name) {
		if (name == NULL) {
			tThrowException("Name is NULL!");
		}
		if (name_id.count(name) == 0) {
			return -1;
		}
		return name_id[name];
	}

	tProcFunction id__func(char id) {
		return id_func[id];
	}

};

void tCompile(tFile *source, tFile *exe, tFuncLib *lib, bool copy_args_flag =
		false) {
	source->tStartMapping();

	unsigned total_lines = tSymbolsCount((const char*) source->tGetBuffer(),
			'\n');

	const unsigned base_length = 20;
	unsigned line_length = base_length;
	char *line = NULL;

	exe->tStartMapping(total_lines * line_length);
	exe->tClearBuffer();

	unsigned line_number = 1;

	for (; source->tHasMoreSymbs(); line_number++) {
		line_length = base_length;
		line = source->tReadLine(line_length);

		if (line == NULL) {
			tThrowException("Line is NULL!");
		}

		char id = -1;
		unsigned args_length = -1;
		char *args = NULL;

		char *p = line;
		for (unsigned i = 0; i < line_length; p++, i++) {
			if (*p == ' ') {
				*p = '\0';
				id = lib->name__id(line);
				*p = ' ';

				args_length = line_length - (p - line) - 1;
				args = p + 1;

				break;
			}
		}

		if (id == -1) {
			char tmp = *(line + line_length);
			*(line + line_length) = '\0';
			id = lib->name__id(line);
			*(line + line_length) = tmp;

			args_length = 0;
		}

		if (id == -1) {
			tThrowException("Compilation error!");
			return;
		}

		exe->tWritec(id);
		if (args_length != 0) {
			exe->tWriteLine(args, args_length);
		} else {
			exe->tWritec('\n');
		}
	}

	exe->tStopMapping();
	source->tStopMapping();

	free(line);
}

void tDecompile(tFile *exec, tFile *source, tFuncLib *lib) {

}

void tInvoke(tFile *exec, tFuncLib *lib) {
	tProcessor *processor = new tProcessor();

	const unsigned base_length = 20;
	unsigned line_length = base_length;
	char *line = NULL;

	exec->tStartMapping();
	while (exec->tHasMoreSymbs()) {
		line_length = base_length;
		line = exec->tReadLine(line_length);

		tProcFunction func = lib->id__func(line[0]);

		if (func != NULL) {
			func(line + 1, line_length - 1, processor);
		}

	}
	exec->tStopMapping();

	delete line;
	delete processor;
}

}

#endif
