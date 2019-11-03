#ifndef T_AKINATOR
#define T_AKINATOR

#include <bits/stdc++.h>
#include "../tUtilities/tUtilities.h"
#include "../tStorage/tList.h"
#include "../tUtilities/tFileHandler.h"

using namespace tFileHandler;
using namespace tStorage;
using namespace tUtilities;

using std::cout;
using std::cin;
using std::tuple;
using std::map;

char Y_OR_N[] = "y|n";
char QUESTION[] = "Is your object ";
char NEW_OBJECT[] = "I do not know what you guessed, name your object";
char GUESS[] = "Your object is ";
char WIN[] = "I win!";
char DIFF[] = "What is the difference between ";
char AND[] = " and ";
char DEFIN[] = " is: ";
char DEFIN_NO[] = "Not ";
char WHAT_TO_DO[] =
		"Choose operation: g - guess, d - define, e - end, s - save, l - load";
char DEF_NAME[] = "Input name";
char FILE_NAME[] = "Input file name";

unsigned STR_LEN = 20;

class statementVertex {

public:
	char *statement;

	statementVertex *no;
	statementVertex *yes;

	statementVertex() :
			statement(NULL), no(NULL), yes(NULL) {
	}

	statementVertex(char *st) :
			statement(st), no(NULL), yes(NULL) {
	}

	statementVertex(char *st, char *yes_, char *no_) :
			statement(st), no(new statementVertex(no_)), yes(
					new statementVertex(yes_)) {
	}

	statementVertex(char *st, char *yes_) :
			statement(st), no(NULL), yes(new statementVertex(yes_)) {
	}

	bool isLeaf() {
		return no == NULL || yes == NULL;
	}

	~statementVertex() {
		if (no != NULL) {
			delete no;
		}
		if (yes != NULL) {
			delete yes;
		}
		if (statement != NULL) {
			delete statement;
		}
	}

};
#define DEB cout << "debug\n";cout.flush();
statementVertex* load(tFile *file) {
	statementVertex *result = NULL;

	if (tGetFileSize(file->tGetName()) < 4) {
		return NULL;
	}

	file->tStartMapping();

	unsigned maxVerts = tConvertBytes<unsigned>(file->tGetBuffer());

	cout << "Loaded " << maxVerts << " vertices\n";

	statementVertex **verts = (statementVertex**) calloc(maxVerts,
			sizeof(statementVertex*));

	unsigned len = 0;
	file->tReadLine(len);

	for (unsigned i = 0; i < maxVerts; i++) {
		char *line = file->tReadLine(len);
		char *copy = new char[len + 1];
		copy[len] = '\0';
		tCopyBuffers(line, copy, len);
		verts[i] = new statementVertex(copy);
	}

	result = verts[0];

	while (*file->tGetCurrentPointer() == '#') {
		file->tMovePointer(1);
	}

	while (1) {
		int root = 0;
		int no = 0;
		int yes = 0;
		root = tConvertBytes<int>(file->tGetCurrentPointer());
		file->tMovePointer(4);

		if (root == -100) {
			break;
		}

		no = tConvertBytes<int>(file->tGetCurrentPointer());
		file->tMovePointer(4);
		yes = tConvertBytes<int>(file->tGetCurrentPointer());
		file->tMovePointer(4);

		if (no != -1) {
			verts[root]->no = verts[no];
		}
		if (yes != -1) {
			verts[root]->yes = verts[yes];
		}
	}

	file->tStopMapping();

	return result;
}

void info(statementVertex *vert) {
	if (vert != NULL) {
		cout << "Vertex: " << vert->statement << "\n";
		info(vert->yes);
		info(vert->no);
	}
}

unsigned biggestPathFromRoot(statementVertex *vert) {
	if (vert == NULL || (vert->yes == NULL && vert->no == NULL)) {
		return 0;
	}
	return 1
			+ std::max(biggestPathFromRoot(vert->yes),
					biggestPathFromRoot(vert->no));
}

