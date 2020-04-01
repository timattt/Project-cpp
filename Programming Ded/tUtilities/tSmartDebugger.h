#ifndef T_EX_HANDL
#define T_EX_HANDL

#include "../tStorage/tList.h"
#include "tCallTreeBuilder.h"

using namespace tStorage;

namespace tUtilities {

class tException {

	tString message;
	tString stackTrace;

public:
	tException(tString mes, tString stacktr) :
			message(mes), stackTrace(stacktr) {
	}

	void tPrintMessage() {
		message.out();
	}

	void tPrintStackTrace() {
		stackTrace.out();
	}

};

//! Flushes unused functions that may be skipped when interrupting.
void __tDropBrokenFunctions(tString stableFuncName) {
	while (!(call_hierarchy.tGetLastElement().y == stableFuncName)) {
		__tGoOut();
	}
}

//! Collects stack trace in one nice looling string.
tString __collectStackTrace() {
	tString result = "Stack trace: ";
	for (unsigned i = 0; i < call_hierarchy.tGetSize(); i++) {
		result += (call_hierarchy[i].y);
		if (i + 1 < call_hierarchy.tGetSize()) {
			result += " -> ";
		}
	}
	return result;
}

void tThrowSmartExceptionWithStackTrace(tString message) {
	throw tException(message, __collectStackTrace());
}

#define START_SMART_DEBUG __tFastStart();
#define END_SMART_DEBUG __tFastEnd();
#define SMART_FUNC(RET_TYPE, NAME, PARAMS, CODE) RET_TYPE NAME PARAMS {\
						__tGoInto(#NAME);\
						CODE\
						__tGoOut(#NAME);\
}

}

#endif
