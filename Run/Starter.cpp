#include <cstdio>
#include "../Utilities/String.hpp"
#include "../Utilities/Utilities.hpp"
#include "Run.hpp"

#ifdef BUILD_TREAP

int main(int argc, char *argv[]) {
	__tNewBranch();
	try {
		treapRun(argc - 1, argv + 1);

	} catch (tException *ex) {
		printf("Something bad happens...\n");
		ex->print();
	}

	__tFlushBranch();
	return 0;
}

#endif

#ifdef BUILD_COM

int main(int argc, char *argv[]) {
	__tNewBranch();
	try {
		comRun(argc - 1, argv + 1);

	} catch (tException *ex) {
		printf("Something bad happens...\n");
		ex->print();
	}

	__tFlushBranch();
	return 0;
}

#endif

#ifdef BUILD_EXPR

int main(int argc, char *argv[]) {
	__tNewBranch();
	try {
		exprRun(argc - 1, argv + 1);

	} catch (tException *ex) {
		printf("Something bad happens...\n");
		ex->print();
	}

	__tFlushBranch();
	return 0;
}

#endif

#ifdef BUILD_GRAM

int main(int argc, char *argv[]) {
	__tNewBranch();
	try {
		gramRun(argc - 1, argv + 1);

	} catch (tException *ex) {
		printf("Something bad happens...\n");
		ex->print();
	}

	__tFlushBranch();
	return 0;
}

#endif

#ifdef BUILD_INTR

int main(int argc, char *argv[]) {
	__tNewBranch();
	try {
		intrRun(argc - 1, argv + 1);

	} catch (tException *ex) {
		printf("Something bad happens...\n");
		ex->print();
	}

	__tFlushBranch();
	return 0;
}

#endif

#ifdef BUILD_AK

int main(int argc, char *argv[]) {
	__tNewBranch();
	try {
		akRun(argc - 1, argv + 1);

	} catch (tException *ex) {
		printf("Something bad happens...\n");
		ex->print();
	}

	__tFlushBranch();
	return 0;
}

#endif

#ifdef BUILD_ALL
int main(int argc, char *argv[]) {
	__tNewBranch();

	printf("Running timattt's projects...\n");

	if (argc == 1) {
		printf("See README for more info...\n");
	} else try {
		CREATE_ARG(1);

		// HELP
		if (arg1 == "help" || arg1 == "version") {
			printf("See README for more info...\n");
		}

		// AKINATOR
		if (arg1 == "akin") {
			akRun(argc - 2, argv + 2);
		}

		// PROCESSOR
		if (arg1 == "comp") {
			comRun(argc - 2, argv + 2);
		}

		// EXPRESSIONS
		if (arg1 == "expr") {
			exprRun(argc - 2, argv + 2);
		}

		// GRAMMAR PARSER CREATOR
		if (arg1 == "gram") {
			gramRun(argc - 2, argv + 2);
		}

		// INTERPRITATOR
		if (arg1 == "intr") {
			intrRun(argc - 2, argv + 2);
		}

	} catch (tException *ex) {
		printf("Something bad happens...\n");
		ex->print();
	}

	__tFlushBranch();
	return 0;
}

#endif
