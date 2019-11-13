#ifndef T_FORMULA_CALCULATOR
#define T_FORMULA_CALCULATOR

#include "../tUtilities/tString.h"
#include "../tExpressionHandler/tExpressionHandler.h"
#include <bits/stdc++.h>
#include "../tUtilities/tUtilities.h"

using namespace tUtilities;
using namespace tExpressionHandler;

using std::cout;

void tCalculateFormula(tString formula, map<tString, double> values,
		map<tString, double> infs, double &ans, double &inf) {
	tExprNode *expr = new tExprNode(formula);

	tList<tString> vars = { };
	tFindAllVariables(expr, &vars);

	tExprNode **diffs = (tExprNode**) calloc(vars.tGetSize(),
			sizeof(tExprNode*));

	for (unsigned i = 0; i < vars.tGetSize(); i++) {
		tString var = vars.tGet(i);

		diffs[i] = tDifferentiate(expr, { var });
	}

	ans = tCalc(expr, values);

	inf = 0;

	for (unsigned i = 0; i < vars.tGetSize(); i++) {
		tString var = vars.tGet(i);
		inf += std::pow((tCalc(diffs[i], values)) * infs[var], 2);
	}

	inf = std::sqrt(inf);

	int digits = tGetFirstSignificantDecimalPlace(inf);

	ans = tRoundTo(ans, digits);
	inf = tRoundTo(inf, digits);

	delete expr;
}

void tParseLabEaterFile(tFile *in_file, tFile *out_file) {
	in_file->tStartMapping();
	out_file->tStartMapping(10 * tGetFileSize(in_file->tGetName()));

	map<tString, double> values = { };
	map<tString, double> infs = { };

	while (in_file->tHasMoreSymbs()) {
		tString line = in_file->tReadLine();
		if (line.tContains("//") || line.tCount('=') == 0) {
			out_file->tWriteLine(line);
			continue;
		}

		tString name = line.tSubstring(0, line.firstPosition('=') - 1);

		out_file->tWrite(name);
		out_file->tWrite("=");

		double value = 0;
		double infl = 0;

		// This is initialization of variable VAR=VALUE+-INFIL
		if (line.tContains("+-")) {
			tString value_str = line.tSubstring(line.firstPosition('=') + 1,
					line.firstPositionStr("+-") - 1);
			tString inf_str = line.tSubstring(line.firstPositionStr("+-") + 2,
					line.size - 2);

			value = values[name] = value_str.tToFloat();
			infl = infs[name] = inf_str.tToFloat();
		} else {
			tString expr = line.tSubstring(line.firstPosition('=') + 1,
					line.size - 2);

			tCalculateFormula(expr, values, infs, value, infl);

			values[name] = value;
			infs[name] = infl;

			out_file->tWrite(expr);
			out_file->tWrite('=');
		}

		out_file->tWriteNum_fl(value);
		out_file->tWrite("+-");
		out_file->tWriteNum_fl(infl);
		out_file->tWriteLine(";");

	}

	out_file->tMovePointer(-1);

	tShrink(out_file);
	in_file->tStopMapping();
}

void tRunLabEater() {
	char c = 0;
	while (c != 'e') {
		cout << "What do you want to do? [p - parse file, e - exit]\n";
		c = tReadCharFromLine();
		if (c == 'p') {
			cout << "Input source file name!\n";
			tString src_name = { };
			src_name.tReadLine();
			cout << "Input export file name!\n";
			tString exp_name = { };
			exp_name.tReadLine();

			tFile *in = new tFile(src_name);
			tFile *out = new tFile(exp_name);
			tParseLabEaterFile(in, out);
			delete in;
			delete out;
		}
	}
}

#endif
