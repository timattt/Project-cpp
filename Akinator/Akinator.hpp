#pragma once

// Includes
//===========================================
#include <stddef.h>
#include <stdlib.h>
#include <algorithm>
#include <iostream>
#include <map>
#include <tuple>
#include <utility>

#include "../Storage/tList.hpp"
#include "../Utilities/FileHandler.hpp"
#include "../Utilities/String.hpp"
#include "../Utilities/Utilities.hpp"
//===========================================


// Structures
//===========================================
struct statementVertex {
	// Fields
	//---------------------
	tString statement;

	statementVertex *no;
	statementVertex *yes;
	//---------------------

	// Functions
	//---------------------
	// Constructors and destructor
	statementVertex();
	statementVertex(tString st);
	statementVertex(tString st, char *yes_, char *no_);
	statementVertex(tString st, char *yes_);
	~statementVertex();

	// Other
	bool isLeaf();
	//---------------------
};
//===========================================


// Functions
//===========================================
statementVertex* 	akLoadFromFile				(tFile *file);//! Loads tree from given file.
unsigned 			akFindBiggestPathFromRoot	(statementVertex *vert);//! Gives length of biggest path from root to any vertex.
unsigned 			akTreeVerticesCount			(statementVertex *vert);//! Gives size of the vertex
void 				akSaveToFile				(tFile *dest, statementVertex *vert);//! Saves tree in given file.
void 				akGuess						(statementVertex *&root);//! Manipulating this tree.
statementVertex* 	akFindNode					(statementVertex *&root, tString name);//! Finds node with given statement.
void 				akGetObjectDefinition		(tString name, statementVertex *&root, tList<const tPair<tString, bool>> *stacktrc = NULL);//! Gives properties of given object.
void 				akCreateDotFile				(statementVertex *root, tFile *dest);//! Creates dot file using given tree
//===========================================

