#ifndef T_DIFFERENCIATOR
#define T_DIFFERENCIATOR

#include <map>

#include "../tUtilities/tFileHandler.h"
#include "../tUtilities/tString.h"

using namespace tFileHandler;

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
#include "NodeConstructor"
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
#include "NodeConstructor"
#undef OPERATION

	return val.size;
}

unsigned FREE_ID = 0;

class tDiffNode {
public:
	// Children
	tDiffNode *left;
	tDiffNode *right;

	// Value
	tString value;
	double num_value;

	// Type
	nodeType type;

	// Id for dot constructor
	unsigned id = FREE_ID++;

	tDiffNode() :
			left(NULL), right(NULL), value(tString('0')), num_value(0), type(
					constant) {
	}

	tDiffNode(double val) :
			left(NULL), right(NULL), value(tString(val)), num_value(val), type(
					constant) {
	}

	tDiffNode(char type_, tDiffNode *l, tDiffNode *r) :
			left(l), right(r), value(tString(type_)), num_value(0), type(
					operation) {
	}

	tDiffNode(tString name, tDiffNode *arg) :
			left(NULL), right(arg), value(name), num_value(0), type(function) {
	}

	tDiffNode(tString val) :
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
#include "NodeConstructor"
#undef FUNCTION

		if (type != none) {
			right = new tDiffNode(arg);
			return;
		}

		// OPERATION
		unsigned pos = chouseOperation(val);
		if (pos != val.size) {
			value = { val[pos] };
			type = operation;
			tString *divs = val.tDivide(pos);
			left = new tDiffNode(divs[0]);
			right = new tDiffNode(divs[1]);
			return;
		}

		// VARIABLE
#define VARIABLE(NAME, DIFF_CODE, CODE_CALC) if (type == none && val == tString(#NAME)) {\
	type = variable;\
	value = {#NAME};\
}
#include "NodeConstructor"
#undef VARIABLE

		if (type != none) {
			return;
		}

		// CONSTANT
		value = val;
		num_value = val.tToFloat();
		type = constant;
	}

	~tDiffNode() {
		if (left != NULL) {
			delete left;
		}
		if (right != NULL) {
			delete right;
		}
	}

	bool isOperation(char c) const {
		return (type == operation) && (value == c);
	}

	bool isFunction() const {
		return (type == function);
	}

	bool isConstant() const {
		return (type == constant);
	}

	bool isZero() const {
		return (type == constant) && (num_value == 0);
	}

	bool isOne() const {
		return (type == constant) && (num_value == 1);
	}

	//! This function copy this vertex and the entire subtree of this vertex
	tDiffNode* tCopy() {
		tDiffNode *result = new tDiffNode();
		result->type = type;
		result->value = value;
		result->num_value = num_value;
		if (left != NULL) {
			result->left = this->left->tCopy();
		}
		if (right != NULL) {
			result->right = this->right->tCopy();
		}
		return result;
	}

	//! This function substitutes the value of the
	//! variables and calculates the numerical value of the expression that represents this tree.
	double tCalc(map<tString, double> vars) {
		double l = 0;
		double r = 0;
		if (left != NULL) {
			l = left->tCalc(vars);
		}
		if (right != NULL) {
			r = right->tCalc(vars);
		}

		double res = 0;

#define OPERATION(SYMB, CODE_D, CODE_C) if (value == SYMB) {CODE_C;}
#include "NodeConstructor"
#undef OPERATION

#define FUNCTION(NAME, CODE_D, CODE_C) if (value == tString(#NAME)) {CODE_C;}
#include "NodeConstructor"
#undef FUNCTION

#define VARIABLE(NAME, CODE_D, CODE_C) if (value == tString(#NAME)) {CODE_C;}
#include "NodeConstructor"
#undef VARIABLE

		if (type == constant) {
			res = num_value;
		}

		return res;
	}

	//! This function constructs the line on which this tree was built.
	tString tToTexString() const {
		tString result = { };
		if (type == operation) {
			if (value == '/') {
				result = { "\\frac{" };
			} else {
				result = { '(' };
			}
		}
		if (left != NULL) {
			result = result + left->tToTexString();
		}
		if (value == '/') {
			result = result + tString("}{");
		} else {
			if (type == constant) {
				result = result + tString((int) (num_value));
			} else {
				result = result + tString(value);
			}
		}
		if (right != NULL) {
			if (type == function) {
				result = result + tString('(');
			}
			if (isOperation('^')) {
				result = result + tString('{');
			}
			result = result + right->tToTexString();
			if (isOperation('^')) {
				result = result + tString('}');
			}
			if (type == function) {
				result = result + tString(')');
			}
		}
		if (type == operation) {
			if (value == '/') {
				result = result + tString("}");
			} else {
				result = result + tString(')');
			}
		}
		return result;
	}

	//! This function constructs the line on which this tree was built.
	tString tToString() const {
		tString result = { };
		if (type == operation) {
			result = { '(' };
		}
		if (left != NULL) {
			result = result + left->tToString();
		}
		result = result + tString(value);
		if (right != NULL) {
			if (type == function) {
				result = result + tString('(');
			}
			result = result + right->tToString();
			if (type == function) {
				result = result + tString(')');
			}
		}
		if (type == operation) {
			result = result + tString(')');
		}
		return result;
	}
};

