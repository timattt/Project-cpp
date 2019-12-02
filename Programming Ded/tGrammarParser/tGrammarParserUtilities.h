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

const unsigned MAX_CHILDREN = 200;

class tLangNode {

public:
	tString name;
	tString value;

	tLangNode *children[MAX_CHILDREN];

	unsigned size = 0;

	void dfs_out() {
		__tGoInto(name + tString(" : ") + value);
		for (unsigned i = 0; i < size; i++) {
			children[i]->dfs_out();
		}
		__tGoOut();
	}

	tLangNode(tString str) :
			name(str), value(), size(0) {
	}

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

	tLangNode(tString str, tLangNode *f, tLangNode *s) :
			name(str), size(2) {
		children[0] = f;
		children[1] = s;
	}

	void addSymbol(char s) {
		value += s;
	}

	void addChild(tLangNode *ch) {
		children[size++] = ch;
	}

};

#endif
