#include "../Expressions/ExpressionHandler.hpp"

// !This function truncates the extra brackets standing on the
//! edge so as not to lose other important brackets and the order of constructing the tree is not violated.
tString cropBrackets(tString val) {
	// Total brackets to remove from the edge
	unsigned total_crop = 0;

	// Calculating how many brackets in the begin of string
	unsigned beg_br = 0;
	while (val[beg_br] == '(') {
		beg_br++;
	}
	// Calculating how many brackets in the end of string
	unsigned end_br = val.tSize() - 1;
	while (val[end_br] == ')') {
		end_br--;
	}

	tList<unsigned> open_br = { };

	// Checking all pairs of brackets.
	// And remove those which are both in begin and end
	// Example ((..).) => (..).
	for (unsigned i = 0; i < val.tSize(); i++) {
		if (val[i] == '(') {
			open_br.tAddLast(i);
		}
		if (val[i] == ')') {
			unsigned beg = open_br.tRemoveLast();
			unsigned end = i;

			if (beg < beg_br && end_br < end) {
				total_crop++;
			}
		}
	}

	// Cropping
	return val.tSubstring(total_crop, val.tSize() - 1 - total_crop);
}

//! this function selects the desired operation in such a way as not to disrupt their execution order
//! and correctly build the tree. this function is rather complicated therefore will not be described.
unsigned chouseOperation(tString val) {
	// Position of operation symbol and quantity of brackets pairs in which it lies.
	map<unsigned, unsigned> points;

	// Checking all operation and calculating quantity of brackets for them all.
	// Example: ((4*7)) => 2 brackets
	unsigned brackets = 0;
	unsigned min_br = 10000000;
	for (unsigned i = 0; i < val.tSize(); i++) {
		// Correct quantity of brackets
		if (val[i] == '(') {
			brackets++;
			continue;
		}
		if (val[i] == ')') {
			brackets--;
			continue;
		}

		// Finding operation
#define OPERATION(SYMB, DIFF_CODE, CODE_CALC) if (val[i] == SYMB) {points[i] = brackets; min_br = tMin(min_br, brackets);}
#include "../Expressions/NodeConstructor"
#undef OPERATION

	}

	// Finding list of operations with same MINIMUM quantity of brackets.
	tList<unsigned> best_ops = { };
	for (auto it = points.begin(); it != points.end(); it++) {
		if (it->second == min_br) {
			best_ops.tAddLast(it->first);
		}
	}

	// Choosing from this list operation with highest priority
#define OPERATION(SYMB, DIFF_CODE, CODE_CALC) for (unsigned i = 0; i < best_ops.tGetSize(); i++) {\
	if (val[best_ops.tGet(i)] == SYMB) {return best_ops.tGet(i);}\
	}
#include "../Expressions/NodeConstructor"
#undef OPERATION

	return val.tSize();
}

unsigned FREE_ID = 0;



ExprNode::ExprNode() :
			left(NULL), right(NULL), value(tString('0')), num_value(0), type(
					constant), id(FREE_ID++) {
	}

ExprNode::ExprNode(double val) :
			left(NULL), right(NULL), value(tString(val)), num_value(val), type(
					constant), id(FREE_ID++) {
	}

ExprNode::ExprNode(char type_, ExprNode *l, ExprNode *r) :
			left(l), right(r), value(tString(type_)), num_value(0), type(
					operation), id(FREE_ID++) {
	}

ExprNode::ExprNode(tString name, ExprNode *arg) :
			left(NULL), right(arg), value(name), num_value(0), type(function), id(FREE_ID++) {
	}

