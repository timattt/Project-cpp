#ifndef T_FORMULA_CALCULATOR
#define T_FORMULA_CALCULATOR

#include "../tUtilities/tString.h"
#include "../tExpressionHandler/tExpressionHandler.h"
#include <bits/stdc++.h>
#include "../tUtilities/tUtilities.h"

using namespace tUtilities;
using namespace tExpressionHandler;

using std::cout;

struct tLangVariable {
	double *values;
	double *infilicity;
	unsigned size;
	tString name;
	tExprNode *expr;

	tVariable(tString nm) :
			values(NULL), infilicity(NULL), size(0), name(nm), expr(NULL) {
	}

	tVariable() :
			values(NULL), infilicity(NULL), size(0), name( { "NO_NAME" }), expr(
			NULL) {
	}

	void setSize(unsigned sz) {
		if (values == NULL) {
			values = new double[sz];
			infilicity = new double[sz];
		}
		values = (double*) realloc(values, sz * sizeof(tLangVariable));
		infilicity = (double*) realloc(infilicity, sz * sizeof(tLangVariable));

		size = sz;
	}

	bool isTable() {
		return (size == 1);
	}

	void setValue(double val, double inf) {
		setSize(1);
		values[0] = val;
		infilicity[0] = inf;
	}

	void out() {
		cout << "Variable: ";
		name.tWrite();
		cout << " values: [";
		cout.flush();
		for (unsigned i = 0; i < size; i++) {
			cout << values[i] << " +- " << infilicity[i];
			cout.flush();
			if (i + 1 < size) {
				cout << ", ";
			}
		}
		cout << "]\n";
		cout.flush();
	}

	void writeVar(tFile *fl) {
		fl->tWrite(name);
		fl->tWrite(" = ");
		if (expr != NULL) {
			fl->tWrite(tToString(expr));
			fl->tWrite(" = ");
		}
		if (size == 1) {
			fl->tWriteNum_fl(values[0]);
			fl->tWrite(" +- ");
			fl->tWriteNum_fl(infilicity[0]);
		} else {
			fl->tWrite('[');
			for (unsigned i = 0; i < size; i++) {
				fl->tWriteNum_fl(values[i]);
				if (infilicity[i] != 0) {
					fl->tWrite(" +- ");
					fl->tWriteNum_fl(infilicity[i]);
				}
				if (i + 1 < size) {
					fl->tWrite(", ");
				}
			}
			fl->tWrite(']');
		}
		fl->tWriteLine(tString(';'));
	}
};

map<tString, tLangVariable*> variables = { };

tLangVariable *current_var = NULL;

tLangVariable *a = NULL;
tLangVariable *b = NULL;

const char dividor[] = { ' ', ',', '\t', ';', '\n', '\r' };
const unsigned total_dividors = 6;

void tCalculateFormula(tExprNode *expr, map<tString, double> values,
		map<tString, double> infs, double &ans, double &inf) {

	cout << "Calculating formula!\n";
	tToString(expr).out();

	tList<tString> vars = { };
	tFindAllVariables(expr, &vars);

	tExprNode **diffs = (tExprNode**) calloc(vars.tGetSize(),
			sizeof(tExprNode*));

	cout << "Variables:\n";
	for (unsigned i = 0; i < vars.tGetSize(); i++) {
		tString var = vars.tGet(i);
		var.tWrite();
		cout << " = " << values[var] << " +- " << infs[var] << ";\n";
		diffs[i] = tDifferentiate(expr, var);
	}

	ans = tCalc(expr, values);

	cout << "Answer: " << ans << "\n";

	inf = 0;

	for (unsigned i = 0; i < vars.tGetSize(); i++) {
		tString var = vars.tGet(i);
		inf += std::pow((tCalc(diffs[i], values)) * infs[var], 2);
	}

	inf = std::sqrt(inf);

	cout << "Infilicity: " << inf << "\n";

	int digits = tGetFirstSignificantDecimalPlace(inf);
	cout << "Total digits: " << digits << "\n";
	ans = tRoundTo(ans, digits);
	inf = tRoundTo(inf, digits);

	cout << "Final answer: " << ans << " +- " << inf << "\n";

}

