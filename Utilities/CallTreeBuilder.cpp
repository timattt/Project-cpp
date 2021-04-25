#include "CallTreeBuilder.hpp"

// first - id, second - name
tList<tPair<tString, tString>> call_hierarchy = { };
tFile *hierarchy_output = NULL;

unsigned freeID = 0;

void __tStartMonitoring(tFile *file) {
	hierarchy_output = file;
	file->tStartMapping(10000000);
	file->tWriteLine("digraph graphname {");
}

void __tEndMonitoring() {
	hierarchy_output->tWriteLine("}");
	hierarchy_output->tStopMapping();
	hierarchy_output = NULL;
}

void __tGoInto(tString lable, unsigned color) {
	tString name = tString('_') + tString(freeID++);

	tString line = { };

	line += name;
	line += ("[label=");
	line += ('"');
	line += lable;
	line += ('"');
	line += ("]");

	if (hierarchy_output == NULL) {
		printf("No monitoring is started!\n");
		exit(0);
	}

	hierarchy_output->tWriteLine(line);

	line = { };

	if (call_hierarchy.tGetSize() > 0) {
		line += call_hierarchy.tGetLastElement().x;
		line += tString(" -> ");
		line += name;

		switch (color) {
		case 1:
			line += "[color=red]";
			break;
		case 2:
			line += "[color=blue]";
			break;
		case 3:
			line += "[color=green]";
			break;
		default:
			break;
		}

	}

	call_hierarchy.tAddLast({name, lable});

	hierarchy_output->tWriteLine(line);
}

void __tGoOut(tString ret) {
	CHECK(call_hierarchy.tGetSize() == 0,"Call hierarchy is empty!");

	//tString from =
	call_hierarchy.tRemoveLast().x;

/*
	if (call_hierarchy.tGetSize() >= 1) {
		tString to = call_hierarchy.tGetLastElement().x;

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

void __tFastStart(tString name) {
	DeleteFileA((name).tToPlainArray());
	fastStartFile = new tFile(name);
	__tStartMonitoring(fastStartFile);
}

void __tFastEnd() {
	const char *nm = fastStartFile->tGetName();
	DeleteFileA(nm);
	__tEndMonitoring();
	delete fastStartFile;
	tCreateDotImg(nm, tString(nm) + tString("_IMG"));
	DeleteFileA(nm);
	delete fastStartFile;
}


