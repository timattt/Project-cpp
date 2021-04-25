#include <stddef.h>

#include "../Utilities/CallTreeBuilder.hpp"
#include "../Utilities/Utilities.hpp"


/*
 * SYMBOLS:
 * [A] - один из символов A.
 * A+ - A есть один и более раз.
 * A* - А есть ноль и более раз.
 * A? - А есть ноль или один раз.
 */
/*
 * G ::= N;
 * N ::= [0-9]+;
 */
struct test1 {

	char *s = NULL;

	int G(char *arr) {
		s = arr;

		int val = N();

		if (*s != '\0') {
			THROW_T_EXCEPTION("End is not reached!");
		}

		return val;
	}

	int N() {
		int val = 0;

		for (; *s <= '9' && '0' <= *s; s++) {
			val = val * 10 + (*s - '0');
		}
		return val;
	}

};

/*
 * G ::= E;
 * E ::= N{[+-]N}*;
 * N ::= [0-9]+;
 */
struct test2 {

	char *s = NULL;

	// G ::= E;
	int G(char *arr) {
		s = arr;

		int val = E();

		if (*s != '\0') {
			THROW_T_EXCEPTION("End is not reached!");
		}

		return val;
	}

	// E ::= N{P};
	int E() {
		int val = 0;

		val = N();

		while (_P()) {
			val += P();
		}

		return val;
	}

	// P ::= [+-]N;
	int P() {
		char sign = *(s++);
		return (sign == '-' ? -1 : 1) * N();
	}

	// FITTER for P
	bool _P() {
		return *s == '+' || *s == '-';
	}

	// N ::= [0-9]+;
	int N() {
		int val = 0;

		for (; *s <= '9' && '0' <= *s; s++) {
			val = val * 10 + (*s - '0');
		}
		return val;
	}

};

/*
 * G ::= E;
 * E ::= T{[+-]T}*;
 * T ::= N{[*\]N}*;
 * N ::= [0-9]+;
 */
struct test3 {

	char *s = NULL;

	// G := E;
	int G(char *arr) {
		s = arr;

		int val = E();

		if (*s != '\0') {
			THROW_T_EXCEPTION("End is not reached!");
		}

		return val;
	}

	int E() {
		int val = 0;

		val = T();

		while (*s == '+' || *s == '-') {
			char op = *s++;
			if (op == '+') {
				val += T();
			}
			if (op == '-') {
				val -= T();
			}
		}

		return val;
	}

	bool _T() {
		return _N();
	}

	bool _N() {
		return *s >= '0' && *s <= '9';
	}

	int T() {
		int val = 0;

		val = N();

		while (*s == '*' || *s == '/') {
			char op = *s++;
			if (op == '*') {
				val *= N();
			}
			if (op == '/') {
				val /= N();
			}
		}

		return val;
	}

	// N := [0-9]+;
	int N() {
		int val = 0;

		for (; *s <= '9' && '0' <= *s; s++) {
			val = val * 10 + (*s - '0');
		}
		return val;
	}

};

/*
 *  G ::= E;
 *  E ::= T{[+-]T}*;
 *  T ::= P{[*\]P}*;
 *  P ::= (E) | N;
 *  N ::= [0-9]+;
 */
struct test4 {

	char *s = NULL;

	// G := E;
	int G(char *arr) {
		s = arr;

		__tFastStart("GRAMMAR_TEST");

		__tGoInto("G");

		int val = E();

		if (*s != '\0') {
			THROW_T_EXCEPTION("End is not reached!");
		}

		__tGoOut(val);

		__tFastEnd();
		return val;
	}

	// E ::= T{[+-]T}*;
	int E() {
		__tGoInto("E");
		int val = 0;

		val = T();

		while (*s == '+' || *s == '-') {
			char op = *s++;
			if (op == '+') {
				val += T();
			}
			if (op == '-') {
				val -= T();
			}
		}
		__tGoOut(val);
		return val;
	}

	// T ::= P{[*\]P}*;
	int T() {
		__tGoInto("T");
		int val = 0;

		val = P();

		while (*s == '*' || *s == '/') {
			char op = *s++;
			if (op == '*') {
				val *= P();
			}
			if (op == '/') {
				val /= P();
			}
		}
		__tGoOut(val);
		return val;
	}

	// P ::= (E) | N;
	int P() {
		__tGoInto("P");
		int val = 0;
		if (*s == '(') {
			s++;
			val = E();
			if (*s == ')') {
				s++;
			} else {
				THROW_T_EXCEPTION("No close bracket!");
			}
		} else {
			val = N();
		}
		__tGoOut(val);
		return val;
	}

	bool _N() {
		return *s >= '0' && *s <= '9';
	}

	// N := [0-9]+;
	int N() {
		__tGoInto("N");
		int val = 0;

		for (; *s <= '9' && '0' <= *s; s++) {
			val = val * 10 + (*s - '0');
		}
		__tGoOut(val);
		return val;
	}

};
