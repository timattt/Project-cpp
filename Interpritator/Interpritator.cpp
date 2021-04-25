#include "Interpritator.hpp"

tLangType *tINT = NULL;

#define METHOD(NAME, ARGS, CODE, RET_TYPE, ARG_TYPE) tString NAME (tString * args) {CODE}
#include "../Interpritator/InterpritatorPatterns/DEFAULT_FUNCS"
#undef METHOD

// Root of the code
tLangNode *root;

// Types for variables
map<tString, tLangType*> types;

// Variables
map<tString, tLangVariable*> vars;
tList<map<tString, tLangVariable*>> mem_stack;

// Functions
map<tString, tLangMethod*> funcs;
tString return_value = { };
tLangMethod *currentMethod = NULL;

// Current type
tLangType *currentType = NULL;
tLangVariable *currentVar = NULL;

void intrInit(tLangNode *rt) {
	NOT_NULL(rt);
	root = rt;

	// Initializing standard types
#define TYPE(NAME, SIZE, ADD, MUL, NEG, DIV, TO_BTS, FROM_BTS) types[tString(#NAME)] = new tLangType(SIZE, tString(#NAME), \
		[](tString f, tString s) {ADD;}, [] (tString f, tString s) {MUL;}, [] (tString f) {NEG;}, \
				[] (tString f, tString s) {DIV;}, [] (tString data) {TO_BTS;}, [] (tString data) { FROM_BTS;});
#define A(S) S.__tArray()
#define CONV(TP, STR) (*(TP*)( A(STR) ))
#include "../Interpritator/InterpritatorPatterns/TYPES"
#undef CONV
#undef A
#undef TYPE

	// Initializing standart methods
#define METHOD(NAME, ARGS, CODE, RET_TYPE, ARG_TYPE) {tString (*f)(tString*) = NAME; funcs[tString(#NAME)]\
	= new tLangMethod(f, ARGS, tString(#NAME), types[#RET_TYPE]);}\
	for (unsigned i = 0; i < ARGS; i++) {funcs[tString(#NAME)]->args[i] = new tLangVariable(types[#ARG_TYPE], tString("def_arg") + tString(i));}
#include "../Interpritator/InterpritatorPatterns/DEFAULT_FUNCS"
#undef METHOD

	tINT = types["int"];
}

void newVarBranch() {
	map<tString, tLangVariable*> *br = new map<tString, tLangVariable*>();

	mem_stack.tAddLast(*br);
	vars = *br;
}

void flushBranch() {
	map<tString, tLangVariable*> br = mem_stack.tRemoveLast();
	for (auto it = br.begin(); it != br.end(); it++) {
		//delete it->second;
	}
	vars = mem_stack.tGetLastElement();
}

tString intrRunProgram(tLangNode *cur) {
	if (cur == NULL) {
		cur = root;
	}

	if (cur == root) {
		__tFastStart("INTR_PROGRAM_STACK_TRACE");
	}

	__tGoInto(cur->name);

	tString res = cur->name;

#define OPERATOR(NAME, CODE) if (cur->name == tString(#NAME)) {CODE;}
#include "../Interpritator/InterpritatorPatterns/OPERATORS"
#undef OPERATOR

	__tGoOut();

	if (cur == root) {
		__tFastEnd();
	}

	return res;
}