tDiffNode& operator+(tDiffNode &a, tDiffNode &b) {
	return *new tDiffNode('+', &a, &b);
}

tDiffNode& operator*(tDiffNode &a, tDiffNode &b) {
	return *new tDiffNode('*', &a, &b);
}

tDiffNode& operator/(tDiffNode &a, tDiffNode &b) {
	return *new tDiffNode('/', &a, &b);
}

//! This function cleans the equation tree by
//! removing the extra branches that contain either 0 or 1 or other unnecessary data.
//! 1. Delete this node if it is multiplication and any child is NULL. (x*0 -> 0)
//! 2. Replace this node with nonnull child if it is sum and one child is NULL. (x + 0 -> x)
//! 3. If it is multiplication and one child is 1 then it is replaced with other child. (x * 1 -> x)
//! 4. If it is pow and power is 1 then it is replaced with other child. (x^1 -> x)
unsigned tCleanup(tDiffNode *&node) {
	tDiffNode *todelete = NULL;

	int total = 0;

#define COL_GARB todelete = node; total++;

	// 1.
	if (node->isOperation('*')) {
		if (node->left != NULL && node->left->isZero()) {
			COL_GARB
			node = new tDiffNode();
		}
		if (node->right != NULL && node->right->isZero()) {
			COL_GARB
			node = new tDiffNode();
		}
	}

	// 2.
	if (node->isOperation('+')) {
		if (node->left != NULL && node->left->isZero()) {
			COL_GARB
			node = node->right->tCopy();
		}
		if (node->right != NULL && node->right->isZero()) {
			COL_GARB
			node = node->left->tCopy();
		}
	}

	// 3.
	if (node->isOperation('*')) {
		if (node->left != NULL && node->left->isOne()) {
			COL_GARB
			node = node->right->tCopy();
		}
		if (node->right != NULL && node->right->isOne()) {
			COL_GARB
			node = node->left->tCopy();
		}
	}

	// 4.
	if (node->isOperation('^')) {
		if (node->left != NULL && node->left->isOne()) {
			COL_GARB
			node = node->right->tCopy();
		}
		if (node->right != NULL && node->right->isOne()) {
			COL_GARB
			node = node->left->tCopy();
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
void tConvertToDot(tDiffNode *node, tFile *dest, bool init = 1) {
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
void tSaveDotImage(tString imgName, tDiffNode *node) {
	tString fileName = { "TEMP_DOT_SAVE" };
	DeleteFileA("TEMP_DOT_SAVE");
	tFile *fl = new tFile(fileName);
	tConvertToDot(node, fl);
	delete fl;
	DeleteFileA("TEMP_DOT_SAVE");
	tCreateDotImg(fileName, imgName);
}

//! This function differentiate given in node tree equation.
//! Differentiation is carried out according to the variables that are stored in the string.
tDiffNode* tDifferentiate(tDiffNode *node, tString vars) {

	// These all is to make access from Node constructor easier
#define l (*node->left)
#define r (*node->right)
#define dl (*tDifferentiate(&l, vars))
#define dr (*tDifferentiate(&r, vars))
#define cl (*l.tCopy())
#define cr (*r.tCopy())
#define num(A) (*new tDiffNode(A))
#define pow(A, B) (*(new tDiffNode('^', &A, &B)))
#define func(A, B) (*(new tDiffNode(tString(#A), &B)))

	switch (node->type) {
	case constant:
		return new tDiffNode();
	case operation:

#define OPERATION(SYMB, CODE, CODE_C) if (node->value == SYMB) {CODE;}
#include "NodeConstructor"
#undef OPERATION

		break;
	case function:

#define FUNCTION(NAME, CODE, CODE_C) if (node->value == tString(#NAME)) {CODE;}
#include "NodeConstructor"
#undef FUNCTION

		break;
	case variable:

#define VARIABLE(NAME, CODE, CODE_C) if (node->value == tString(#NAME)) {CODE;}
#include "NodeConstructor"
#undef VARIABLE

		break;
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

#endif
