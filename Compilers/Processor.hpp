#pragma once

// Includes
//===========================================
#include <iostream>
#include <list>
#include <map>
#include <string>
#include <utility>
#include <stdlib.h>
#include <typeinfo>
#include <math.h>

#include "../PE_Editor/PE_Editor.hpp"
#include "../Storage/Stack.hpp"
#include "../Utilities/FileHandler.hpp"
#include "../Utilities/String.hpp"
#include "../Utilities/Utilities.hpp"
//===========================================


// Constants
//===========================================
// TYPE
typedef int PROCESSOR_TYPE;

// SIZES
const unsigned STACK_SIZE = 10000;
const unsigned RAM_SIZE = 10000;
const unsigned REGS_SIZE = 5; //ax, bx, cx, dx, bp
const unsigned MAX_ARGS = 10;
//===========================================


// Global functions
//===========================================
void comDisasm_TEXE_to_TASM			(tFile *exe,	 	tFile *&src	);
void comCompile_TASM_to_TEXE		(tFile *source	, 	tFile *&exe	);
void comCompile_TASM_to_EXE			(tFile *source	, 	tFile *exe_	);
void comTranslateFast_TEXE_to_EXE	(tFile *texe	,	tFile *exe	);
void comTranslateSlow_TEXE_to_EXE	(tFile *texe	,	tFile *exe	);
void comRun_TEXE					(tFile *texe					);
void comRun_EXE						(tFile *exe						);
void comRun							(int argc		, 	char *argv[]);
//===========================================
