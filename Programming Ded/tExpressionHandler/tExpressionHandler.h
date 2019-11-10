#ifndef T_DIFFERENCIATOR
#define T_DIFFERENCIATOR

#include <map>

#include "../tUtilities/tFileHandler.h"
#include "../tUtilities/tString.h"

using namespace tFileHandler;

namespace tExpressionHandler {

enum nodeType {
	none, operation, constant, function, variable
};

// !This function truncates the extra brackets standing on the
//! edge so as not to lose other important brackets and the order of constructing the tree is not violated.
tString cropBrackets(tString val) {
	unsigned total_crop = 0;

	unsigned beg_br = 0;
	while (val[beg_br] == '(') {
		beg_br++;
	}
	unsigned end_br = val.size - 1;
	while (val[end_br] == ')') {
		end_br--;
	}

	tList<unsigned> open_br = { };

	for (unsigned i = 0; i < val.size; i++) {
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

	return val.tSubstring(total_crop, val.size - 1 - total_crop);
}

//! this function selects the desired operation in such a way as not to disrupt their execution order
//! and correctly build the tree. this function is rather complicated therefore will not be described.
unsigned chouseOperation(tString val) {
	// Position of operation symbol and quantity of brackets.
	map<unsigned, unsigned> points;

	unsigned brackets = 0;
	unsigned min_br = 10000000;
	for (unsigned i = 0; i < val.size; i++) {
		if (val[i] == '(') {
			brackets++;
			continue;
		}
		if (val[i] == ')') {
			brackets--;
			continue;
		}
#define OPERATION(SYMB, DIFF_CODE, CODE_CALC) if (val[i] == SYMB) {points[i] = brackets; min_br = tMin(min_br, brackets);}
#include "../tExpressionHandler/NodeConstructor"
#undef OPERATION

	}

	tList<unsigned> best_ops = { };
	for (auto it = points.begin(); it != points.end(); it++) {
		if (it->second == min_br) {
			best_ops.tAddLast(it->first);
		}
	}

#define OPERATION(SYMB, DIFF_CODE, CODE_CALC) for (unsigned i = 0; i < best_ops.tGetSize(); i++) {\
	if (val[best_ops.tGet(i)] == SYMB) {return best_ops.tGet(i);}\
	}
#include "../tExpressionHandler/NodeConstructor"
#undef OPERATION

	return val.size;
}

unsigned FREE_ID = 0;

class tExprNode {
public:
	// Children
	tExprNode *left;
	tExprNode *right;

	// Value
	tString value;
	double num_value;

	// Type
	nodeType type;

	// Id for dot constructor
	unsigned id = FREE_ID++;

	tExprNode() :
			left(NULL), right(NULL), value(tString('0')), num_value(0), type(
					constant) {
	}

	tExprNode(double val) :
			left(NULL), right(NULL), value(tString(val)), num_value(val), type(
					constant) {
	}

	tExprNode(char type_, tExprNode *l, tExprNode *r) :
			left(l), right(r), value(tString(type_)), num_value(0), type(
					operation) {
	}

	tExprNode(tString name, tExprNode *arg) :
			left(NULL), right(arg), value(name), num_value(0), type(function) {
	}

	tExprNode(tString val) :
			left(NULL), right(NULL), value(), num_value(0), type(none) {
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
#include "../tExpressionHandler/NodeConstructor"
#undef FUNCTION

		if (type != none) {
			right = new tExprNode(arg);
			return;
		}

		// OPERATION
		unsigned pos = chouseOperation(val);
		if (pos != val.size) {
			value = { val[pos] };
			type = operation;
			tString *divs = val.tDivide(pos);
			left = new tExprNode(divs[0]);
			right = new tExprNode(divs[1]);
			return;
		}

		// VARIABLE
		if (!val.tHasOnlyNumbers()) {
			type = variable;
			value = val;
			return;
		}

		// CONSTANT
		value = val;
		num_value = val.tToFloat();
		type = constant;
	}

