#ifndef T_LANG
#define T_LANG

#include "../tUtilities/tString.h"
#include "../tUtilities/tUtilities.h"
#include "../tUtilities/tCallTreeBuilder.h"
#include "bits/stdc++.h"
#include "../tStorage/tTreap.h"

using namespace std;
using namespace tUtilities;
using namespace tStorage;

const unsigned MAX_CHILDREN = 200;

class tLangNode {

public:
	tString name;

	tLangNode *children[MAX_CHILDREN];

	unsigned size = 0;

	void dfs_out() {
		__tGoInto(name);
		for (unsigned i = 0; i < size; i++) {
			children[i]->dfs_out();
		}
		__tGoOut();
	}

	tLangNode(tString str) :
			name(str), size(0) {
	}

	//! Removes and gives last child.
	tLangNode* pullLast() {
		size--;
		return children[size];
	}

	tLangNode(tString str, tLangNode *f, tLangNode *s) :
			name(str), size(2) {
		children[0] = f;
		children[1] = s;
	}

	void addChild(tLangNode *ch) {
		children[size++] = ch;
	}

};

namespace tLangParser {

char *s = NULL;
char *array = NULL;

void init(tString text) {
	array = text.tToPlainArray();
	s = array;
}

tLangNode* code();
tLangNode* func();
tLangNode* ifs();
tLangNode* returns();
void E();
tLangNode* T();
tLangNode* P();
tLangNode* statement();
tLangNode* block();
tLangNode* action();
tLangNode* assign();
tLangNode* create();
tLangNode* returns();
tLangNode* expr();
tLangNode* N();
tLangNode* name();
tLangNode* useFunc();
tLangNode* whiles();
tLangNode* V();

bool V_();
bool useFunc_();
bool code_();
bool func_();
bool ifs_();
bool returns_();
bool E_();
bool T_();
bool P_();
bool statement_();
bool block_();
bool action_();
bool assign_();
bool create_();
bool returns_();
bool expr_();
bool N_();
bool name_();
bool whiles_();

tLangNode* G();

tLangNode* G() {

	DeleteFileA("CALL_HIERARCHY_GRAMMAR");
	tFile *fl = new tFile("CALL_HIERARCHY_GRAMMAR");
	__tStartMonitoring(fl);

	__tGoInto("G");

	E();
	tLangNode *root = code();

	__tGoOut();

	__tEndMonitoring();
	delete fl;
	tCreateDotImg("CALL_HIERARCHY_GRAMMAR", "CALL_HIERARCHY_GRAMMAR_IMG");

	if (*s != '\0') {
		cout << "END PART: " << s << "\n";
		tThrowException("ERROR WHILE PARSING! END IS NOT REACHED! ");
	}

	__tFastStart();

	root->dfs_out();

	__tFastEnd();

	return root;
}

tLangNode* code() {
	tLangNode *res = new tLangNode("code");

	E();

	while (func_()) {
		res->addChild(func());
	}

	return res;
}

bool func_() {
	return *(s) == 'm' && *(s + 1) == 'e' && *(s + 2) == 't' && *(s + 3) == 'h';
}

tLangNode* func() {
	__tGoInto("func");
	tLangNode *res = new tLangNode("func");

	s += 6; //method
	E();
	s++; //(
	E();
	res->addChild(name());
	E();
	s++; //)
	E();
	res->addChild(name());
	E();
	res->addChild(block());
	E();

	__tGoOut();
	return res;
}

tLangNode* statement() {
	__tGoInto("statement");
	bool ifs__ = ifs_();
	bool block__ = block_();
	bool whiles__ = whiles_();

	tLangNode *res = NULL;
	if (ifs__) {
		res = ifs();
	}
	if (block__) {
		res = block();
	}
	if (whiles__) {
		res = whiles();
	}
	E();
	__tGoOut();
	return res;
}

bool whiles_() {
	return *s == 'w' && *(s + 1) == 'h' && *(s + 2) == 'i' && *(s + 3) == 'l'
			&& *(s + 4) == 'e';
}

tLangNode* ifs() {
	__tGoInto("ifs");
	s += 2; // if
	E();
	s++; //(
	tLangNode *res = new tLangNode("if");
	E();
	res->addChild(expr());
	s++;
	//)
	E();
	res->addChild(block());
	__tGoOut();
	return res;
}

bool ifs_() {
	return *s == 'i' && *(s + 1) == 'f';
}

bool returns_() {
	return *(s) == 'r' && *(s + 1) == 'e' && *(s + 2) == 't' && *(s + 3) == 'u';
}

tLangNode* returns() {
	__tGoInto("return");
	tLangNode *res = new tLangNode("return");

	s += 6; //return

	E();
	res->addChild(expr());
	E();

	__tGoOut();
	return res;
}

tLangNode* useFunc() {
	__tGoInto("useFunc");
	tLangNode *res = new tLangNode("useFunc");

	s += 7; //usefunc

	E();

	res->addChild(name());

	s++; //(

	E();

	if (expr_()) {
		res->addChild(expr());
		E();
		while (*s == ',') {
			s++; //,
			E();
			res->addChild(expr());
			E();
		}
	}

	s++; //)

	__tGoOut();
	return res;
}

tLangNode* whiles() {
	tLangNode *res = new tLangNode("while");
	s += 5; //while

	E();
	s++; //(

	E();

	res->addChild(expr());

	s++; //)
	E();

	res->addChild(block());

	return res;
}

tLangNode* block() {
	__tGoInto("block");
	s++; //{
	tLangNode *nd = new tLangNode("{}");
	E();
	while (*s != '}' && (action_() || statement_())) {
		bool action__ = action_();
		bool block__ = statement_();
		if (action__) {
			nd->addChild(action());
			E();
			s++; //;
		}
		if (block__) {
			nd->addChild(statement());
		}
		E();
	}

	s++; //}

	__tGoOut();
	return nd;
}

bool statement_() {
	return ifs_() || block_() || whiles_();
}

bool block_() {
	return *s == '{';
}

tLangNode* action() {
	__tGoInto("action");
	bool assign__ = assign_();
	bool create__ = create_();
	bool useFunc__ = useFunc_();
	bool returns__ = returns_();

	tLangNode *res = NULL;

	if (assign__) {
		res = assign();
	}
	if (create__) {
		res = create();
	}
	if (useFunc__) {
		res = useFunc();
	}
	if (returns__) {
		res = returns();
	}

	__tGoOut();
	return res;
}

bool action_() {
	return (assign_() || create_() || useFunc_() || returns_()) && !statement_();
}

tLangNode* assign() {
	__tGoInto("assign");

	tLangNode *res = new tLangNode("=");

	res->addChild(name());
	E();
	s++;
	E();
	res->addChild(expr());
	__tGoOut();

	return res;
}

bool assign_() {
	return name_() && !create_() && !useFunc_() && !returns_();
}

tLangNode* create() {
	__tGoInto("create");
	s += 3;

	tLangNode *res = new tLangNode("var");

	E();
	res->addChild(name());
	E();
	res->addChild(name());
	__tGoOut();

	return res;
}

bool create_() {
	return *s == 'v' && *(s + 1) == 'a' && *(s + 2) == 'r';
}

tLangNode* expr() {
	__tGoInto("expr");
	tLangNode *res = new tLangNode("+");
	tLangNode *minus = new tLangNode("-");
	E();
	if (*s == '+' || *s == '-') {
		s++;
		minus->addChild(T());
	} else {
		res->addChild(T());
	}
	E();
	while (*s == '+' || *s == '-') {
		char op = *s;
		s++;
		E();
		if (op == '+') {
			res->addChild(T());
		} else {
			minus->addChild(T());
		}
		E();
	}
	__tGoOut();

	if (minus->size == 0) {
		delete minus;
	} else {
		res->addChild(minus);
	}

	return res;
}

tLangNode* T() {
	__tGoInto("T");
	tLangNode *res = new tLangNode("*");

	res->addChild(P());
	E();

	while (*s == '*' || *s == '/') {
		char op = *s;
		s++;
		E();
		tLangNode *p = P();
		E();
		if (op == '*') {
			res->addChild(p);
		} else {
			res->addChild(new tLangNode("/", res->pullLast(), p));
		}
	}

	__tGoOut();
	return res;
}

tLangNode* P() {
	__tGoInto("P");
	tLangNode *res = NULL;
	if (*s == '(') {
		s++;
		res = expr();
		s++;
	} else {
		res = V();
	}

	__tGoOut();
	return res;
}

bool T_() {
	return P_();
}

bool P_() {
	return (*s == '(' || V_() || useFunc_());
}

bool name_() {
	return (*s <= 'Z' && *s >= 'A') || (*s <= 'z' && *s >= 'a') || *s == '_';
}

tLangNode* name() {
	__tGoInto("name");
	tString result = { };

	result += *s;
	s++;
	while ((*s <= 'Z' && *s >= 'A') || (*s <= 'z' && *s >= 'a')
			|| ('0' <= *s && *s <= '9') || *s == '_') {
		result += *s;
		s++;
	}

	tLangNode *res = new tLangNode(result);

	__tGoOut();

	return res;
}

bool expr_() {
	return T_();
}

bool useFunc_() {
	return *s == 'u' && *(s + 1) == 's' && *(s + 2) == 'e' && *(s + 3) == 'F';
}

bool V_() {
	return name_() || N_();
}

tLangNode* V() {
	__tGoInto("V");
	bool name__ = name_() && !useFunc_();
	bool N__ = N_();
	bool useFunc__ = useFunc_();

	tLangNode *res = NULL;

	if (name__) {
		res = new tLangNode("VAR_VALUE");
		res->addChild(name());
	}
	if (N__) {
		res = new tLangNode("NUM_VALUE");
		res->addChild(N());
	}
	if (useFunc__) {
		res = (useFunc());
	}
	__tGoOut();

	return res;
}

tLangNode* N() {
	__tGoInto("N");
	int val = 0;

	for (; N_(); s++) {
		val = val * 10 + (*s - '0');
	}

	tLangNode *res = new tLangNode(val);

	__tGoOut();

	return res;
}

bool N_() {
	return *s <= '9' && '0' <= *s;
}

void E() {
	__tGoInto("E");
	while (E_()) {
		s++;
	}
	__tGoOut();
}

bool E_() {
	return (*s) == ' ' || (*s) == '\n' || (*s) == '\t';
}

}
;

