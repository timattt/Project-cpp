#pragma once

// Includes
//==========================================
#include <stddef.h>
#include <winbase.h>
#include <iostream>
#include <math.h>

#include "../Storage/tList.hpp"
#include "../Utilities/FileHandler.hpp"
#include "../Utilities/String.hpp"
//==========================================


// Structures
//==========================================
enum nodeType {
	none, operation, constant, function, variable
};
struct ExprNode {
	//Fields
	//-----------------------
	// Children
	ExprNode *left;
	ExprNode *right;

	// Value
	tString value;
	double num_value;

	// Type
	nodeType type;

	// Id for dot constructor
	unsigned id;
	//-----------------------

	// Constructors
	//-----------------------
	ExprNode();
	ExprNode(double val);
	ExprNode(char type_, ExprNode *l, ExprNode *r);
	ExprNode(tString name, ExprNode *arg);
	ExprNode(tString val);
	~ExprNode();
	//-----------------------
};
//==========================================


// Global functions
//==========================================
//! This function constructs the line on which this tree was built.
tString exprToTexString(ExprNode *node);
//! This function constructs the line on which this tree was built.
tString exprToString(ExprNode *node);
ExprNode& operator+(ExprNode &a, ExprNode &b);
ExprNode& operator*(ExprNode &a, ExprNode &b);
ExprNode& operator/(ExprNode &a, ExprNode &b);
void exprFindAllVariables(ExprNode *node, tList<tString> *dest);
//! This function copy this vertex and the entire subtree of this vertex
ExprNode* exprCopy(ExprNode *node);
//! This function cleans the equation tree by
//! removing the extra branches that contain either 0 or 1 or other unnecessary data.
//! 1. Delete this node if it is multiplication and any child is NULL. (x*0 -> 0)
//! 2. Replace this node with nonnull child if it is sum and one child is NULL. (x + 0 -> x)
//! 3. If it is multiplication and one child is 1 then it is replaced with other child. (x * 1 -> x)
//! 4. If it is pow and power is 1 then it is replaced with other child. (x^1 -> x)
unsigned exprCleanup(ExprNode *&node);
//! Converts node tree into dot format and convert it to image.
void exprSaveDotImage(tString imgName, ExprNode *node);
//! This function substitutes the value of the
//! variables and calculates the numerical value of the expression that represents this tree.
double exprCalculate(const ExprNode *const node, map<tString, double> vars);
//! This function differentiate given in node tree equation.
//! Differentiation is carried out according to the variable that is stored in the string.
ExprNode* exprDifferentiate(const ExprNode *node, const tString var__);
//==========================================
