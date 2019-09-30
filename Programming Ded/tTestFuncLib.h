#ifndef T_TEST_FUNC_LIB
#define T_TEST_FUNC_LIB

#include "tProcessor/tLanguageHandler.h"
#include "tUtilities/tUtilities.h"

using namespace tUtilities;
using namespace tLanguageHandler;

typedef double tType;

void push(const char *args, unsigned args_len, tProcessor *proc) {
	int address = proc->determineAddress(args, args_len);

	tType val =
			(address == -1) ?
					tConvertBytes<tType>(args) : proc->getReg<tType>(address);

	proc->push<tType>(val);
}

void pop(const char *args, unsigned args_len, tProcessor *proc) {
	tType val = proc->pop<tType>();

	int address = proc->determineAddress(args, args_len);

	if (address != -1) {
		proc->setReg<tType>(val, address);
	}
}

void in(const char *args, unsigned args_len, tProcessor *proc) {
	tType v;
	cin >> v;

	int address = proc->determineAddress(args, args_len);

	if (address != -1) {
		proc->setReg<tType>(v, address);
	} else {
		proc->push<tType>(v);
	}
}

void sub(const char *args, unsigned args_len, tProcessor *proc) {
	tType a = proc->pop<tType>();
	tType b = proc->pop<tType>();
	proc->push<tType>(-a + b);
}

void out(const char *args, unsigned args_len, tProcessor *proc) {
	int address = proc->determineAddress(args, args_len);
	if (address == -1) {
		cout << proc->pop<tType>() << "\n";
	} else {
		cout << proc->getReg<tType>(address) << "\n";
	}
}

void sum(const char *args, unsigned args_len, tProcessor *proc) {
	if (args_len != 0) {
		tType su = 0;
		for (unsigned i = 0;; i += 3) {
			if (i >= args_len) {
				break;
			}
			int adr = proc->determineAddress(args + i, 2);

			if (adr == -1) {
				sum(args, 0, proc);
				return;
			}

			tType v = proc->getReg<tType>(adr);
			su += v;
		}

		proc->push<tType>(su);
	} else {
		tType a = proc->pop<tType>();
		tType b = proc->pop<tType>();
		proc->push<tType>(a + b);
	}
}

#endif