struct tLangType {
	unsigned size;
	tString name;

	tString (*add)(tString, tString);
	tString (*mul)(tString, tString);
	tString (*neg)(tString);
	tString (*div)(tString, tString);
	tString (*toBytes)(tString);
	tString (*fromBytes)(tString);

	tLangType() :
			size(0), name(), add(NULL), mul(NULL), neg(NULL), div(NULL), toBytes(
			NULL), fromBytes(NULL) {
	}

	tLangType(unsigned sz, tString nm, tString (*add_)(tString, tString),
			tString (*mul_)(tString, tString), tString neg_(tString),
			tString (*div_)(tString, tString), tString (*toString_)(tString),
			tString (*fromString_)(tString)) :
			size(sz), name(nm), add(add_), mul(mul_), neg(neg_), div(div_), toBytes(
					toString_), fromBytes(fromString_) {
	}

	int operator<(tLangType &other) {
		return name < other.name;
	}
};

struct tLangVariable {
	tLangType *type;
	tString value;
	tString name;

	tLangVariable(tLangType *type_, tString name_) :
			type(type_), value(), name(name_) {

	}
};

struct tLangMethod {
	tString (*func)(tString*);
	tLangNode *start;
	unsigned argsQuant;
	tString name;
	tLangType *return_type;

