#ifndef T_AKINATOR
#define T_AKINATOR

#include "../TXLIB.h"
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
		"Choose operation: g - guess, d - define, e - end, s - save, l - load, p - paint TX, t - terminate TX window, w - wizzard graph";
char DEF_NAME[] = "Input name";
char FILE_NAME[] = "Input file name";

unsigned STR_LEN = 20;

class statementVertex {

public:
	tString statement;

	statementVertex *no;
	statementVertex *yes;

	statementVertex() :
			statement( { }), no(NULL), yes(NULL) {
	}

	statementVertex(tString st) :
			statement(st), no(NULL), yes(NULL) {
	}

	statementVertex(tString st, char *yes_, char *no_) :
			statement(st), no(new statementVertex(no_)), yes(
					new statementVertex(yes_)) {
	}

	statementVertex(tString st, char *yes_) :
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
	}

};

//! Loads tree from given file.
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

	file->tReadLine();

	for (unsigned i = 0; i < maxVerts; i++) {
		tString line = file->tReadLine();
		verts[i] = new statementVertex(line);
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

//! Gives length of biggest path from root to any vertex.
unsigned biggestPathFromRoot(statementVertex *vert) {
	if (vert == NULL || (vert->isLeaf())) {
		return 0;
	}
	return 1
			+ std::max(biggestPathFromRoot(vert->yes),
					biggestPathFromRoot(vert->no));
}

unsigned totalVertices(statementVertex *vert) {
	if (vert == NULL) {
		return 0;
	}
	return 1 + totalVertices(vert->no) + totalVertices(vert->yes);
}

//! Saves tree in given file.
void save(tFile *dest, statementVertex *vert) {
	unsigned totalVerts = totalVertices(vert);

	dest->tStartMapping(2 * totalVerts * STR_LEN);

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

	dest->tWrite(tString("#####"));

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

//! Manipulating this tree.
void guess(statementVertex *&root) {
	if (root == NULL) {
		cout << NEW_OBJECT << "\n";
		tString obj = { };

		obj.tReadLine();

		statementVertex *vert = new statementVertex(obj);
		root = vert;
		return;
	} else if (root->isLeaf()) {
		cout << GUESS;
		root->statement.tWrite();
		cout << "?\n";
		cout << Y_OR_N << "\n";
		if (tReadCharFromLine() == 'y') {
			cout << WIN << "\n";
		} else {
			cout << NEW_OBJECT << "\n";

			tString obj = { };
			obj.tReadLine();

			cout << DIFF;
			obj.tWrite();
			cout << AND;
			root->statement.tWrite();
			cout << "\n";

			tString dif;
			dif.tReadLine();

			statementVertex *vert = new statementVertex(dif);
			statementVertex *yes = new statementVertex(obj);
			statementVertex *no = new statementVertex(root->statement);

			delete root;

			vert->yes = yes;
			vert->no = no;
			root = vert;
		}
	} else {
		cout << QUESTION;
		root->statement.tWrite();
		cout << "\n";
		cout << Y_OR_N << "\n";
		if (tReadCharFromLine() == 'y') {
			guess(root->yes);
		} else {
			guess(root->no);
		}
	}
}

//! Finds node with given statement.
statementVertex* find(statementVertex *&root, tString name) {
	if (root != NULL) {
		if (root->isLeaf()) {
			return (!(root->statement == name) ? root : NULL);
		} else {
			statementVertex *x = find(root->no, name);
			statementVertex *y = find(root->yes, name);

			return (x != NULL ? x : y);
		}
	}
	return NULL;
}

//! Gives properties of given object.
void definition(tString name, statementVertex *&root,
		tList<const tPair<tString, bool>> *stacktrc = NULL) {
	bool created = 0;
	if (stacktrc == NULL) {
		stacktrc = new tList<const tPair<tString, bool>>();
		created = 1;
	}
	if (root != NULL) {
		if (root->isLeaf()) {
			if (!(root->statement == name)) {
				name.tWrite();
				cout << DEFIN << "\n";
				stacktrc->tForEach([](const tPair<tString, bool> &elem) {
					if (!elem.y) {
						cout << DEFIN_NO;
					}
					elem.x.tWrite();
					cout << "\n";
				});
			}
		} else {
			stacktrc->tAddLast( { root->statement, 1 });
			definition(name, root->yes, stacktrc);
			stacktrc->tRemoveLast();

			stacktrc->tAddLast( { root->statement, 0 });
			definition(name, root->no, stacktrc);
			stacktrc->tRemoveLast();
		}
	}
	if (created) {
		delete stacktrc;
	}
}

const unsigned WIDTH = 1300;
const unsigned HEIGHT = 1000;

const unsigned SIDE = 35;

void drawLines(statementVertex *vert, unsigned x = WIDTH / 2 - SIDE / 2,
		unsigned y = SIDE, unsigned px = WIDTH / 2, unsigned py = SIDE,
		unsigned step = 9999) {
	if (vert == NULL) {
		return;
	}
	if (step == 9999) {
		step = (HEIGHT - SIDE) / (biggestPathFromRoot(vert) + 1);
	}
	txSetColor(RGB(0, 0, 127));
	txLine(SIDE / 2 + x, SIDE / 2 + y, SIDE / 2 + px, SIDE / 2 + py);

	unsigned w = (x % (WIDTH / 2)) / 2;
	if (vert->no != NULL) {
		drawLines(vert->no, x - w, y + step, x, y, step);
	}
	if (vert->yes != NULL) {
		drawLines(vert->yes, x + w, y + step, x, y, step);
	}
}

void drawVertex(statementVertex *vert, unsigned x = WIDTH / 2 - SIDE / 2,
		unsigned y = SIDE, unsigned px = WIDTH / 2, unsigned py = SIDE,
		unsigned step = 9999) {
	if (vert == NULL) {
		return;
	}
	if (step == 9999) {
		step = (HEIGHT - SIDE) / (biggestPathFromRoot(vert) + 1);
	}
	char col = (vert->isLeaf() ? 127 : 0);
	txSetFillColor(RGB(col, 127, 0));
	txSetColor(RGB(0, 127, 127));

	txRectangle(SIDE / 2 + x - SIDE / 2, SIDE / 2 + y - SIDE / 2,
			SIDE / 2 + x + SIDE / 2, SIDE / 2 + y + SIDE / 2);
	txSetColor(RGB(127, 0, 0));

	char tmp[200];

	tFill<char>(tmp, 0, 200);
	tCopyBuffers(vert->statement.ggggg, tmp, vert->statement.size());

	txDrawText(SIDE / 2 + x - SIDE / 2, SIDE / 2 + y - SIDE / 2,
			SIDE / 2 + x + SIDE / 2, SIDE / 2 + y + SIDE / 2, tmp);

	unsigned w = (x % (WIDTH / 2)) / 2;
	if (vert->no != NULL) {
		drawVertex(vert->no, x - w, y + step, x, y, step);
	}
	if (vert->yes != NULL) {
		drawVertex(vert->yes, x + w, y + step, x, y, step);
	}
}

void makeDotFile(statementVertex *root, tFile *dest) {
	dest->tStartMapping(2 * totalVertices(root) * STR_LEN);

	dest->tWriteLine("digraph AKINATOR {");

	tList<statementVertex*> queue;

	queue.tAddLast(root);
	while (queue.tGetSize() != 0) {
		statementVertex *curr = queue.tGet(0);
		queue.tRemove(0);

		if (curr == NULL) {
			continue;
		}

		dest->tWriteLine(curr->statement);

		queue.tAddLast(curr->yes);
		queue.tAddLast(curr->no);

	}

	queue.tAddLast(root);
	while (queue.tGetSize() != 0) {
		statementVertex *curr = queue.tGet(0);
		queue.tRemove(0);

		if (curr == NULL) {
			continue;
		}

		if (curr->no != NULL) {
			dest->tWrite(curr->statement);
			dest->tWrite("->");
			dest->tWrite(curr->no->statement);
			dest->tWriteLine(";");
		}
		if (curr->yes != NULL) {
			dest->tWrite(curr->statement);
			dest->tWrite("->");
			dest->tWriteLine(curr->yes->statement);
			dest->tWriteLine(";");
		}

		queue.tAddLast(curr->yes);
		queue.tAddLast(curr->no);

	}

	dest->tWriteLine("}");

	dest->tStopMapping();
}

bool txCreated = 0;

void draw(statementVertex *root) {
	if (!txCreated) {
		txCreateWindow(WIDTH, HEIGHT, 1);
		txCreated = 1;
	}
	txSetColor(RGB(127, 127, 127));
	txSelectFont("Times new roman", SIDE / 5 * 2);
	drawLines(root);
	drawVertex(root);
}

void runAkinator() {
	__tNewBranch();

	statementVertex *root = NULL;

	char op = 0;
	while (1) {
		cout << WHAT_TO_DO << "\n";

		op = tReadCharFromLine();
		if (op == 'e') {
			break;
		}
		if (op == 'g') {
			guess(root);
		}
		if (op == 'd') {
			cout << DEF_NAME << "\n";
			tString name = { };
			name.tReadLine();
			definition(name, root);
		}
		if (op == 's') {
			cout << FILE_NAME << "\n";
			tString name = { };
			name.tReadLine();
			tFile *out = new tFile(name);
			save(out, root);
			delete out;
		}
		if (op == 'l') {
			cout << FILE_NAME << "\n";
			tString name = { };
			name.tReadLine();
			tFile *in = new tFile(name);
			root = load(in);
			delete in;
		}
		if (op == 'p') {
			draw(root);
		}
		if (op == 't') {
			txDestroyWindow();
		}
		if (op == 'w') {
			cout << FILE_NAME << "\n";
			tString name = { };
			name.tReadLine();
			tFile *in = new tFile(name);
			makeDotFile(root, in);
			delete in;
		}
	}

	__tFlushBranch();
}

#endif
