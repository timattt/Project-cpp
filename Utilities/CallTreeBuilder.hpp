#pragma once

// Includes
//======================================================
#include <stddef.h>
#include <winbase.h>

#include "../Storage/tList.hpp"
#include "FileHandler.hpp"
#include "String.hpp"
#include "Utilities.hpp"
//======================================================


// Global variables
//======================================================
extern tList<tPair<tString, tString>> call_hierarchy; //! first - id, second - name
//======================================================


// Global functions
//======================================================
void __tStartMonitoring(tFile *file);
void __tEndMonitoring();
void __tGoInto(tString lable, unsigned color = 0);
void __tGoOut(tString ret = {});
void __tFastStart(tString name);
void __tFastEnd();
//======================================================
