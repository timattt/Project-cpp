#ifndef T_GRAM_PAR_UT
#define T_GRAM_PAR_UT

#include "../tUtilities/tString.h"
#include "../tUtilities/tUtilities.h"
#include "../tUtilities/tCallTreeBuilder.h"
#include "bits/stdc++.h"
#include "../tStorage/tTreap.h"

using namespace std;
using namespace tUtilities;
using namespace tStorage;

//! Array inside tLangNode is static so this constant is its size.
const unsigned MAX_CHILDREN = 200;

//! Class that stores expanded grammar
class tLangNode {

public:
	//! Name of the nonterminal
	tString name;
	//! string readed by this nonterminal
	tString value;

	//! Nonterminals that are called by this one.
	tLangNode *children[MAX_CHILDREN];

	//! Quantity of children
	unsigned size = 0;

	//! Simple debug function
	void dfs_out() {
		__tGoInto(name + tString(" : ") + value);

		for (unsigned i = 0; i < size; i++) {
			children[i]->dfs_out();
		}

		__tGoOut();
	}

	//! Construct initializes only name of this nonterminal
	tLangNode(tString str) :
			name(str), value(), size(0) {
	}

	// Gives last added child. And throws exception if there are no children.
	tLangNode* getLastChild() {
		if (size == 0) {
			tThrowException("NO CHILDREN YET!!!");
		}
		return children[size - 1];
	}

	//! Removes and gives last child.
	tLangNode* pullLast() {
		size--;
		return children[size];
	}

	//! Initializes this vertex with two children.
	tLangNode(tString str, tLangNode *f, tLangNode *s) :
			name(str), size(2) {
		children[0] = f;
		children[1] = s;
	}

	//! Adding symbol to value field.
	void addSymbol(char s) {
		value += s;
	}

	//! Adding child and increase size.
	void addChild(tLangNode *ch) {
		children[size++] = ch;
	}

};

#endif
