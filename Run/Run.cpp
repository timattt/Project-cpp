#include "Run.hpp"

#include <cstdio>
#include <iostream>

#include "../Akinator/Akinator.hpp"
#include "../Expressions/ExpressionHandler.hpp"
#include "../GrammarParsing/GrammarParsersCreator/GrammarParsersCreator.hpp"
#include "../Interpritator/GrammarParserUtilities.hpp"
#include "../Interpritator/Interpritator.hpp"
#include "../Interpritator/LangParser.hpp"
#include "../Compilers/Processor.hpp"
#include "../Utilities/FileHandler.hpp"
#include "../Utilities/String.hpp"
#include "../Utilities/Utilities.hpp"

void comRun(int argc, char * argv[]) {

	printf("Running COMPILERS subproject...\n");

	CREATE_ARG(0);

	if (arg0 == "compile") {
		CREATE_ARG(1);

		if (arg1 == "exe") {
			CREATE_NAME_ARG(2);
			CREATE_NAME_ARG(3);

			tFile *src = new tFile(arg2);
			tFile *dest = new tFile(arg3);

			printf("Begin compile TASM to EXE...\n");

			comCompile_TASM_to_EXE(src, dest);

			printf("Compile OK\n");

			delete src;
			delete dest;

		}
		if (arg1 == "texe") {
			CREATE_NAME_ARG(2);
			CREATE_NAME_ARG(3);

			tFile *src = new tFile(arg2);
			tFile *dest = new tFile(arg3);

			printf("Begin compile TASM to TEXE...\n");

			comCompile_TASM_to_TEXE(src, dest);

			printf("Compile OK\n");

			delete src;
			delete dest;

		}
	}
	if (arg0 == "disasm") {
		CREATE_ARG(1);

		if (arg1 == "texe") {
			CREATE_NAME_ARG(2);
			CREATE_NAME_ARG(3);

			tFile *src = new tFile(arg2);
			tFile *dest = new tFile(arg3);

			printf("Begin disasm...\n");

			comDisasm_TEXE_to_TASM(src, dest);

			printf("Disasm OK\n");

			delete src;
			delete dest;

		}
	}
	if (arg0 == "transToSys") {
		CREATE_ARG(1);

		if (arg1 == "texe") {
			CREATE_NAME_ARG(2);
			CREATE_NAME_ARG(3);

			tFile *src = new tFile(arg2);
			tFile *dest = new tFile(arg3);

			printf("Begin translate TEXE to EXE...\n");

			comTranslateFast_TEXE_to_EXE(src, dest);

			printf("Translate OK\n");

			delete src;
			delete dest;

		}
	}
	if (arg0 == "run") {
		CREATE_ARG(1);

		if (arg1 == "exe") {
			CREATE_NAME_ARG(2);

			tFile *src = new tFile(arg2);

			printf("Running EXE...\n");

			comRun_EXE(src);

			printf("Running OK\n");

			delete src;
		}
		if (arg1 == "texe") {
			CREATE_NAME_ARG(2);

			tFile *src = new tFile(arg2);

			printf("Running TEXE...\n");

			comRun_TEXE(src);

			printf("Running OK\n");

			delete src;
		}
	}

}

void akRun(int argc, char * argv[]) {
	__tNewBranch();

	printf("Running AKINATOR subproject...\n");

	statementVertex *root = NULL;

	char op = 0;
	while (1) {
		std::cout << "Choose operation: g - guess, d - define, e - end, s - save, l - load, p - paint TX, t - terminate TX window, w - wizzard graph" << "\n";

		op = tReadCharFromLine();
		if (op == 'e') {
			break;
		}
		if (op == 'g') {
			akGuess(root);
		}
		if (op == 'd') {
			std::cout << "Input name" << "\n";
			tString name = { };
			name.tReadLine();
			akGetObjectDefinition(name, root);
		}
		if (op == 's') {
			std::cout << "Input file name" << "\n";
			tString name = { };
			name.tReadLine();
			tFile *out = new tFile(name);
			akSaveToFile(out, root);
			delete out;
		}
		if (op == 'l') {
			std::cout << "Input file name" << "\n";
			tString name = { };
			name.tReadLine();
			tFile *in = new tFile(name);
			root = akLoadFromFile(in);
			delete in;
		}
		if (op == 'w') {
			std::cout << "Input file name" << "\n";
			tString name = { };
			name.tReadLine();
			tFile *in = new tFile(name);
			akCreateDotFile(root, in);
			delete in;
		}
	}

	__tFlushBranch();
}

void exprRun(int argc, char * argv[]) {

	printf("Running EXPRESSIONS subproject...\n");

	CREATE_ARG(0);
	if (arg0 == "calc") {
		CREATE_NAME_ARG(1);

		ExprNode * head = new ExprNode(arg1);

		printf("Begin calculations...\n");

		double res = exprCalculate(head, {});

		printf("Result: %.2f\n", res);

		printf("Calculations OK\n");

		exprCleanup(head);
	}
	if (arg0 == "diff") {
		CREATE_NAME_ARG(1);
		CREATE_NAME_ARG(2);

		printf("Begin differentiating...\n");

		ExprNode * head = new ExprNode(arg1);
		ExprNode * res = exprDifferentiate(head, arg2);
		exprCleanup(res);
		tString line = exprToString(res);

		printf("Result: %s\n", line.tToPlainArray());

		delete head;
		delete res;

		printf("Differentiating OK\n");
	}
	if (arg0 == "draw") {
		CREATE_NAME_ARG(1);
		CREATE_NAME_ARG(2);

		printf("Begin drawing...\n");

		ExprNode * head = new ExprNode(arg1);
		exprSaveDotImage(arg2, head);

		delete head;

		printf("Drawing OK\n");
	}
	if (arg0 == "tex") {
		CREATE_NAME_ARG(1);

		printf("Begin TEX...\n");

		ExprNode *head = new ExprNode(arg1);
		printf("Result: ");
		exprToTexString(head).tOut();

		delete head;

		printf("TEX OK\n");
	}

}

void gramRun(int argc, char * argv[]) {
	
	printf("Running GRAMMAR subproject...\n");
	
	CREATE_NAME_ARG(0);
	CREATE_NAME_ARG(1);

	printf("Begin creating grammar parser...\n");

	tFile * gramSrc = new tFile(arg0);
	tFile * gram = new tFile(arg1);

	gramCreateParser(gramSrc, gram);

	printf("Creation OK\n");

	delete gramSrc;
	delete gram;
}

void intrRun(int argc, char * argv[]) {
	CREATE_NAME_ARG(0);

	printf("Running INTERPRITATOR subproject...\n");

	tFile *src = new tFile(arg0);
	src->tStartMapping();

	tString text = src->tReadAll();
	delete src;

	tLangNode *head = intrParse(text);


	NOT_NULL(head);

	intrInit(head);
	intrRunProgram();

	printf("Program finished OK\n");fflush(stdout);

//	delete head;
}
