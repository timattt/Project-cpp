#ifndef T_TEST_FUNC_LIB
#define T_TEST_FUNC_LIB

#include "tProcessor/tLanguageHandler.h"
#include "tUtilities/tUtilities.h"

using namespace tUtilities;
using namespace tLanguageHandler;

void push(int *arg, tProcessor *proc) {
	proc->mem_stack.tPush(*arg);
}

void in(int * arg, tProcessor *proc) {
	int value = 0;
	cin >> value;
	if (arg != NULL) {
		*arg = value;
	} else {
		proc->mem_stack.tPush(value);
	}
}

void pop(int * args, tProcessor *proc) {
	int val = proc->mem_stack.tPop();
	if (args != NULL) {
		*args = val;
	}
}

void out(int * args, tProcessor *proc) {
	if (NULL != args) {
		cout << (*args) << "\n";
	} else {
		int val = proc->mem_stack.tPop();
		cout << val << "\n";
		proc->mem_stack.tPush(val);
	}
}

void sum(int * args, tProcessor *proc) {
	int v1 = proc->mem_stack.tPop();
	int v2 = proc->mem_stack.tPop();

	proc->mem_stack.tPush(v1 + v2);
}

void sub(int * args, tProcessor *proc) {
	int v1 = proc->mem_stack.tPop();
	int v2 = proc->mem_stack.tPop();

	proc->mem_stack.tPush(v1 - v2);
}

void mul(int * args, tProcessor *proc) {
	int v1 = proc->mem_stack.tPop();
	int v2 = proc->mem_stack.tPop();

	proc->mem_stack.tPush(v1 * v2);
}

#endif
