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

class tLangParser {
private:
	char *s;
	char *array;
	tString text;

public:
	tLangParser(tString text) :
			s(NULL), array(NULL), text() {
		array = text.tToPlainArray();
		s = array;
	}
	~tLangParser() {
		delete array;
	}

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
		__tGoInto("code");

		tLangNode *res = new tLangNode(";");

		while (action_()) {
			res->addChild(action());
			E();
			s++;
			E();
		}

		__tGoOut();

		return res;
	}

	bool code_() {
		return action_();
	}

	tLangNode* action() {
		__tGoInto("action");
		bool assign__ = assign_();
		bool create__ = create_();
		bool out__ = out_();

		tLangNode *res = NULL;

		if (assign__) {
			res = assign();
		}
		if (create__) {
			res = create();
		}
		if (out__) {
			res = out();
		}
		__tGoOut();

		return res;
	}

	bool action_() {
		return assign_() || create_() || out_();
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
		return name_() && !create_() && !out_();
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
		return (*s == '(' || V_());
	}

	bool name_() {
		return *s <= 'z' && 'a' <= *s;
	}

	tLangNode* name() {
		__tGoInto("name");
		tString result = { };

		result += *s;
		s++;
		while ((*s <= 'z' && *s >= 'a') || ('0' <= *s && *s <= '9') || *s == '_') {
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

	bool out_() {
		return *s == 'o' && *(s + 1) == 'u' && *(s + 2) == 't';
	}

	tLangNode* out() {
		__tGoInto("out");
		tLangNode *res = new tLangNode("out");
		s += 3;
		E();
		res->addChild(name());
		__tGoOut();

		return res;
	}

	bool V_() {
		return name_() || N_();
	}

	tLangNode* V() {
		__tGoInto("V");
		bool name__ = name_();
		bool N__ = N_();

		tLangNode *res = new tLangNode("VALUE");

		if (name__) {
			res->addChild(name());
		}
		if (N__) {
			res->addChild(N());
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

};

struct ttType {
	unsigned size;
	tString name;

	tString (*add)(tString, tString);
	tString (*mul)(tString, tString);
	tString (*sub)(tString, tString);
	tString (*div)(tString, tString);
	tString (*toBytes)(tString);
	tString (*fromBytes)(tString);

	ttType() :
			size(0), name(), add(NULL), mul(NULL), sub(NULL), div(NULL), toBytes(
			NULL), fromBytes(NULL) {
	}

	ttType(unsigned sz, tString nm, tString (*add_)(tString, tString),
			tString (*mul_)(tString, tString), tString sub_(tString, tString),
			tString (*div_)(tString, tString), tString (*toString_)(tString), tString (*fromString_)(tString)) :
			size(sz), name(nm), add(add_), mul(mul_), sub(sub_), div(div_), toBytes(
					toString_), fromBytes(fromString_) {
	}

	int operator<(ttType &other) {
		return name < other.name;
	}
};

struct tVariable {
	ttType *type;
	tString value;
	tString name;

	tVariable(ttType *type_, tString name_) :
			type(type_), value(), name(name_) {

	}
};

class tLangCompiler {

	// Root of the code
	tLangNode *root;

	// Types for variables
	map<tString, ttType*> types;

	// Variables
	map<tString, tVariable*> vars;

	// Current type
	ttType *currentType = NULL;
	tVariable *currentVar = NULL;

public:

	tLangCompiler(tLangNode *rt) :
			root(rt), types( { }) {

		// Initializing standard types
#define TYPE(NAME, SIZE, ADD, SUB, MUL, DIV, TO_BTS, FROM_BTS) types[tString(#NAME)] = new ttType(SIZE, tString(#NAME), \
		[](tString f, tString s) {ADD;}, [] (tString f, tString s) {MUL;}, [] (tString f, tString s) {SUB;}, \
				[] (tString f, tString s) {DIV;}, [] (tString data) {TO_BTS;}, [] (tString data) { FROM_BTS;});
#define A(S) S.__array()
#define CONV(TP, STR) (*(TP*)( A(STR) ))
#include "Patterns/TYPES"
#undef CONV
#undef A
#undef TYPE

	}

	tString run(tLangNode *cur = NULL) {

		if (cur == NULL) {
			cur = root;
		}

		__tGoInto(cur->name);

		//cur->name.out();

#define OPERATOR(NAME, CODE) if (cur->name == tString(#NAME)) {CODE;}
#include "Patterns/OPERATORS"
#undef OPERATOR

		__tGoOut();
		return cur->name;
	}

};

#endif