	tLangMethod(tString (*f)(tString*), unsigned args, tString name_,
			tLangType *ret) :
			func(f), start(NULL), argsQuant(args), name(name_), return_type(ret) {
	}

	tLangMethod(tLangNode *st, unsigned args, tString name_, tLangType *ret) :
			func(NULL), start(st), argsQuant(args), name(name_), return_type(
					ret) {
	}
};

namespace tLangCompiler {

tLangType *tINT = NULL;

#define METHOD(NAME, ARGS, CODE, RET_TYPE) tString NAME (tString * args) {CODE}
#include "Patterns/DEFAULT_FUNCS"
#undef METHOD

// Root of the code
tLangNode *root;

// Types for variables
map<tString, tLangType*> types;

// Variables
map<tString, tLangVariable*> vars;

// Functions
map<tString, tLangMethod*> funcs;
tString return_value;
tLangMethod *currentMethod = NULL;

// Current type
tLangType *currentType = NULL;
tLangVariable *currentVar = NULL;

void init(tLangNode *rt) {
	return_value = {};
	root = rt;

	// Initializing standard types
#define TYPE(NAME, SIZE, ADD, MUL, NEG, DIV, TO_BTS, FROM_BTS) types[tString(#NAME)] = new tLangType(SIZE, tString(#NAME), \
		[](tString f, tString s) {ADD;}, [] (tString f, tString s) {MUL;}, [] (tString f) {NEG;}, \
				[] (tString f, tString s) {DIV;}, [] (tString data) {TO_BTS;}, [] (tString data) { FROM_BTS;});
#define A(S) S.__array()
#define CONV(TP, STR) (*(TP*)( A(STR) ))
#include "Patterns/TYPES"
#undef CONV
#undef A
#undef TYPE

	// Initializing standart methods
#define METHOD(NAME, ARGS, CODE, RET_TYPE) {tString (*f)(tString*) = NAME; funcs[tString(#NAME)] = new tLangMethod(f, ARGS, tString(#NAME), types[#RET_TYPE]);}
#include "Patterns/DEFAULT_FUNCS"
#undef METHOD

	tINT = types["int"];
}

tString run(tLangNode *cur = NULL) {
	if (cur == NULL) {
		cur = root;
	}
	__tGoInto(cur->name);

	tString res = cur->name;

#define OPERATOR(NAME, CODE) if (cur->name == tString(#NAME)) {CODE;}
#include "Patterns/OPERATORS"
#undef OPERATOR

	__tGoOut();

	return res;
}

}

#endif
