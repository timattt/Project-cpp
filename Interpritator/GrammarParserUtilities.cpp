#include "GrammarParserUtilities.hpp"

tLangNode::~tLangNode() {
	for (unsigned i = 0; i < size; i++) {
		delete children[i];
	}
}

//! Simple debug function
void tLangNode::dfs_out() {
	__tGoInto(name + tString(" : ") + value);

	for (unsigned i = 0; i < size; i++) {
		children[i]->dfs_out();
	}

	__tGoOut();
}

//! Construct initializes only name of this nonterminal
tLangNode::tLangNode(tString str) :
		name(str), value(), size(0) {
}

// Gives last added child. And throws exception if there are no children.
tLangNode* tLangNode::getLastChild() {
	CHECK (size == 0, "NO CHILDREN YET!!!");

	return children[size - 1];
}

//! Removes and gives last child.
tLangNode* tLangNode::pullLast() {
	size--;
	return children[size];
}

//! Initializes this vertex with two children.
tLangNode::tLangNode(tString str, tLangNode *f, tLangNode *s) :
		name(str), size(2) {
	children[0] = f;
	children[1] = s;
}

//! Adding symbol to value field.
void tLangNode::addSymbol(char s) {
	value += s;
}

//! Adding child and increase size.
void tLangNode::addChild(tLangNode *ch) {
	children[size++] = ch;
}