ExprNode::ExprNode(tString val) :
			left(NULL), right(NULL), value(), num_value(0), type(none), id(FREE_ID++) {

		// Cropping brackets
		val = cropBrackets(val);

		// String for the function argument
		tString arg = { };

		// FUNCTION
#define FUNCTION(NAME, DIFF_CODE, CODE_CALC) \
if (type == none && val.tIsPrefix(#NAME)) {\
	type = function; \
	value = {#NAME};\
	arg = val.tCrop('(', ')');\
}
#include "../Expressions/NodeConstructor"
#undef FUNCTION

		if (type != none) {
			right = new ExprNode(arg);
			return;
		}

		// OPERATION
		unsigned pos = chouseOperation(val);
		if (pos != val.tSize()) {
			value = { val[pos] };
			type = operation;
			tString *divs = val.tDivide(pos);
			left = new ExprNode(divs[0]);
			right = new ExprNode(divs[1]);
			return;
		}

		// VARIABLE
		if (!val.tHasOnlyNumbers()) {
			type = variable;
			value = tString(val);
			return;
		}

		// CONSTANT
		value = val;
		num_value = val.tToFloat();
		type = constant;
	}

ExprNode::~ExprNode() {
		if (left != NULL) {
			delete left;
		}
		if (right != NULL) {
			delete right;
		}
	}

bool isOperation(ExprNode *node, char c) {
	if (node == NULL) {
		return 0;
	}
	return (node->type == operation) && (node->value == c);
}

bool isFunction(ExprNode *node) {
	if (node == NULL) {
		return 0;
	}
	return (node->type == function);
}

bool isConstant(ExprNode *node) {
	if (node == NULL) {
		return 0;
	}
	return (node->type == constant);
}

bool isZero(ExprNode *node) {
	if (node == NULL) {
		return 0;
	}
	return (node->type == constant) && (node->num_value == 0);
}

bool isOne(ExprNode *node) {
	if (node == NULL) {
		return 0;
	}
	return ((node->type == constant) && (node->num_value == 1));
}

bool isPolynomial(ExprNode *node) {
	if (node == NULL) {
		return 0;
	}
	if (isOperation(node, '+')) {
		return 1;
	}
	if (isPolynomial(node->left)) {
		return 1;
	}
	if (isPolynomial(node->right)) {
		return 1;
	}

	return 0;
}

//! This function constructs the line on which this tree was built.
tString exprToTexString(ExprNode *node) {
	if (node == NULL) {
		return {};
	}
	tString result = { };
	if (node->left != NULL) {
		if (node->type == operation && node->value != '+' && node->value != '/'
				&& isPolynomial(node->left)) {
			result += '(';
		}
		if (isOperation(node, '/')) {
			result += tString("\\frac{");
		}
		result += exprToTexString(node->left);
		if (node->type == operation && node->value != '+' && node->value != '/'
				&& isPolynomial(node->left)) {
			result += ')';
		}
		if (isOperation(node, '/')) {
			result += '}';
		}
	}
	if (node->type == constant) {
		result += tString(node->num_value);
		if (node->num_value < 0) {
			result = tString('(') + result + ')';
		}
	} else {
		if (node->value != '/') {
			result += tString(node->value);
		}
	}
	if (node->right != NULL) {
		if (node->type == operation && node->value != '+' && node->value != '/'
				&& isPolynomial(node->right)) {
			result += '(';
		}
		if (isOperation(node, '/')) {
			result += '{';
		}
		if (isFunction(node)) {
			result += tString('(');
		}
		result += exprToTexString(node->right);
		if (isFunction(node)) {
			result += tString(')');
		}
		if (isOperation(node, '/')) {
			result += '}';
		}
		if (node->type == operation && node->value != '+' && node->value != '/'
				&& isPolynomial(node->right)) {
			result += ')';
		}
	}
	return result;
}
//! This function constructs the line on which this tree was built.
tString exprToString(ExprNode *node) {
	if (node == NULL) {
		return {};
	}
	tString result = { };
	if (node->left != NULL) {
		if (node->type == operation && node->value != '+'
				&& isPolynomial(node->left)) {
			result += '(';
		}
		if (node->type == operation && node->value == '/') {
			result += '(';
		}
		result += exprToString(node->left);
		if (node->type == operation && node->value == '/') {
			result += ')';
		}
		if (node->type == operation && node->value != '+'
				&& isPolynomial(node->left)) {
			result += ')';
		}
	}
	if (node->type == constant) {
		result += tString(node->num_value);
		if (node->num_value < 0) {
			result = tString('(') + result + ')';
		}
	} else {
		result += tString(node->value);
	}
	if (node->right != NULL) {
		if (node->type == operation && node->value != '+'
				&& isPolynomial(node->right)) {
			result += '(';
		}
		if (isFunction(node)) {
			result += tString('(');
		}
		if (node->type == operation && node->value == '/') {
			result += '(';
		}
		result += exprToString(node->right);
		if (node->type == operation && node->value == '/') {
			result += ')';
		}
		if (isFunction(node)) {
			result += tString(')');
		}
		if (node->type == operation && node->value != '+'
				&& isPolynomial(node->right)) {
			result += ')';
		}
	}
	return result;
}

ExprNode& operator+(ExprNode &a, ExprNode &b) {
	return *new ExprNode('+', &a, &b);
}

ExprNode& operator*(ExprNode &a, ExprNode &b) {
	return *new ExprNode('*', &a, &b);
}

ExprNode& operator/(ExprNode &a, ExprNode &b) {
	return *new ExprNode('/', &a, &b);
}

void exprFindAllVariables(ExprNode *node, tList<tString> *dest) {
	if (node == NULL) {
		return;
	}
	if (node->type == variable && (!dest->tContains(node->value))) {
		dest->tAddLast(node->value);
	}

	exprFindAllVariables(node->left, dest);
	exprFindAllVariables(node->right, dest);
}

//! This function copy this vertex and the entire subtree of this vertex
ExprNode* exprCopy(ExprNode *node) {
	if (node == NULL) {
		return NULL;
	}
	ExprNode *result = new ExprNode();
	result->type = node->type;
	result->value = node->value;
	result->num_value = node->num_value;
	if (node->left != NULL) {
		result->left = exprCopy(node->left);
	}
	if (node->right != NULL) {
		result->right = exprCopy(node->right);
	}
	return result;
}

//! This function cleans the equation tree by
//! removing the extra branches that contain either 0 or 1 or other unnecessary data.
//! 1. Delete this node if it is multiplication and any child is NULL. (x*0 -> 0)
//! 2. Replace this node with nonnull child if it is sum and one child is NULL. (x + 0 -> x)
//! 3. If it is multiplication and one child is 1 then it is replaced with other child. (x * 1 -> x)
//! 4. If it is pow and power is 1 then it is replaced with other child. (x^1 -> x)
unsigned exprCleanup(ExprNode *&node) {
	ExprNode *todelete = NULL;

	unsigned total = 0;

#define COL_GARB todelete = node; total++;

	// 1.
	if (isOperation(node, '*')) {
		if (isZero(node->left)) {
			COL_GARB
			node = new ExprNode();
		}
		if (isZero(node->right)) {
			COL_GARB
			node = new ExprNode();
		}
	}

	// 2.
	if (isOperation(node, '+')) {
		if (isZero(node->left)) {
			COL_GARB
			node = exprCopy(node->right);
		}
		if (isZero(node->right)) {
			COL_GARB
			node = exprCopy(node->left);
		}
	}

	// 3.
	if (isOperation(node, '*')) {
		if (isOne(node->left)) {
			COL_GARB
			node = exprCopy(node->right);
		}
		if (isOne(node->right)) {
			COL_GARB
			node = exprCopy(node->left);
		}
	}

	// 4.
	if (isOperation(node, '^')) {
		if (isOne(node->left)) {
			COL_GARB
			node = exprCopy(node->right);
		}
		if (isOne(node->right)) {
			COL_GARB
			node = exprCopy(node->left);
		}
	}

#undef COL_GARB

	// Deleting trash
	if (todelete != NULL) {
		delete todelete;
	}

	if (node->left != NULL) {
		total += exprCleanup(node->left);
	}
	if (node->right != NULL) {
		total += exprCleanup(node->right);
	}

	return total;
}

//! This function converts the equation tree into the language format DOT.
void tConvertToDot(ExprNode *node, tFile *dest, bool init = 1) {
	if (init) {
		dest->tStartMapping(2000);
		dest->tWriteLine("digraph graphname {");
	}
	dest->tWrite("vrt");
	dest->tWriteNum<unsigned>(node->id);
	dest->tWrite("[label=");
	dest->tWritec('"');
	dest->tWrite(node->value);
	dest->tWritec('"');
	dest->tWriteLine("]");
	if (node->left != NULL) {
		dest->tWrite("vrt");
		dest->tWrite(node->id);
		dest->tWrite(" -> ");
		dest->tWrite("vrt");
		dest->tWriteLine(node->left->id);
		tConvertToDot(node->left, dest, 0);
	}
	if (node->right != NULL) {
		dest->tWrite("vrt");
		dest->tWrite(node->id);
		dest->tWrite(" -> ");
		dest->tWrite("vrt");
		dest->tWriteLine(node->right->id);
		tConvertToDot(node->right, dest, 0);
	}
	if (init) {
		dest->tWriteLine("}");
		dest->tStopMapping();
	}
}

//! Converts node tree into dot format and convert it to image.
void exprSaveDotImage(tString imgName, ExprNode *node) {
	tString fileName = { "TEMP_DOT_SAVE" };
	DeleteFileA("TEMP_DOT_SAVE");
	tFile *fl = new tFile(fileName);
	tConvertToDot(node, fl);
	delete fl;
	tCreateDotImg(fileName, imgName);
	DeleteFileA("TEMP_DOT_SAVE");
}

//! This function substitutes the value of the
//! variables and calculates the numerical value of the expression that represents this tree.
double exprCalculate(const ExprNode *const node, map<tString, double> vars) {
	if (node == NULL) {
		return 0;
	}
	double l = 0;
	double r = 0;

	l = exprCalculate(node->left, vars);
	r = exprCalculate(node->right, vars);

	double res = 0;

	if (node->type == operation) {
#define OPERATION(SYMB, CODE_D, CODE_C) if (node->value == SYMB) {CODE_C;}
#include "../Expressions/NodeConstructor"
#undef OPERATION
	}
	if (node->type == function) {
#define FUNCTION(NAME, CODE_D, CODE_C) if (node->value == tString(#NAME)) {CODE_C;}
#include "../Expressions/NodeConstructor"
#undef FUNCTION
	}
	if (node->type == variable) {
		if (vars.count(node->value) == 0) {
			std::cout << "NO SUCH VARIABLE: ";
			node->value.tOut();
		}
		res = vars[node->value];
	}
	if (node->type == constant) {
		res = node->num_value;
	}

	return res;
}

//! This function differentiate given in node tree equation.
//! Differentiation is carried tOut according to the variable that is stored in the string.
ExprNode* exprDifferentiate(const ExprNode *node, const tString var__) {
	if (node == NULL) {
		return NULL;
	}

	// These all is to make access from Node constructor easier
#define l (*node->left)
#define r (*node->right)
#define dl (*exprDifferentiate(&l, var__))
#define dr (*exprDifferentiate(&r, var__))
#define cl (*exprCopy(&l))
#define cr (*exprCopy(&r))
#define num(A) (*new ExprNode(A))
#define pow(A, B) (*(new ExprNode('^', &A, &B)))
#define func(A, B) (*(new ExprNode(tString(#A), &B)))

	switch (node->type) {
	case constant:
		return new ExprNode();
	case operation:

#define OPERATION(SYMB, CODE, CODE_C) if (node->value == SYMB) {CODE;}
#include "../Expressions/NodeConstructor"
#undef OPERATION

		break;
	case function:

#define FUNCTION(NAME, CODE, CODE_C) if (node->value == tString(#NAME)) {CODE;}
#include "../Expressions/NodeConstructor"
#undef FUNCTION

		break;
	case variable:
		return ((var__ == node->value) ? &num(1) : &num(0));
	case none:
		break;
	}

#undef l
#undef r
#undef cr
#undef cl
#undef dr
#undef dl
#undef num
#undef pow
#undef func

	return NULL;
}

