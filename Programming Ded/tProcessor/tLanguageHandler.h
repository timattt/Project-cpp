#ifndef T_PROCESSOR_V1
#define T_PROCESSOR_V1

#include <bits/stdc++.h>
#include "../tUtilities/tFileHandler.h"
#include "../tUtilities/tUtilities.h"
#include "../tStorage/tStack.h"

using namespace std;
using namespace tFileHandler;
using namespace tUtilities;
using namespace tStorage;

namespace tLanguageHandler {

const unsigned Stack_size = 10000;
const unsigned Total_registers = 3; //ax, bx, cx

char tGetRegisterIndex(const char *args, unsigned len) {
	if (args == NULL) {
		tThrowException("Arguments are NULL!");
	}
	if (tCompare<char>(args, "ax", 2)) {
		return 1;
	}
	if (tCompare<char>(args, "bx", 2)) {
		return 2;
	}
	if (tCompare<char>(args, "cx", 2)) {
		return 3;
	}

	return 0;
}

struct tProcessor {
	// Stack
	tStack<int, Stack_size> mem_stack;

	int *regs;

	tProcessor() :
			mem_stack( { }), regs(NULL) {
		regs = (int*) calloc(Total_registers, sizeof(int));
	}

	~tProcessor() {
		delete regs;
	}

};

typedef void (*tProcFunction)(int*, tProcessor*);

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

void tCompile(tFile *source, tFile *exe, tFuncLib *lib) {
	if (source == NULL || exe == NULL || lib == NULL) {
		tThrowException("Something went wrong!");
	}

	unsigned line_length = 0;
	char *line = NULL;

	char *exe_buf = (char*) calloc(200, sizeof(char));
	unsigned buf_size = 0;

	source->tStartMapping();
	while (source->tHasMoreSymbs()) {
		line = source->tReadLine(line_length);

		if (line[0] == '/' && line[1] == '/') {
			continue;
		}

		while (*line == ' ' || *line == '	') {
			line++;
		}

		int id = -1;
		unsigned arg_beg = 0;
		for (unsigned i = 0; i < line_length + 1 && id == -1; i++, arg_beg++) {
			char tmp = *(line + i);
			*(line + i) = '\0';
			id = lib->name__id(line);
			*(line + i) = tmp;
		}

		if (id == -1) {
			tThrowException("Compilation error!");
		}
		exe_buf[buf_size] = id;
		buf_size++;

		int args_len = line_length - arg_beg;
		char address = (
				args_len >= 2 ? tGetRegisterIndex(line + arg_beg, 2) : -1);

		if (address != -1) {
			exe_buf[buf_size] = address;
			buf_size++;
		} else {
			exe_buf[buf_size] = ((char) (args_len <= 0 ? 127 : 0));
			buf_size++;

			if (args_len > 0) {
				char tmp = *(line + line_length);
				*(line + line_length) = '\0';
				int value = std::atoi(line + arg_beg);
				*(line + line_length) = tmp;
				tWriteBytes<int>(value, exe_buf);
				buf_size++;
			}
		}

	}
	source->tStopMapping();

	exe->tStartMapping(buf_size);
	tCopyBuffers((const char*) exe_buf, exe->tGetBuffer(), buf_size);
	exe->tStopMapping();

	delete exe_buf;
}

void tDecompile(tFile *exec, tFile *source, tFuncLib *lib) {

}

void tInvoke(tFile *exec, tFuncLib *lib) {
	tAssert(exec != NULL && lib != NULL);
	tProcessor *processor = new tProcessor();

	int arg_buf[1];

	exec->tStartMapping();
	while (exec->tHasMoreSymbs()) {
		char id = exec->tGetc();

		tProcFunction func = lib->id__func(id);

		if (func == NULL) {
			continue;
		}

		char arg_param = exec->tGetc();

		// Arguments are 4 bytes after
		if (arg_param == (char) 0) {
			*arg_buf = tConvertBytes<int>(exec->tGetCurrentPointer());
			exec->tMovePointer(4);
			func(arg_buf, processor);
		}

		// Argument is register
		if (arg_param > 0 && arg_param < (char) 127) {
			func(&processor->regs[(unsigned) arg_param], processor);
		}

		// No arguments
		if (arg_param == (char) 127) {
			func(NULL, processor);
		}

	}
	exec->tStopMapping();

	delete processor;
}

}

#endif
