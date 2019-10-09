#ifndef T_PROCESSOR_V1
#define T_PROCESSOR_V1

#include "../tUtilities/tFileHandler.h"
#include "../tUtilities/tUtilities.h"
#include "tLanguageHandlerUtilities.h"

using namespace tFileHandler;
using namespace tLanguageHandlerUtilities;

namespace tLanguageHandlerV1 {

void tCompile(tFile *source, tFile *exe, tFuncLib *lib) {
	if (source == NULL || exe == NULL || lib == NULL) {
		tThrowException("Something went wrong!");
	}

	unsigned line_length = 0;
	char *line = NULL;

	char *exe_buf = (char*) calloc(200, sizeof(char));
	unsigned buf_size = 0;

	char parse_symbols[2] = { ' ', '	' };

	source->tStartMapping();
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

		int id = lib->name__id(divs[0]);

		if (id == -1) {
			tThrowException("Compilation error!");
		}
		exe_buf[buf_size] = id;
		buf_size++;

		char address = (
				(total_divs >= 2 && tStrlen(divs[1]) > 1) ?
						tGetRegisterIndex(divs[1], 2) : -1);

		if (address != -1) {
			exe_buf[buf_size] = address;
			buf_size++;
		} else {
			exe_buf[buf_size] = ((char) (total_divs >= 2 ? 0 : 127));
			buf_size++;

			if (total_divs >= 2) {
				int value = std::atoi(divs[1]);
				tWriteBytes<int>(value, exe_buf + buf_size);
				buf_size+=4;
			}
		}

		delete divs;
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
