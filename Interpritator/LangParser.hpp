#pragma once

// Includes
//===========================================
#include <stdlib.h>
#include <winbase.h>
#include <iostream>

#include "../Utilities/CallTreeBuilder.hpp"
#include "../Utilities/FileHandler.hpp"
#include "../Utilities/String.hpp"
#include "../Utilities/Utilities.hpp"
#include "GrammarParserUtilities.hpp"
//===========================================


// Global functions
//===========================================
tLangNode* intrParse(tString text);
//===========================================


// Structures
//===========================================
struct tLangType {
	unsigned size;
	tString name;

	tString (*add)(tString, tString);
	tString (*mul)(tString, tString);
	tString (*neg)(tString);
	tString (*div)(tString, tString);
	tString (*toBytes)(tString);
	tString (*fromBytes)(tString);

	tLangType();

	tLangType(unsigned sz, tString nm, tString (*add_)(tString, tString),
			tString (*mul_)(tString, tString), tString neg_(tString),
			tString (*div_)(tString, tString), tString (*toString_)(tString),
			tString (*fromString_)(tString));

	int operator<(tLangType &other);
};

struct tLangVariable {
	tLangType *type;
	tString value;
	tString name;

	tLangVariable(tLangType *type_, tString name_);
};

struct tLangMethod {
	tString (*func)(tString*);
	tLangNode *start;
	unsigned argsQuant;
	tString name;
	tLangType *return_type;
	tLangVariable **args;

	tLangMethod(tString (*f)(tString*), unsigned args_q, tString name_,
			tLangType *ret);

	tLangMethod(tLangNode *st, unsigned args_q, tString name_, tLangType *ret);
};
//===========================================
