#pragma once

// Includes
//===========================================
#include "../Utilities/CallTreeBuilder.hpp"
#include "../Utilities/String.hpp"
#include "../Utilities/Utilities.hpp"
//===========================================


// Constants
//===========================================
const unsigned MAX_CHILDREN = 200; //! Array inside tLangNode is static so this constant is its size.
//===========================================


// Structures
//===========================================
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
	void dfs_out();

	//! Construct initializes only name of this nonterminal
	tLangNode(tString str);

	~tLangNode();

	// Gives last added child. And throws exception if there are no children.
	tLangNode* getLastChild();

	//! Removes and gives last child.
	tLangNode* pullLast();

	//! Initializes this vertex with two children.
	tLangNode(tString str, tLangNode *f, tLangNode *s);

	//! Adding symbol to value field.
	void addSymbol(char s);

	//! Adding child and increase size.
	void addChild(tLangNode *ch);

};
//===========================================
