#include "LangParser.hpp"

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

char *s = NULL;
char *parseArray = NULL;
char *end = NULL;

tLangNode* intrParse(tString text) {
	parseArray = text.tToPlainArray();
	s = parseArray;
	end = s + strlen(s);

	const char * DOT_NAME = "INTR_PROGRAM_GRAMMAR";
	const char * IMG_NAME = "INTR_PROGRAM_GRAMMAR_IMG";

	DeleteFileA(DOT_NAME);
	tFile *fl = new tFile(DOT_NAME);
	__tStartMonitoring(fl);

	__tGoInto("G");

	E();
	tLangNode *root = code();

	__tGoOut();

	__tEndMonitoring();
	delete fl;
	tCreateDotImg(DOT_NAME, IMG_NAME);
	DeleteFileA(DOT_NAME);

	if (*s != '\0') {
		std::cout << "END PART: [" << s << "]\n";
		THROW_T_EXCEPTION("ERROR WHILE PARSING! END IS NOT REACHED! ");
	}

	__tFastStart("INTR_PROGRAM_STRUCTURE");

	root->dfs_out();

	__tFastEnd();

	return root;
}

#define FARM(STR) {int vvv = strlen(STR);ENSURE(s + vvv < end, "Out of range");for (int l = 0; l < vvv; l++) {if (s[l] != STR[l]) {THROW_T_EXCEPTION((tString("Expected ") + STR).tToPlainArray());}}s+=vvv; }

#define PLUS_ONE \
s++;\
	ENSURE(s <= end, "Out of range");

#define PLUS(K)\
	s+=K;\
	ENSURE(s <= end, "Out of range");
#define TRY_PLUS(K) //if ((s + K) >= end) {return 0;}

tLangNode* code() {
	tLangNode *res = new tLangNode("code");

	E();

	while (func_()) {
		res->addChild(func());
	}

	return res;
}

bool func_() {
	TRY_PLUS(3);
	return *(s) == 'm' && *(s + 1) == 'e' && *(s + 2) == 't' && *(s + 3) == 'h';
}

tLangNode* func() {
	__tGoInto("func");
	tLangNode *res = new tLangNode("func");

	FARM("method");
	E();
	FARM("(");
	E();
	res->addChild(name());
	E();
	FARM(")");
	E();
	res->addChild(name());
	E();
	FARM("(");
	E();
	if (create_()) {
		res->addChild(create());
		E();
	}
	FARM(")");
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
	FARM("if");
	E();
	FARM("(");
	tLangNode *res = new tLangNode("if");
	E();
	res->addChild(expr());
	FARM(")");
	E();
	res->addChild(block());
	__tGoOut();
	return res;
}

bool ifs_() {
	TRY_PLUS(1);
	return *s == 'i' && *(s + 1) == 'f';
}

bool returns_() {
	TRY_PLUS(3);
	return *(s) == 'r' && *(s + 1) == 'e' && *(s + 2) == 't' && *(s + 3) == 'u';
}

tLangNode* returns() {
	__tGoInto("return");
	tLangNode *res = new tLangNode("return");

	FARM("return"); //return

	E();
	res->addChild(expr());
	E();

	__tGoOut();
	return res;
}

tLangNode* useFunc() {
	__tGoInto("useFunc");
	tLangNode *res = new tLangNode("useFunc");

	FARM("useFunc"); //usefunc

	E();

	res->addChild(name());

	FARM("(");

	E();

	if (expr_()) {
		res->addChild(expr());
		E();
		while (*s == ',') {
			FARM(",");
			E();
			res->addChild(expr());
			E();
		}
	}

	FARM(")");

	__tGoOut();
	return res;
}

tLangNode* whiles() {
	tLangNode *res = new tLangNode("while");
	FARM("while");

	E();
	FARM("(");

	E();

	res->addChild(expr());

	FARM(")");
	E();

	res->addChild(block());

	return res;
}

tLangNode* block() {
	__tGoInto("block");
	FARM("{");
	tLangNode *nd = new tLangNode("{}");
	E();
	while (*s != '}' && (action_() || statement_())) {
		bool action__ = action_();
		bool block__ = statement_();
		if (action__) {
			nd->addChild(action());
			E();
			FARM(";");
		}
		if (block__) {
			nd->addChild(statement());
		}
		E();
	}

	FARM("}");

	__tGoOut();
	return nd;
}

