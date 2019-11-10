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

	tString vars = { };
	tFindAllVariables(expr, vars);

	map<tString, double> var_values;
	map<tString, double> var_infelicity;

	for (unsigned i = 0; i < vars.size; i++) {
		cout << "Input value and infelicity for " << vars[i] << "\n";
		double value = 0;
		double inf = 0;
		cin >> value >> inf;
		var_values[tString(vars[i])] = value;
		var_infelicity[tString(vars[i])] = inf;
	}

	tExprNode **diffs = (tExprNode**) calloc(vars.size, sizeof(tExprNode*));

	for (unsigned i = 0; i < vars.size; i++) {
		diffs[i] = tDifferentiate(expr, tString(vars[i]));
	}

	double ans = expr->tCalc(var_values);

	double inf = 0;

	for (unsigned i = 0; i < vars.size; i++) {
		inf += std::pow(
				(diffs[i]->tCalc(var_values))
						* var_infelicity[tString(vars[i])], 2);
	}

	cout << ans << "+-" << inf << "\n";

	delete expr;
}

#endif
