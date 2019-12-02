#ifndef T_GRAMMAR_PP
#define T_GRAMMAR_PP

#include "../../tUtilities/tString.h"
#include "../../tUtilities/tUtilities.h"
#include "../../tUtilities/tCallTreeBuilder.h"
#include "bits/stdc++.h"
#include "../../tStorage/tTreap.h"
#include "../tGrammarParserUtilities.h"

using namespace std;
using namespace tUtilities;
using namespace tStorage;

//! Writes something like this
//! EXAMPLE: smt = "abc";
//! 		(*(s + 0) == 'a' && *(s + 1) == 'b' && *(s + 2) == 'c');
tString writeEquals(tString smt, unsigned dx = 0) {
	tString res = { "(" };

	// If there is encounter
	if (smt.tCount('-') > 0 && smt.size() == 3) {
		char from = smt[0];
		char to = smt[2];
		res += (tString("(*s) <= ") + tString(char(39)) + to + tString(char(39))
				+ tString(" && (*s) >= ") + tString(char(39)) + from
				+ tString(char(39)));
	} else {
		for (unsigned i = 0; i < smt.size(); i++) {
			res += "*(s + ";
			res += tString(i + dx);
			res += ") == '";
			res += smt[i];
			if (smt[i] == '\\') {
				res += smt[i + 1];
				i++;
			}
			res += "'";
			if (i + 1 < smt.size()) {
				res += " && ";
			}
		}
	}

	res += ')';

	return res;
}

//! Creates checker for nonterminal
//! @param name - name of this nonterminal
//! @param line_ - body of this nonterminal
//! @param nonterminals - list of all nonterminals
//! @param total_nont - total of nonterminals
tString createChecker(tString name, tString body, tString *nonterminals,
		unsigned total_nont) {
	tString res = { };
	tString line = tString(body);

	// Splitting by these symbols
	char bad[] = { '"', '[', ']' };

	// Checker declaration
	res += (tString("bool ") + name + tString("_() { return ("));

	// If nonterminal begins with terminal
	if (line[0] == '[') {
		for (unsigned j = 0; (line.size() > 0 && line[0] == '['); j++) {
			// Inserting AND operator between pairs of brackets
			if (j > 0) {
				res += " && ";
			}
			// End of this brackets pair
			unsigned end = line.firstPosition(']');
			unsigned total_divs = 0;

			// Splitting by brackets and quotes
			// Result will be something that lies in the quotes pairs inside these brackets pair.
			tString *divs = line.tSubstring(0, end - 1).tParse(bad, 3,
					total_divs);

			// If this symbol can not exists
			if (end + 1 < line.size()
					&& (line[end + 1] == '*' || line[end + 1] == '?')) {
				break;
			}

			// quotes makes compiler choose any combination of symbols that are stored inside
			for (unsigned i = 0; i < total_divs; i++) {
				res += (writeEquals(divs[i], j));
				// Inserting OR operator between pairs of quotes
				if (i + 1 < total_divs) {
					res += " || ";
				}
			}

			// Freeing memory
			delete divs;

			// If there is nothing after these brackets pair then break
			if (end + 1 >= line.size()) {
				break;
			}

			// Cutting of text that was parsed already
			line = line.tSubstring(end + 1, line.size() - 1);
		}
	} else {
		// Finding leading nonterminal and make link to it
		for (unsigned i = 0; i < total_nont; i++) {
			if (line.tIsPrefix(nonterminals[i])) {
				res += (nonterminals[i] + "_()");
				break;
			}
		}
	}

	// Close brackets
	res += ");}";

	return res;
}

