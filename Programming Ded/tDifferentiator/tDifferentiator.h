#ifndef T_DIFFERENCIATOR
#define T_DIFFERENCIATOR

#include "../tUtilities/tString.h"
#include "../tStorage/tList.h"

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
#define OPERATION(SYMB, DIFF_CODE) if (val[i] == SYMB) {points[i] = brackets; min_br = tMin(min_br, brackets);}
#include "NodeConstructor"
#undef OPERATION

	}

	tList<unsigned> best_ops = { };
	for (auto it = points.begin(); it != points.end(); it++) {
		if (it->second == min_br) {
			best_ops.tAddLast(it->first);
		}
	}

#define OPERATION(SYMB, DIFF_CODE) for (unsigned i = 0; i < best_ops.tGetSize(); i++) {\
	if (val[best_ops.tGet(i)] == SYMB) {return best_ops.tGet(i);}\
	}
#include "NodeConstructor"
#undef OPERATION

	return val.size;
}

class tDiffNode {
public:
	tDiffNode *left;
	tDiffNode *right;

	tString value;
	double num_value;
	nodeType type;

	tDiffNode() :
			left(NULL), right(NULL), value(tString('0')), num_value(0), type(constant) {
	}

	tDiffNode(double val) :
			left(NULL), right(NULL), value(tString(val)), num_value(val), type(constant) {
	}

	tDiffNode(char type_, tDiffNode *l, tDiffNode *r) :
			left(l), right(r), value(tString(type_)), num_value(0), type(operation) {
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
#define FUNCTION(NAME, DIFF_CODE) \
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
#define VARIABLE(NAME, DIFF_CODE) if (type == none && val == tString(#NAME)) {\
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

	void tOut() const {
		std::cout << "Node type:" << type << " value :[";
		value.tWrite();
		std::cout << "]\n";
		if (left != NULL) {
			left->tOut();
		}
		if (right != NULL) {
			right->tOut();
		}
	}

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
void tCleanup(tDiffNode *&node) {
	if (node->left != NULL) {
		tCleanup(node->left);
	}
	if (node->right != NULL) {
		tCleanup(node->right);
	}
	if (node->type == constant && node->num_value == 0) {
		delete node;
		node = NULL;
		return;
	}
	if (node->type == operation) {
		if (node->left == NULL || node->right == NULL) {
			if (node->value == '*') {
				node = NULL;
				return;
			}
			if (node->value == '+') {
				node = (node->left == NULL ? node->right : node->left);
				return;
			}
		} else {
			if (node->value == '*') {
				if (node->left->num_value == 1) {
					node = node->right;
				}
				if (node->right->num_value == 1) {
					node = node->left;
				}
			}
			if (node->value == '^') {
				if (node->right->num_value == 1) {
					node = node->left;
				}
			}
		}
	}

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

#define OPERATION(SYMB, CODE) if (node->value == SYMB) {CODE;}
#include "NodeConstructor"
#undef OPERATION

		break;
	case function:

#define FUNCTION(NAME, CODE) if (node->value == tString(#NAME)) {CODE;}
#include "NodeConstructor"
#undef FUNCTION

		break;
	case variable:

#define VARIABLE(NAME, CODE) if (node->value == tString(#NAME)) {CODE;}
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