void tLeastSquares(unsigned sz, double *x, double *y, double &a, double &a_inf,
		double &b, double &b_inf) {

	cout << "Array size: " << sz << "\n";
	double av_x = 0;
	cout << "x = [";
	for (unsigned i = 0; i < sz; i++) {
		av_x += x[i];
		cout << x[i];
		if (i + 1 < sz) {
			cout << ", ";
		}
	}
	cout << "]\n";
	av_x /= sz;

	double av_y = 0;
	cout << "y = [";
	for (unsigned i = 0; i < sz; i++) {
		av_y += y[i];
		cout << y[i];
		if (i + 1 < sz) {
			cout << ", ";
		}
	}
	cout << "]\n";
	av_y /= sz;

	double av_x2 = 0;
	for (unsigned i = 0; i < sz; i++) {
		av_x2 += (x[i] * x[i]);
	}
	av_x2 /= sz;

	double av_y2 = 0;
	for (unsigned i = 0; i < sz; i++) {
		av_y2 += (y[i] * y[i]);
	}
	av_y2 /= sz;

	double av_xy = 0;
	for (unsigned i = 0; i < sz; i++) {
		av_xy += (x[i] * y[i]);
	}
	av_xy /= sz;

	b = (av_xy - av_x * av_y) / (av_x2 - av_x * av_x);
	a = av_y - b * av_x;

	b_inf = pow(1 / (double) (sz), 0.5)
			* std::pow((av_y2 - av_y * av_y) / (av_x2 - av_x * av_x) - b * b,
					0.5);
	a_inf = b_inf * pow(av_x2 - av_x * av_x, 0.5);

	cout << "Raw answer: a = " << a << " +- " << a_inf << ", b = " << b
			<< " +- " << b_inf << "\n";

	int digits = tGetFirstSignificantDecimalPlace(a_inf);

	a_inf = tRoundTo(a_inf, digits);
	a = tRoundTo(a, digits);

	digits = tGetFirstSignificantDecimalPlace(b_inf);

	b_inf = tRoundTo(b_inf, digits);
	b = tRoundTo(b, digits);

	cout << "Answer: a = " << a << " +- " << a_inf << ", b = " << b << " +- "
			<< b_inf << "\n";

}

map<tString, double> genValueMap(unsigned ind) {
	map<tString, double> res = { };

	for (auto it = variables.begin(); it != variables.end(); it++) {
		res.operator [](it->second->name) = it->second->values[ind
				% it->second->size];
	}

	return res;
}

map<tString, double> genInflMap(unsigned ind) {
	map<tString, double> res = { };

	for (auto it = variables.begin(); it != variables.end(); it++) {
		res.operator [](it->second->name) = it->second->infilicity[ind
				% it->second->size];
	}

	return res;
}

// Assignment via []
void array_assignment(tString line, tFile *in) {
	if (line.tCount('[') == 0) {
		return;
	}

	cout << "Assignment via array\n";
	cout.flush();

	tString last = line.tSubstring(line.firstPosition(']') + 1,
			line.size() - 1);

	line = line.tSubstring(line.firstPosition('[') + 1,
			line.firstPosition(']') - 1);

	unsigned total_divs = 0;
	tString *divs = line.tParse(dividor, total_dividors, total_divs);

	current_var->setSize(total_divs);

	for (unsigned i = 0; i < total_divs; i++) {
		current_var->infilicity[i] = 0.0;
		if (divs[i].tContains("+-")) {
			double inf = divs[i].tSubstring(divs[i].lastPosition('-') + 1,
					divs[i].size() - 1).tToFloat();
			current_var->infilicity[i] = inf;
		}
		current_var->values[i] = divs[i].tToFloat();
	}

	if (last.tContains("+-")) {
		tString inf_str = last.tSubstring(last.firstPosition('-') + 1,
				last.size() - 2);
		double inf = inf_str.tToFloat();
		for (unsigned i = 0; i < current_var->size; i++) {
			if (current_var->infilicity[i] == 0.0) {
				current_var->infilicity[i] = inf;
			}
		}
	}

	current_var->out();
	delete divs;
}

