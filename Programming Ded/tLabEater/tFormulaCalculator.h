#ifndef T_FORMULA_CALCULATOR
#define T_FORMULA_CALCULATOR

#include "../tUtilities/tString.h"
#include "../tExpressionHandler/tExpressionHandler.h"
#include <bits/stdc++.h>
#include "../tUtilities/tUtilities.h"

using namespace tUtilities;
using namespace tExpressionHandler;

using std::cout;
using std::cin;

void tCalculateFormula() {
	cout << "Insert formula!\n";
	tString formula = { };
	formula.tReadLine();
	tExprNode *expr = new tExprNode(formula);

	tList<tString> vars = { };
	tFindAllVariables(expr, &vars);

	map<tString, double> var_values;
	map<tString, double> var_infelicity;

	tExprNode **diffs = (tExprNode**) calloc(vars.tGetSize(), sizeof(tExprNode*));

	for (unsigned i = 0; i < vars.tGetSize(); i++) {
		tString var = vars.tGet(i);
		cout << "Input value and infelicity for ";
		var.tWrite();
		cout << "\n";
		double value = 0;
		double inf = 0;
		cin >> value >> inf;
		var_values[var] = value;
		var_infelicity[var] = inf;

		diffs[i] = tDifferentiate(expr, {var});
	}

	double ans = tCalc(expr, var_values);

	double inf = 0;

	for (unsigned i = 0; i < vars.tGetSize(); i++) {
		tString var = vars.tGet(i);
		inf += std::pow(
				(tCalc(diffs[i], var_values))
						* var_infelicity[var], 2);
	}

	inf = std::sqrt(inf);

	cout << ans << "+-" << inf << "\n";

	delete expr;
}

#endif
