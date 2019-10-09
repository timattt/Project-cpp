#ifndef T_LANGUAGE_HANDLER_V2
#define T_LANGUAGE_HANDLER_V2

#include "tLanguageHandlerUtilities.h"
#include "../tUtilities/tUtilities.h"
#include "../tUtilities/tFileHandler.h"

#include "bits/stdc++.h"

using namespace std;
using namespace tUtilities;
using namespace tFileHandler;
using namespace tLanguageHandlerUtilities;

namespace tLanguageHandlerV2 {

void tCompile(tFile *source, tFile *exe) {
	if (source == NULL || exe == NULL) {
		tThrowException("Something went wrong!");
	}

	unsigned line_length = 0;
	char *line = NULL;

	char parse_symbols[2] = { ' ', '	' };

	source->tStartMapping();
	exe->tStartMapping(200);
	while (source->tHasMoreSymbs()) {
		line = source->tReadLine(line_length);

		unsigned total_divs = 0;

		char **divs = tParse(line, line_length, parse_symbols, 2, total_divs);

		if (total_divs < 1) {
			continue;
		}

		if (tStrlen(divs[0]) > 1 && divs[0][0] == '/' && divs[0][1] == '/') {
			delete divs;
			continue;
		}

		int id = -1;

#define DEF(NAME, ID, CODE) if (!tStrcmp(divs[0], #NAME, tStrlen(divs[0]))) {id = ID;}
#include "cmd.tlang"
#undef DEF

		if (id == -1) {
			delete divs;
			tThrowException("Compilation error!");
		}

		exe->tWritec(id);

		char address = (
				(total_divs >= 2 && tStrlen(divs[1]) > 1) ?
						tGetRegisterIndex(divs[1], 2) : -1);

		if (address != -1) {
			exe->tWritec(address);
		} else {
			exe->tWritec((char) (total_divs >= 2 ? 0 : 127));

			if (total_divs >= 2) {
				PROCESSOR_TYPE value = 0;
				if (typeid(PROCESSOR_TYPE) == typeid(int)) {
					value = std::atoi(divs[1]);
				}
				if (typeid(PROCESSOR_TYPE) == typeid(int)) {
					value = std::atof(divs[1]);
				}
				tWriteBytes<PROCESSOR_TYPE>(value, exe->tGetBuffer());
				exe->tMovePointer(4);
			}
		}

		delete divs;
	}
	source->tStopMapping();
	exe->tStopMapping();

}

void tInvoke(tFile *exec) {

	tAssert(exec != NULL);

	tProcessor *processor = new tProcessor();

	PROCESSOR_TYPE arg_buf[1];

	exec->tStartMapping();
	while (exec->tHasMoreSymbs()) {
		char id = exec->tGetc();

		tProcFunction func = NULL;

#define DEF(NAME, ID, CODE) if (id == ID) func = [](PROCESSOR_TYPE*arg, tProcessor*proc) CODE;

#include "cmd.tlang"

#undef DEF

		if (func == NULL) {
			continue;
		}

		char arg_param = exec->tGetc();

		// Arguments are 4 bytes after
		if (arg_param == (char) 0) {
			*arg_buf = tConvertBytes<PROCESSOR_TYPE>(exec->tGetCurrentPointer());
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
