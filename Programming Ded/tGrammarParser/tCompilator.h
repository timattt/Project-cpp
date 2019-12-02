#ifndef T_LANG
#define T_LANG

#include "tGrammarParser.h"

using namespace std;
using namespace tUtilities;
using namespace tStorage;

namespace tLangCompilator {

tLangType *tINT = NULL;

#define METHOD(NAME, ARGS, CODE, RET_TYPE, ARG_TYPE) tString NAME (tString * args) {CODE}
#include "InterpritatorPatterns/DEFAULT_FUNCS"
#undef METHOD

// Root of the code
tLangNode *root;

// Types for variables
map<tString, tLangType*> types;

// Variables
map<tString, tLangVariable*> vars;
tList<map<tString, tLangVariable*>> stack;

// Functions
map<tString, tLangMethod*> funcs;
tString return_value = { };
tLangMethod *currentMethod = NULL;

// Current type
tLangType *currentType = NULL;
tLangVariable *currentVar = NULL;

void init(tLangNode *rt) {
	root = rt;

	// Initializing standard types
#define TYPE(NAME, SIZE, ADD, MUL, NEG, DIV, TO_BTS, FROM_BTS) types[tString(#NAME)] = new tLangType(SIZE, tString(#NAME), \
		[](tString f, tString s) {ADD;}, [] (tString f, tString s) {MUL;}, [] (tString f) {NEG;}, \
				[] (tString f, tString s) {DIV;}, [] (tString data) {TO_BTS;}, [] (tString data) { FROM_BTS;});
#define A(S) S.__array()
#define CONV(TP, STR) (*(TP*)( A(STR) ))
#include "InterpritatorPatterns/TYPES"
#undef CONV
#undef A
#undef TYPE

	// Initializing standart methods
#define METHOD(NAME, ARGS, CODE, RET_TYPE, ARG_TYPE) {tString (*f)(tString*) = NAME; funcs[tString(#NAME)]\
	= new tLangMethod(f, ARGS, tString(#NAME), types[#RET_TYPE]);}\
	for (unsigned i = 0; i < ARGS; i++) {funcs[tString(#NAME)]->args[i] = new tLangVariable(types[#ARG_TYPE], tString("def_arg") + tString(i));}
#include "InterpritatorPatterns/DEFAULT_FUNCS"
#undef METHOD

	tINT = types["int"];
}

void newVarBranch() {
	map<tString, tLangVariable*> *br = new map<tString, tLangVariable*>();

	stack.tAddLast(*br);
	vars = *br;
}

void flushBranch() {
	map<tString, tLangVariable*> br = stack.tRemoveLast();
	for (auto it = br.begin(); it != br.end(); it++) {
		//delete it->second;
	}
	vars = stack.tGetLastElement();
}

tString run(tLangNode *cur = NULL) {
	if (cur == NULL) {
		cur = root;
	}
	__tGoInto(cur->name);

	tString res = cur->name;

#define OPERATOR(NAME, CODE) if (cur->name == tString(#NAME)) {CODE;}
#include "InterpritatorPatterns/OPERATORS"
#undef OPERATOR

	__tGoOut();

	return res;
}

}

#endif
