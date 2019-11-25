#ifndef T_CALL_TREE_BUILDER
#define T_CALL_TREE_BUILDER

#include "tUtilities.h"
#include "tString.h"
#include "../tStorage/tList.h"
#include "tFileHandler.h"
#include <bits/stdc++.h>

using namespace tFileHandler;

namespace tUtilities {

void __tFastStart();
void __tFastEnd();
void __tGoInto(tString lable);

tList<tString> call_hierarchy = { };
tFile *hierarchy_output = NULL;

unsigned freeID = 0;

void __tStartMonitoring(tFile *file) {
	hierarchy_output = file;
	file->tStartMapping(100000);
	file->tWriteLine("digraph graphname {");
}

void __tEndMonitoring() {
	hierarchy_output->tWriteLine("}");
	hierarchy_output->tStopMapping();
	hierarchy_output = NULL;
}

int h = 0;

void __tGoInto(tString lable) {
	if (h++ > 200) {
		tUtilities::__tFastEnd();
		exit(0);
	}
	std::cout << "Going into ";
	lable.out();

	tString name = tString('_') + tString(freeID++);

	tString line = { };

	line += name;
	line += ("[label=");
	line += ('"');
	line += lable;
	line += ('"');
	line += ("]");

	hierarchy_output->tWriteLine(line);

	line = { };

	if (call_hierarchy.tGetSize() > 0) {
		line += call_hierarchy.tGetLastElement();
		line += tString(" -> ");
		line += name;
	}

	call_hierarchy.tAddLast(name);

	hierarchy_output->tWriteLine(line);
}

void __tGoOut(tString ret = { }) {
	std::cout << "Going out ";
	ret.out();
	if (call_hierarchy.tGetSize() == 0) {
		tThrowException("Call hierarchy is empty!");
	}
	//tString from =
	call_hierarchy.tRemoveLast();

	/*
	 if (call_hierarchy.tGetSize() >= 1) {
	 tString to = call_hierarchy.tGetLastElement();

	 tString line = { };

	 line += to;
	 line += tString(" -> ");
	 line += from;
	 if (ret.size() > 0) {
	 line += "[label=";
	 line += '"';
	 line += ret;
	 line += '"';
	 line += ']';
	 }

	 hierarchy_output->tWriteLine(line);
	 }
	 */
}

tFile *fastStartFile = NULL;

void __tFastStart() {
	DeleteFileA("CALL_HIERARCHY_FAST");
	fastStartFile = new tFile("CALL_HIERARCHY_FAST");
	__tStartMonitoring(fastStartFile);
}

void __tFastEnd() {
	DeleteFileA("CALL_HIERARCHY_FAST_IMG");
	__tEndMonitoring();
	const char *nm = fastStartFile->tGetName();
	delete fastStartFile;
	tCreateDotImg(nm, "CALL_HIERARCHY_FAST_IMG");
	DeleteFileA("CALL_HIERARCHY_FAST");
	delete fastStartFile;
}

}

#endif
