#pragma once

void treapRun(int argc, char * argv[]);
void comRun(int argc, char * argv[]);
void akRun(int argc, char * argv[]);
void exprRun(int argc, char * argv[]);
void gramRun(int argc, char * argv[]);
void intrRun(int argc, char * argv[]);

#define CREATE_ARG(N) if (argc <= N) {THROW_T_EXCEPTION("Bad arg");} tString arg ## N = argv[N];if (arg ## N .tSize() < 2 || arg ## N [0] != '-' || arg ## N [1] != '-' || arg ## N .tCount('-') != 2) {\
		THROW_T_EXCEPTION("Bad arg");\
		\
	}\
	arg ## N = arg ## N .tCastofSymbol('-');
#define CREATE_NAME_ARG(N) if (argc <= N) {THROW_T_EXCEPTION("Bad arg");} tString arg ## N = argv[N];