void expr_assignment(tString line) {
	if (line.tContains("+-") || line.tCount('[') > 0) {
		return;
	}

	cout << "Assignment expression\n";
	cout.flush();

	tString expr_str = line.tSubstring(line.firstPosition('=') + 1,
			line.firstPosition(';') - 1);

	tExprNode *expr = new tExprNode(expr_str);

	cout << "Expression: ";
	tToString(expr).out();

	current_var->expr = expr;

	tList<tString> vars = { };
	tFindAllVariables(expr, &vars);
	cout << "Found variables: ";
	unsigned max_size = 0;
	for (unsigned i = 0; i < vars.tGetSize(); i++) {
		tLangVariable *vr = variables[vars.tGet(i)];
		vr->name.tWrite();
		max_size = tMax(max_size, vr->size);
		cout << "[" << vr->size << "]";
		if (i + 1 < vars.tGetSize()) {
			cout << ", ";
		}
	}
	cout << "\n";

	current_var->setSize(max_size);

	for (unsigned i = 0; i < max_size; i++) {
		map<tString, double> vals = genValueMap(i);
		map<tString, double> infs = genInflMap(i);
		tCalculateFormula(expr, vals, infs, current_var->values[i],
				current_var->infilicity[i]);
	}

	cout << "Formula calculations finished!\n";
	current_var->out();
}

// Assignment via +-
void single_assignment(tString line) {
	if (!line.tContains("+-") || line.tCount('[') > 0) {
		return;
	}

	cout << "Assignment via +-\n";
	cout.flush();

	tString val_str = line.tSubstring(line.firstPosition('=') + 1,
			line.firstPosition('+') - 1);
	tString inf_str = line.tSubstring(line.firstPosition('+') + 2,
			line.firstPosition(';') - 1);

	current_var->setValue(val_str.tToFloat(), inf_str.tToFloat());

	current_var->out();
}

void make_array(tString line) {
	if (!line.tContains("make_array")) {
		return;
	}
	tString str = line.tSubstring(line.firstPosition('[') + 1,
			line.firstPosition(']') - 1);

	cout << "Making array!\n";
	cout.flush();

	unsigned total_divs = 0;
	tString *divs = str.tParse(dividor, total_dividors, total_divs);

	if (variables.count(divs[0]) == 0) {
		variables[divs[0]] = new tLangVariable(divs[0]);
	}
	current_var = variables[divs[0]];
	for (unsigned i = 1; i < total_divs; i++) {
		tLangVariable *vr = variables[divs[i]];
		unsigned beg = current_var->size;
		current_var->setSize(vr->size + beg);
		for (unsigned i = 0; i < vr->size; i++) {
			current_var->values[i + beg] = vr->values[i];
			current_var->infilicity[i + beg] = vr->infilicity[i];
		}
	}

	delete divs;
}

void sum(tString line) {
	if (!line.tContains("sum")) {
		return;
	}

	cout << "Finding sum of ";
	cout.flush();

	tString from = line.tSubstring(line.firstPosition('[') + 1,
			line.firstPosition(',') - 1);
	tString to = line.tSubstring(line.firstPosition(',') + 1,
			line.firstPosition(']') - 1);

	from.tWrite();
	cout << " and store into ";
	to.tWrite();
	cout << ".\n";
	cout.flush();

	tLangVariable *from_ = variables[from];
	tLangVariable *to_ = variables[to];

	double sum = 0;
	double inf = 0;
	for (unsigned i = 0; i < from_->size; i++) {
		sum += from_->values[i];
		inf += (from_->infilicity[i] * from_->infilicity[i]);
	}

	inf = std::pow(inf, 0.5);

	to_->setValue(sum, inf);

	to_->expr = NULL;

	to_->out();
	current_var = to_;
}