	~tExprNode() {
		if (left != NULL) {
			delete left;
		}
		if (right != NULL) {
			delete right;
		}
	}
};

bool isOperation(tExprNode *node, char c) {
	if (node == NULL) {
		return 0;
	}
	return (node->type == operation) && (node->value == c);
}

bool isFunction(tExprNode *node) {
	if (node == NULL) {
		return 0;
	}
	return (node->type == function);
}

bool isConstant(tExprNode *node) {
	if (node == NULL) {
		return 0;
	}
	return (node->type == constant);
}

bool isZero(tExprNode *node) {
	if (node == NULL) {
		return 0;
	}
	return (node->type == constant) && (node->num_value == 0);
}

bool isOne(tExprNode *node) {
	if (node == NULL) {
		return 0;
	}
	return (node->type == constant) && (node->num_value == 1);
}

bool isPolynomial(tExprNode *node) {
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
tString tToTexString(tExprNode *node) {
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
		result += tToTexString(node->left);
		if (node->type == operation && node->value != '+' && node->value != '/'
				&& isPolynomial(node->left)) {
			result += ')';
		}
		if (isOperation(node, '/')) {
			result += '}';
		}
	}
	if (node->type == constant) {
		result += tString(node->num_value).tRemoveFractTail();
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
		result += tToTexString(node->right);
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
tString tToString(tExprNode *node) {
	if (node == NULL) {
		return {};
	}
	tString result = { };
	if (node->left != NULL) {
		if (node->type == operation && node->value != '+'
				&& isPolynomial(node->left)) {
			result += '(';
		}
		result += tToString(node->left);
		if (node->type == operation && node->value != '+'
				&& isPolynomial(node->left)) {
			result += ')';
		}
	}
	if (node->type == constant) {
		result += tString(node->num_value).tRemoveFractTail();
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
		result += tToString(node->right);
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

tExprNode& operator+(tExprNode &a, tExprNode &b) {
	return *new tExprNode('+', &a, &b);
}

tExprNode& operator*(tExprNode &a, tExprNode &b) {
	return *new tExprNode('*', &a, &b);
}

tExprNode& operator/(tExprNode &a, tExprNode &b) {
	return *new tExprNode('/', &a, &b);
}

void tFindAllVariables(tExprNode *node, tList<tString> *dest) {
	if (node == NULL) {
		return;
	}
	if (node->type == variable && (!dest->tContains(node->value))) {
		dest->tAddLast(node->value);
	}

	tFindAllVariables(node->left, dest);
	tFindAllVariables(node->right, dest);
}

//! This function copy this vertex and the entire subtree of this vertex
tExprNode* tCopy(tExprNode *node) {
	if (node == NULL) {
		return NULL;
	}
	tExprNode *result = new tExprNode();
	result->type = node->type;
	result->value = node->value;
	result->num_value = node->num_value;
	if (node->left != NULL) {
		result->left = tCopy(node->left);
	}
	if (node->right != NULL) {
		result->right = tCopy(node->right);
	}
	return result;
}

//! This function cleans the equation tree by
//! removing the extra branches that contain either 0 or 1 or other unnecessary data.
//! 1. Delete this node if it is multiplication and any child is NULL. (x*0 -> 0)
//! 2. Replace this node with nonnull child if it is sum and one child is NULL. (x + 0 -> x)
//! 3. If it is multiplication and one child is 1 then it is replaced with other child. (x * 1 -> x)
//! 4. If it is pow and power is 1 then it is replaced with other child. (x^1 -> x)
unsigned tCleanup(tExprNode *&node) {
	tExprNode *todelete = NULL;

	unsigned total = 0;

#define COL_GARB todelete = node; total++;

	// 1.
	if (isOperation(node, '*')) {
		if (isZero(node->left)) {
			COL_GARB
			node = new tExprNode();
		}
		if (isZero(node->right)) {
			COL_GARB
			node = new tExprNode();
		}
	}

	// 2.
	if (isOperation(node, '+')) {
		if (isZero(node->left)) {
			COL_GARB
			node = tCopy(node->right);
		}
		if (isZero(node->right)) {
			COL_GARB
			node = tCopy(node->left);
		}
	}

	// 3.
	if (isOperation(node, '*')) {
		if (isOne(node->left)) {
			COL_GARB
			node = tCopy(node->right);
		}
		if (isOne(node->right)) {
			COL_GARB
			node = tCopy(node->left);
		}
	}

	// 4.
	if (isOperation(node, '^')) {
		if (isOne(node->left)) {
			COL_GARB
			node = tCopy(node->right);
		}
		if (isOne(node->right)) {
			COL_GARB
			node = tCopy(node->left);
		}
	}

#undef COL_GARB

	// Deleting trash
	if (todelete != NULL) {
		delete todelete;
	}

	if (node->left != NULL) {
		total += tCleanup(node->left);
	}
	if (node->right != NULL) {
		total += tCleanup(node->right);
	}

	return total;
}

//! This function converts the equation tree into the language format DOT.
void tConvertToDot(tExprNode *node, tFile *dest, bool init = 1) {
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
void tSaveDotImage(tString imgName, tExprNode *node) {
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
double tCalc(tExprNode *node, map<tString, double> vars) {
	if (node == NULL) {
		return 0;
	}
	double l = 0;
	double r = 0;

	l = tCalc(node->left, vars);
	r = tCalc(node->right, vars);

	double res = 0;

	if (node->type == operation) {
#define OPERATION(SYMB, CODE_D, CODE_C) if (node->value == SYMB) {CODE_C;}
#include "../tExpressionHandler/NodeConstructor"
#undef OPERATION
	}
	if (node->type == function) {
#define FUNCTION(NAME, CODE_D, CODE_C) if (node->value == tString(#NAME)) {CODE_C;}
#include "../tExpressionHandler/NodeConstructor"
#undef FUNCTION
	}
	if (node->type == variable) {
		res = vars[node->value];
	}
	if (node->type == constant) {
		res = node->num_value;
	}

	return res;
}

//! This function differentiate given in node tree equation.
//! Differentiation is carried out according to the variable that is stored in the string.
tExprNode* tDifferentiate(tExprNode *node, tString var) {
	if (node == NULL) {
		return NULL;
	}

	// These all is to make access from Node constructor easier
#define l (*node->left)
#define r (*node->right)
#define dl (*tDifferentiate(&l, var))
#define dr (*tDifferentiate(&r, var))
#define cl (*tCopy(&l))
#define cr (*tCopy(&r))
#define num(A) (*new tExprNode(A))
#define pow(A, B) (*(new tExprNode('^', &A, &B)))
#define func(A, B) (*(new tExprNode(tString(#A), &B)))

	switch (node->type) {
	case constant:
		return new tExprNode();
	case operation:

#define OPERATION(SYMB, CODE, CODE_C) if (node->value == SYMB) {CODE;}
#include "../tExpressionHandler/NodeConstructor"
#undef OPERATION

		break;
	case function:

#define FUNCTION(NAME, CODE, CODE_C) if (node->value == tString(#NAME)) {CODE;}
#include "../tExpressionHandler/NodeConstructor"
#undef FUNCTION

		break;
	case variable:
		return ((var == node->value) ? &num(1) : &num(0));
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

}

#endif
