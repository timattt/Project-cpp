#pragma once

// Includes
//===========================================
#include <stddef.h>
#include <map>

#include "../Storage/tList.hpp"
#include "../Utilities/CallTreeBuilder.hpp"
#include "../Utilities/String.hpp"
#include "GrammarParserUtilities.hpp"
#include "LangParser.hpp"
//===========================================

// Global functions
//===========================================
void intrInit(tLangNode *rt);
tString intrRunProgram(tLangNode *cur = NULL);
//===========================================