void save(tFile *dest, statementVertex *vert) {
	unsigned totalLines = tBinpow<unsigned>(2, biggestPathFromRoot(vert) + 2)
			- 1;

	dest->tStartMapping(2 * totalLines * STR_LEN);

	tWriteBytes<unsigned>(0, dest->tGetBuffer());
	dest->tMovePointer(4);

	dest->tWritec('\n');

	tList<statementVertex*> queue;

	map<statementVertex*, unsigned> proj;

	unsigned i = 0;
	queue.tAddLast(vert);
	while (queue.tGetSize() != 0) {
		statementVertex *curr = queue.tGet(0);
		queue.tRemove(0);

		if (curr == NULL) {
			continue;
		}

		proj.insert( { curr, i });

		dest->tWriteLine(curr->statement);

		queue.tAddLast(curr->yes);
		queue.tAddLast(curr->no);

		i++;
	}

	tWriteBytes(i, dest->tGetBuffer());

	dest->tWrite("#####");

	queue.tAddLast(vert);
	while (queue.tGetSize() != 0) {
		statementVertex *curr = queue.tGet(0);
		queue.tRemove(0);

		if (curr == NULL) {
			continue;
		}
		tWriteBytes(proj[curr], dest->tGetCurrentPointer());
		dest->tMovePointer(4);
		if (curr->no == NULL) {
			tWriteBytes(-1, dest->tGetCurrentPointer());
			dest->tMovePointer(4);
		} else {
			tWriteBytes(proj[curr->no], dest->tGetCurrentPointer());
			dest->tMovePointer(4);
		}
		if (curr->yes == NULL) {
			tWriteBytes(-1, dest->tGetCurrentPointer());
			dest->tMovePointer(4);
		} else {
			tWriteBytes(proj[curr->yes], dest->tGetCurrentPointer());
			dest->tMovePointer(4);

		}

		queue.tAddLast(curr->yes);
		queue.tAddLast(curr->no);
	}

	tWriteBytes(-100, dest->tGetCurrentPointer());

	dest->tStopMapping();
}

void guess(statementVertex *&root) {
	if (root == NULL) {
		cout << NEW_OBJECT << "\n";
		char *obj = new char[STR_LEN];

		tReadLine(obj, STR_LEN);

		statementVertex *vert = new statementVertex(obj);
		root = vert;
		return;
	} else if (root->isLeaf()) {
		cout << GUESS << root->statement << "?\n";
		cout << Y_OR_N << "\n";
		if (tReadCharFromLine() == 'y') {
			cout << WIN << "\n";
		} else {
			cout << NEW_OBJECT << "\n";

			char *obj = new char[STR_LEN];
			tReadLine(obj, STR_LEN);

			cout << DIFF << obj << AND << root->statement << "\n";

			char *dif = new char[STR_LEN];
			tReadLine(dif, STR_LEN);

			statementVertex *vert = new statementVertex(dif);
			statementVertex *yes = new statementVertex(obj);
			char *copy = new char[tStrlen(root->statement) + 1];
			copy[tStrlen(root->statement)] = '\0';
			tCopyBuffers(root->statement, copy, tStrlen(root->statement));
			statementVertex *no = new statementVertex(copy);

			//delete root;

			vert->yes = yes;
			vert->no = no;
			root = vert;
		}
	} else {
		cout << QUESTION << root->statement << "\n";
		cout << Y_OR_N << "\n";
		if (tReadCharFromLine() == 'y') {
			guess(root->yes);
		} else {
			guess(root->no);
		}
	}
}

statementVertex* find(statementVertex *&root, char *name) {
	if (root != NULL) {
		if (root->isLeaf()) {
			return (tStrcmp(root->statement, name) == 0 ? root : NULL);
		} else {
			statementVertex *x = find(root->no, name);
			statementVertex *y = find(root->yes, name);

			return (x != NULL ? x : y);
		}
	}
	return NULL;
}

void definition(char *name, unsigned len, statementVertex *&root,
		tList<const tPair<char*, bool>> stacktrc = { }) {
	if (root != NULL) {
		if (root->isLeaf()) {
			if (tStrcmp(root->statement, name) == 0) {
				cout << name << DEFIN << "\n";
				stacktrc.tForEach([](const tPair<char*, bool> &elem) {
					if (!elem.y) {
						cout << DEFIN_NO;
					}
					if (elem.x != NULL)
						cout << elem.x << "\n";
				});
			}
		} else {
			stacktrc.tAddLast( { root->statement, 1 });
			definition(name, len, root->yes, stacktrc);
			stacktrc.tRemoveLast();

			stacktrc.tAddLast( { root->statement, 0 });
			definition(name, len, root->no, stacktrc);
			stacktrc.tRemoveLast();
		}
	}
}

void runAkinator() {
	statementVertex *root = NULL;

	char op = 0;
	while (op != 'e') {
		cout << WHAT_TO_DO << "\n";

		op = tReadCharFromLine();
		if (op == 'g') {
			guess(root);
		}
		if (op == 'd') {
			cout << DEF_NAME << "\n";
			char *name = new char[STR_LEN];
			tReadLine(name, STR_LEN);
			definition(name, tStrlen(name), root);
			delete name;
		}
		if (op == 's') {
			cout << FILE_NAME << "\n";
			char *name = new char[STR_LEN];
			tReadLine(name, STR_LEN);
			tFile *out = new tFile(name);
			save(out, root);
			delete out;
			delete name;
		}
		if (op == 'l') {
			cout << FILE_NAME << "\n";
			char *name = new char[STR_LEN];
			tReadLine(name, STR_LEN);
			tFile *in = new tFile(name);
			root = load(in);
			delete in;
			delete name;
		}
	}
}

#endif