//! This function checks if the given line is suitable
//! for the least squares processing function and if so then it starts the corresponding function.
void least_squares(tString line) {
	if (!line.tContains("least_squares")) {
		return;
	}

	cout << "Least squares function\n";
	cout.flush();

	tString x_nm = line.tSubstring(line.firstPosition('[') + 1,
			line.firstPosition(',') - 1);
	tString y_nm = line.tSubstring(line.firstPosition(',') + 1,
			line.firstPosition(']') - 1);

	tLangVariable *x = variables[x_nm];
	tLangVariable *y = variables[y_nm];

	if (x->size != y->size) {
		tThrowException("Array sizes are not equals!");
	}

	tLeastSquares(x->size, x->values, y->values, a->values[0], a->infilicity[0],
			b->values[0], b->infilicity[0]);
}

//! This function finds a variable in a string, or rather tries to find it;
//! if there is an equal sign, then everything that stands before this sign will be considered a change.
tLangVariable* findVar(tString line) {
	if (line.tCount('=') > 0) {
		cout << "Loading variable: ";
		tString nm = line.tSubstring(0, line.firstPosition('=') - 1);
		nm.out();
		if (variables.count(nm) == 0) {
			variables[nm] = new tLangVariable(nm);
		}
		return variables[nm];
	}
	return NULL;
}

void tParseLabEaterFile(tFile *in_file, tFile *out_file) {
	cout << "PARSING FILE!\n";
	in_file->tStartMapping();
	out_file->tStartMapping(10 * tGetFileSize(in_file->tGetName()));

	tString NAME_A = tString('a');
	tString NAME_B = tString('b');

	a = variables[NAME_A] = new tLangVariable(NAME_A);
	b = variables[NAME_B] = new tLangVariable(NAME_B);

	a->setSize(1);
	b->setSize(1);

	while (in_file->tHasMoreSymbs()) {
		tString line = in_file->tReadLine();

		if (line.tContains("//")) {
			out_file->tWriteLine(line);
			continue;
		}

		line = line.tCastofSymbol(' ');

		cout << "-----------------------\n";
		//	line.out();
		cout.flush();

		current_var = findVar(line);
		if (current_var == NULL) {
			// Then it will be function.
			least_squares(line);
			sum(line);
			make_array(line);
		} else {
			// Else it will be assignment.
			single_assignment(line);
			array_assignment(line, in_file);
			expr_assignment(line);
		}

		if (current_var == NULL) {
			a->writeVar(out_file);
			b->writeVar(out_file);
		} else {
			current_var->writeVar(out_file);
		}

		if (in_file->tGetPassedLines() > 1) {
			out_file->tWriteLine( { });
		}
	}
	out_file->tMovePointer(-1);

	tShrink(out_file);

	in_file->tStopMapping();
}

bool runConfig() {
	if (tFileExists("T_LAB_EATER_CONFIG")) {
		tFile *config = new tFile("T_LAB_EATER_CONFIG");
		cout << "Config file found!\n";
		config->tStartMapping();
		tString in = config->tReadLine();
		tString out = config->tReadLine();

		tFile *in_ = new tFile(in);
		tFile *out_ = new tFile(out);

		tParseLabEaterFile(in_, out_);

		delete in_;
		delete out_;

		config->tStopMapping();
		delete config;

		return 1;
	}
	return 0;
}

void tRunLabEater() {
	cout << "LAB EATER VERSION 1.1\n";
	cout << "AUTHOR: timattt\n";
	if (runConfig()) {
		return;
	}
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
