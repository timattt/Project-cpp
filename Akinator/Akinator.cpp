#include "../Akinator/Akinator.hpp"

// Constants
//===========================================
const char Y_OR_N[] = "y|n";
const char QUESTION[] = "Is your object ";
const char NEW_OBJECT[] = "I do not know what you guessed, name your object";
const char GUESS[] = "Your object is ";
const char WIN[] = "I win!";
const char DIFF[] = "What is the difference between ";
const char AND[] = " and ";
const char DEFIN[] = " is: ";
const char DEFIN_NO[] = "Not ";
const char WHAT_TO_DO[] = "Choose operation: g - guess, d - define, e - end, s - save, l - load, w - wizzard graph";
const char DEF_NAME[] = "Input name";
const char FILE_NAME[] = "Input file name";

const unsigned STR_LEN = 200;
//===========================================


// Global functions
//===========================================
statementVertex::statementVertex() :
		statement(), no(NULL), yes(NULL) {
}

statementVertex::statementVertex(tString st) :
		statement(st), no(NULL), yes(NULL) {
}

statementVertex::statementVertex(tString st, char *yes_, char *no_) :
		statement(st), no(new statementVertex(no_)), yes(
				new statementVertex(yes_)) {
}

statementVertex::statementVertex(tString st, char *yes_) :
		statement(st), no(NULL), yes(new statementVertex(yes_)) {
}

bool statementVertex::isLeaf() {
	return no == NULL || yes == NULL;
}

statementVertex::~statementVertex() {
	if (no != NULL) {
		delete no;
	}
	if (yes != NULL) {
		delete yes;
	}
}


//! Loads tree from given file.
statementVertex* akLoadFromFile(tFile *file) {
	statementVertex *result = NULL;

	if (tGetFileSize(file->tGetName()) < 4) {
		return NULL;
	}

	file->tStartMapping();

	unsigned maxVerts = tConvertBytes<unsigned>(file->tGetBuffer());

	std::cout << "Loaded " << maxVerts << " vertices\n";

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
unsigned akFindBiggestPathFromRoot(statementVertex *vert) {
	if (vert == NULL || (vert->isLeaf())) {
		return 0;
	}
	return 1
			+ std::max(akFindBiggestPathFromRoot(vert->yes),
					akFindBiggestPathFromRoot(vert->no));
}

unsigned akTreeVerticesCount(statementVertex *vert) {
	if (vert == NULL) {
		return 0;
	}
	return 1 + akTreeVerticesCount(vert->no) + akTreeVerticesCount(vert->yes);
}

//! Saves tree in given file.
void akSaveToFile(tFile *dest, statementVertex *vert) {
	unsigned totalVerts = akTreeVerticesCount(vert);

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
void akGuess(statementVertex *&root) {
	if (root == NULL) {
		std::cout << NEW_OBJECT << "\n";
		tString obj = { };

		obj.tReadLine();

		statementVertex *vert = new statementVertex(obj);
		root = vert;
		return;
	} else if (root->isLeaf()) {
		std::cout << GUESS;
		root->statement.tWrite();
		std::cout << "?\n";
		std::cout << Y_OR_N << "\n";
		if (tReadCharFromLine() == 'y') {
			std::cout << WIN << "\n";
		} else {
			std::cout << NEW_OBJECT << "\n";

			tString obj = { };
			obj.tReadLine();

			std::cout << DIFF;
			obj.tWrite();
			std::cout << AND;
			root->statement.tWrite();
			std::cout << "?\n";

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
		std::cout << QUESTION;
		root->statement.tWrite();
		std::cout << "\n";
		std::cout << Y_OR_N << "\n";
		if (tReadCharFromLine() == 'y') {
			akGuess(root->yes);
		} else {
			akGuess(root->no);
		}
	}
}

//! Finds node with given statement.
statementVertex* akFindNode(statementVertex *&root, tString name) {
	if (root != NULL) {
		if (root->isLeaf()) {
			return (!(root->statement == name) ? root : NULL);
		} else {
			statementVertex *x = akFindNode(root->no, name);
			statementVertex *y = akFindNode(root->yes, name);

			return (x != NULL ? x : y);
		}
	}
	return NULL;
}

//! Gives properties of given object.
void akGetObjectDefinition(tString name, statementVertex *&root,
		tList<const tPair<tString, bool>> *stacktrc) {
	bool created = 0;
	if (stacktrc == NULL) {
		stacktrc = new tList<const tPair<tString, bool>>();
		created = 1;
	}
	if (root != NULL) {
		if (root->isLeaf()) {
			if (!(root->statement == name)) {
				name.tWrite();
				std::cout << DEFIN << "\n";
				stacktrc->tForEach([](const tPair<tString, bool> &elem) {
					if (!elem.y) {
						std::cout << DEFIN_NO;
					}
					elem.x.tWrite();
					std::cout << "\n";
				});
			}
		} else {
			stacktrc->tAddLast( { root->statement, 1 });
			akGetObjectDefinition(name, root->yes, stacktrc);
			stacktrc->tRemoveLast();

			stacktrc->tAddLast( { root->statement, 0 });
			akGetObjectDefinition(name, root->no, stacktrc);
			stacktrc->tRemoveLast();
		}
	}
	if (created) {
		delete stacktrc;
	}
}

void akCreateDotFile(statementVertex *root, tFile *dest) {
	dest->tStartMapping(2 * akTreeVerticesCount(root) * STR_LEN);

	dest->tWriteLine("digraph AKINATOR {");

	tList<statementVertex*> queue;

	queue.tAddLast(root);
	while (queue.tGetSize() != 0) {
		statementVertex *curr = queue.tGet(0);
		queue.tRemove(0);

		if (curr == NULL) {
			continue;
		}

		dest->tWriteLine(curr->statement.tReplace(' ', '_') + " [label=" + tString('"') + curr->statement + tString('"') + "]");

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
			dest->tWrite(curr->statement.tReplace(' ', '_'));
			dest->tWrite("->");
			dest->tWrite(curr->no->statement.tReplace(' ', '_'));
			dest->tWriteLine(";");
		}
		if (curr->yes != NULL) {
			dest->tWrite(curr->statement.tReplace(' ', '_'));
			dest->tWrite("->");
			dest->tWriteLine(curr->yes->statement.tReplace(' ', '_'));
			dest->tWriteLine(";");
		}

		queue.tAddLast(curr->yes);
		queue.tAddLast(curr->no);

	}

	dest->tWriteLine("}");

	tString name = tString(dest->tGetName());

	delete dest;

	tCreateDotImg(name, tString("img_") + name);
}
//===========================================