//! This function parses nonterminal if it is defined using or operator.
//! @param name - nice looking name of nonterminal
//! @param body - nice looking body of nonterminal
//! @param nonterminals - array of known nonterminals
//! @param total_nont - size of this array
//! @returns string with functions bounded to this nonterminal
tString parseOr(tString name, tString body, tString *nonterminals,
		unsigned total_nont) {
	// Creating checker function
	tString res = { tString("bool ") + name + tString("_() { return (") };

	// Will be split by or symbol
	char bad[] = { '|' };

	// Splitting
	unsigned total_divs = 0;
	tString *divs = body.tParse(bad, 1, total_divs);

	// Creating statement A1_() | A2_() | A3_() | ...
	for (unsigned i = 0; i < total_divs; i++) {
		res += (divs[i] + "_()");
		if (i + 1 < total_divs) {
			res += " | ";
		}
	}

	// Close brackets and finish with checker function
	res += ");}\n";

	// Creating head function for this nonterminal
	// Declaration
	res += (tString("tString ") + name + tString("(tLangNode * par) {\n"));
	// Result
	res += (tString("\ttString res = {};\n"));
	// Registering this nonterminal as a vertex in the tree.
	res += (tString("\tpar->addChild(new tLangNode(") + tString('"') + name
			+ tString('"') + tString("));\n"));

	// Creating if statements
	for (unsigned i = 0; i < total_divs; i++) {
		res +=
				(tString("\tif ( ") + divs[i] + tString("_() ) {\n")
						+ tString("\t\tres = ") + divs[i]
						+ tString(
								"(par->getLastChild());\n\t\tpar->getLastChild()->value = res;\n\t\treturn res;\n\t}\n"));
	}

	// Returning result
	res += "\treturn res;\n}";

	// Free unused memory
	delete divs;

	return res;
}

//! This function works with nonterminal line without or operator.
//! @param name - nice looking name of nonterminal
//! @param body - nice looking body of nonterminal
//! @param nonterminals - array of known nonterminals
//! @param total_nont - size of this array
//! @returns string with functions bounded to this nonterminal
tString parseNoOr(tString name, tString body_, tString *nonterminals,
		unsigned total_nont) {
	tString res = { };

	// Declaration of nonterminal function
	res += (tString("\ntString ") + name + tString("(tLangNode * par) {\n"));

	// Registering this nonterminal as a vertex in the tree.
	res += (tString("\tpar->addChild(new tLangNode(") + tString('"') + name
			+ tString('"') + tString("));\n"));

	// Make copy
	tString body = tString(body_);

	// Creating result variable
	res += (tString("\ttString res = {};\n"));

	// While line is not finished
	while (body.size() > 0) {
		// Creating tabulation for nice looking code
		res += '\t';

		// There is terminal in front
		if (body[0] == '[') {
			unsigned end = 2;

			// Calculating size of terminal
			for (; body[end] != '"'; end++) {
			}
			unsigned size = end - 2;

			// If there is counter operator
			if (body.tCount('-') > 0
					&& body.firstPosition('-') < body.firstPosition(']')) {
				res += (tString("res += *s;\n"));
				res += (tString("\ts += 1;\n"));
			} else {
				// If there is no such operator
				// Just collect SIZE symbols
				res += (tString("for (unsigned i = 0; i < ") + tString(size)
						+ tString("; i++) { res += *(s+i); }\n"));
				// And move caret to plus SIZE
				res += (tString("s += ") + tString(size) + tString(";\n"));
			}

			// If there is nothing after this brackets pair
			if (body.firstPosition(']') + 1 >= body.size()) {
				body = { };
			} else {
				// If there will be something
				body = body.tSubstring(body.firstPosition(']') + 1,
						body.size() - 1);
			}
		} else {
			// Calculating nonterminal index
			// And cropping nonterminal from body
			unsigned j = 0;
			for (; j < total_nont; j++) {
				if (body.tIsPrefix(nonterminals[j])) {
					if (nonterminals[j].size() >= body.size()) {
						body = { };
					} else {
						body = body.tSubstring(nonterminals[j].size(),
								body.size() - 1);
					}
					break;
				}
			}

			// If something goes after nonterminal
			// Some operator maybe
			if (body.size() > 0
					&& (body[0] == '*' || body[0] == '+' || body[0] == '?')) {
				// While loop
				if (body[0] == '*' || body[0] == '+')
					res += (tString(
							tString("while ( ") + nonterminals[j]
									+ tString("_() ) ")));
				// Just if statement
				if (body[0] == '?')
					res += (tString(
							tString("if ( ") + nonterminals[j]
									+ tString("_() ) ")));
				// If there is nothing after this operator
				if (body.size() == 1) {
					body = { };
				} else {
					// If not
					body = body.tSubstring(1, body.size() - 1);
				}
			}

			// Invoking new nonterminal function
			res += (tString("res += ") + nonterminals[j]
					+ tString("(par->getLastChild());\n"));
		}
	}

	// Setting this vertex value
	res += "\tpar->getLastChild()->value = res;\n\treturn res;\n}";
	return res;
}

