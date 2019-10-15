#ifndef T_LANGUAGE_HANDLER_V2
#define T_LANGUAGE_HANDLER_V2

#include "tLanguageHandlerUtilities.h"
#include "../tUtilities/tFileHandler.h"

using namespace tFileHandler;
using namespace tLanguageHandlerUtilities;

namespace tLanguageHandlerV2 {

void tCompile(tFile *source, tFile *exe) {
	if (source == NULL || exe == NULL) {
		tThrowException("Something went wrong!");
	}

	unsigned line_length = 0;
	char *line = NULL;

	char parse_symbols[1] = { ' ' };

	source->tStartMapping();
	exe->tStartMapping(2 * tGetFileSize(source->tGetName()));
	while (source->tHasMoreSymbs()) {
		line = source->tReadLine(line_length);

		unsigned total_divs = 0;

		char **divs = tParse(line, line_length, parse_symbols, 1, total_divs);

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

		if (id == -1 || total_divs - 1 > Max_arguments) {
			delete divs;
			tThrowException("Compilation error!");
		}

		// Writing id of this command
		exe->tWritec(id);

		// Writing quantity of parameters
		exe->tWritec((char) (total_divs - 1));

		for (unsigned i = 1; i < total_divs; i++) {

			char address = (
					(tStrlen(divs[i]) > 1) ? tGetRegisterIndex(divs[i], 2) : -1);

			if (address != -1) {
				exe->tWritec(address);
			} else {
				exe->tWritec((char) (0));

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
		delete divs;
	}

	exe->tWritec((char) 0);

	source->tStopMapping();
	exe->tStopMapping();

}

void tInvoke(tFile *exec) {
	tAssert(exec != NULL);

	tProcessor *processor = new tProcessor();

	PROCESSOR_TYPE *arg_buf[Max_arguments];
	PROCESSOR_TYPE constant_pointers[Max_arguments];

	exec->tStartMapping();
	while (exec->tHasMoreSymbs()) {
		char id = exec->tGetc();

		tProcFunction func = NULL;

#define T_PROC_FUNC(NAME, ID, CODE) if (id == ID) func = [](PROCESSOR_TYPE**args, unsigned totalArgs, tProcessor*proc) CODE;
#include "tStandartDefs.h"
#include "cmd.tlang"
#include "tStandartUndefs.h"
#undef T_PROC_FUNC

		if (func == NULL) {
			continue;
		}

		unsigned total_args = (unsigned) (exec->tGetc());

		for (unsigned i = 0; i < total_args; i++) {
			char arg_param = exec->tGetc();

			PROCESSOR_TYPE *arg_p = NULL;

			// Arguments are 4 bytes after
			if (arg_param == (char) 0) {
				constant_pointers[i] = tConvertBytes<PROCESSOR_TYPE>(
						exec->tGetCurrentPointer());
				exec->tMovePointer(4);

				arg_p = &constant_pointers[i];
			}
			// Argument is register
			else {
				arg_p = &(processor->regs[(unsigned) arg_param]);
			}
			*(arg_buf + i) = arg_p;
		}

		func(arg_buf, total_args, processor);
	}
	exec->tStopMapping();

	delete processor;
}

}

#endif
