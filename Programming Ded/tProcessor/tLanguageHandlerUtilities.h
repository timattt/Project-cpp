#ifndef T_LANGUAGE_HANDLER_UTILITIES
#define T_LANGUAGE_HANDLER_UTILITIES

#include <bits/stdc++.h>
#include "../tStorage/tStack.h"

namespace tLanguageHandlerUtilities {

// STD FUNCTIONS
//!!!
using std::map;
using std::string;
using std::cout;
using std::cin;
//!!!

using namespace tStorage;

typedef float PROCESSOR_TYPE;

const unsigned Stack_size = 10000;
const unsigned Total_registers = 5; //ax, bx, cx
const unsigned Max_arguments = 10;

char tGetRegisterIndex(const char *args, unsigned len) {
	if (args == NULL || len < 2) {
		tThrowException("Arguments are NULL!");
	}

	char s1 = args[0];
	char s2 = args[1];

	if (s2 != 'x') {
		return -1;
	}

	char index = (s1 - 'a' + 1);

	if (index < 1 || (unsigned) index > Total_registers) {
		return -1;
	}

	return index;
}

struct tProcessor {
	// Stack
	tStack<PROCESSOR_TYPE, Stack_size> mem_stack;

	PROCESSOR_TYPE *regs;

	tProcessor() :
			mem_stack( { }), regs(NULL) {
		regs = (PROCESSOR_TYPE*) calloc(Total_registers, sizeof(int));
	}

	~tProcessor() {
		delete regs;
	}

};

typedef void (*tProcFunction)(PROCESSOR_TYPE**, unsigned, tProcessor*);

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

}

#endif