//! This function removes all spaces and bad symbols except those which are lies inside quotes
//! @param from - string from which spaces will be removed
//! @returns nice looking string
tString removeSpacesSafe(tString from) {
	// result
	tString to = { };

	// Total quotes right now
	int total_ = 0;

	for (unsigned i = 0; i < from.size(); i++) {
		// First quote is opens second quote closes
		if (from[i] == '"') {
			total_++;
			total_ %= 2;
		}
		// If symbols is not inside quotes and it is bad then it will not be added.
		if (total_ == 1
				|| (from[i] != '\t' && from[i] != ' ' && from[i] != ';')) {
			to += from[i];
		}
	}

	return to;
}

//! This function converts normal line from grammar into nice looking nonterminal name and its body.
//! Spaces will be safely removed using removeSpacesSafe function.
//! @param smt - line from grammar.
//! @param name - name of this nonterminal will be stored in here.
//! @param body - body of nonterminal will be stored here.
void splitAsNameAndBody(tString smt, tString &name, tString &body) {
	// Position before :
	unsigned eqsi = smt.firstPosition(':') - 1;
	// Position after ::=
	unsigned eqei = eqsi + 4;
	name = removeSpacesSafe(smt.tSubstring(0, eqsi));
	body = removeSpacesSafe(smt.tSubstring(eqei, smt.size() - 1));
}

//! This function collects all nonterminal symbols.
//! @param fl - file with grammar.
//! @total quantity of nonterminals found.
//! @returns array of all nonterminals presented in the grammar.
tString* findAllNonterminals(tFile *fl, unsigned &total) {
	tList<tString> res = { };

	// Initializing file
	fl->tStartMapping();

	while (fl->tHasMoreSymbs()) {
		tString ln = fl->tReadLine();
		if (ln.tContains("::=")) {
			tString nm = { };
			tString bd = { };
			splitAsNameAndBody(ln, nm, bd);
			res.tAddLast(nm);
		}
	}

	// Flushing file
	fl->tStopMapping();

	// Returning quantity
	total = res.tGetSize();

	// Converting to array
	return res.tToArray();
}

//! This function generates prototypes for all nonterminals in this grammar.
//! @param dest - file where code may be stored.
//! @param nonterms - all nonterminal names
//! @param nontrms_tot - quantity of nonterminals
void genProts(tFile *dest, tString *nonterms, unsigned nontrms_tot) {
	for (unsigned i = 0; i < nontrms_tot; i++) {
		// Checker function
		dest->tWriteLine(tString("bool ") + nonterms[i] + tString("_();"));
		// Nonterminal function with tLangNode as parameter
		dest->tWriteLine(
				tString("tString ") + nonterms[i]
						+ tString("(tLangNode* par);"));
	}

	// Skipping line
	dest->tWriteLine("");
}

//! This function generates code for parser of grammar.
//! @param gr - file where grammar is.
//! @param dest - file where code will be stored.
void tCreateParser(tFile *gr, tFile *dest) {
	unsigned total_nonterms = 0;
	tString *nonterms = findAllNonterminals(gr, total_nonterms);

	// Init files
	dest->tStartMapping(10000);
	gr->tStartMapping();

	// Info and namespace and init function
	dest->tWriteLine(
			"//THIS CODE IS AUTOGENERATED BY RED GRAMMAR PARSER made by timattt//\n\nnamespace tLangParserAuto { \n\nchar *s = NULL;\nchar *array = NULL;\nvoid init(tString text) {\n\tarray = text.tToPlainArray();\n\ts = array;\n}\n");

	// Generating prototypes for functions
	genProts(dest, nonterms, total_nonterms);

	// Parsing lines with nonterminals
	while (gr->tHasMoreSymbs()) {
		tString line_out = { };
		tString line_in = (gr->tReadLine());

		tString name = { };
		tString body = { };

		// If it is strange line
		if (!line_in.tContains("::=")) {
			continue;
		}

		// Finding name and body for nonterminal
		splitAsNameAndBody(line_in, name, body);

		if (line_in.tCount('|') > 0) {
			// If there is or operator then it will be parsed separately
			line_out += parseOr(name, body, nonterms, total_nonterms);
		} else {
			line_out += createChecker(name, body, nonterms, total_nonterms);
			line_out += parseNoOr(name, body, nonterms, total_nonterms);
		}

		// Skipping line
		line_out += '\n';

		dest->tWriteLine(line_out);
	}

	// Adding close bracket
	dest->tWriteLine("};");

	tShrink(dest);
	gr->tStopMapping();
	dest->tStopMapping();

}

#endif