bool statement_() {
	return ifs_() || block_() || whiles_();
}

bool block_() {
	TRY_PLUS(0);
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
	FARM("=");
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
	FARM("var");

	tLangNode *res = new tLangNode("var");

	E();
	res->addChild(name());
	E();
	res->addChild(name());
	__tGoOut();

	return res;
}

bool create_() {
	TRY_PLUS(2);
	return *s == 'v' && *(s + 1) == 'a' && *(s + 2) == 'r';
}

tLangNode* expr() {
	__tGoInto("expr");
	tLangNode *res = new tLangNode("+");
	tLangNode *minus = new tLangNode("-");
	E();
	if (*s == '-') {
		FARM("-");
		E();
		minus->addChild(T());
	} else {
		if (*s == '+') {
			FARM("+");
		}
		E();
		res->addChild(T());
	}

	E();
	while (*s == '+' || *s == '-') {
		char op = *s;
		if (*s == '+') {
			FARM("+");
		} else {
			FARM("-");
		}
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
		if (op == '*') {
			FARM("*");
		} else {
			FARM("/");
		}
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
		FARM("(");
		res = expr();
		FARM(")");
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
	TRY_PLUS(0);
	return (*s == '(' || V_() || useFunc_());
}

bool name_() {
	TRY_PLUS(0);
	return (*s <= 'Z' && *s >= 'A') || (*s <= 'z' && *s >= 'a') || *s == '_';
}

tLangNode* name() {
	__tGoInto("name");
	tString result = { };

	result += *s;
	PLUS_ONE;
	while ((*s <= 'Z' && *s >= 'A') || (*s <= 'z' && *s >= 'a')
			|| ('0' <= *s && *s <= '9') || *s == '_') {
		result += *s;
		PLUS_ONE;
	}

	tLangNode *res = new tLangNode(result);

	__tGoOut();

	return res;
}

bool expr_() {
	return T_();
}

bool useFunc_() {
	TRY_PLUS(3);
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

	while (N_()) {
		val = val * 10 + (*s - '0');
		PLUS_ONE;
	}

	tLangNode *res = new tLangNode(val);

	__tGoOut();

	return res;
}

bool N_() {
	TRY_PLUS(0);
	return *s <= '9' && '0' <= *s;
}

void E() {
	__tGoInto("E");
	while (E_()) {
		PLUS_ONE;
	}
	__tGoOut();
}

bool E_() {
	TRY_PLUS(0);
	return (*s) == ' ' || (*s) == '\n' || (*s) == '\t';
}


tLangType::tLangType() :
			size(0), name(), add(NULL), mul(NULL), neg(NULL), div(NULL), toBytes(
			NULL), fromBytes(NULL) {
	}

tLangType::tLangType(unsigned sz, tString nm, tString (*add_)(tString, tString),
			tString (*mul_)(tString, tString), tString neg_(tString),
			tString (*div_)(tString, tString), tString (*toString_)(tString),
			tString (*fromString_)(tString)) :
			size(sz), name(nm), add(add_), mul(mul_), neg(neg_), div(div_), toBytes(
					toString_), fromBytes(fromString_) {
	}

	int tLangType::operator<(tLangType &other) {
		return name < other.name;
	}


	tLangVariable::tLangVariable(tLangType *type_, tString name_) :
			type(type_), value(), name(name_) {

	}


tLangMethod::tLangMethod(tString (*f)(tString*), unsigned args_q, tString name_,
			tLangType *ret) :
			func(f), start(NULL), argsQuant(args_q), name(name_), return_type(
					ret), args(
					(tLangVariable**) calloc(args_q, sizeof(tLangVariable*))) {
	}

tLangMethod::tLangMethod(tLangNode *st, unsigned args_q, tString name_, tLangType *ret) :
			func(NULL), start(st), argsQuant(args_q), name(name_), return_type(
					ret), args(
					(tLangVariable**) calloc(args_q, sizeof(tLangVariable*))) {
	}


