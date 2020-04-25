#ifndef T_INT
#define T_INT

#include <iostream>
#include "../tUtilities/tString.h"

using namespace std;
using namespace tUtilities;

namespace tMath {

map<tString, tString> name_to_id;
map<tString, tString> id_to_name;

tFile *dest;

tString currColor1 = "white";
tString currColor2 = "black";

void setColor1(tString color) {
	currColor1 = color;
}

void setColor2(tString color) {
	currColor2 = color;
}

void begin() {
	DeleteFileA("grsrc");
	dest = new tFile("grsrc");
	dest->tStartMapping(10000);
	dest->tWriteLine("digraph gr {");
}
tString rando() {
	tString res = "";
	for (int i = 0; i < 10; i++) {
		char s = 'a' + (rand() % 20);
		res += s;
	}
	return res;
}
tString id(tString nm) {
	if (name_to_id.count(nm) > 0) {
		return name_to_id[nm];
	} else {
		tString id = rando();
		dest->tWriteLine(
				id + " [style=filled, label=" + '"' + nm + '"' + ", fillcolor="
						+ currColor1 + "];");
		dest->tWriteLine(tString("_") +
				id + " [style=filled, label=" + '"' + nm + '"' + ", fillcolor="
						+ currColor2 + "];");
		name_to_id[nm] = id;
		id_to_name[id] = nm;
		return id;
	}
}

void write(tString p1, tString p2, tString c) {
	tString id_p1 = id(p1);
	tString id_p2 = id(p2);

	tString id_c = id(c);

	dest->tWriteLine(id_p1 + "->" + id_c + ";");
	dest->tWriteLine(id_p2 + "->" + id_c + ";");

	dest->tWriteLine(tString("_") + id_p1 + "-> _" + id_c + ";");
	dest->tWriteLine(tString("_") + id_p2 + "-> _" + id_c + ";");
}

void write(tString p1, tString c) {
	tString id_p1 = id(p1);
	tString id_c = id(c);
	dest->tWriteLine(id_p1 + "->" + id_c + ";");

	dest->tWriteLine(tString("_") + id_p1 + "-> _" + id_c + ";");
}

void end() {
	dest->tWriteLine("}");
	dest->tStopMapping();
	delete dest;
	tCreateDotImg("grsrc", "gr.png");
}

void add(tString v) {
	id(v);
}

template<typename T = int> class tNumber {
public:
	T core;
	tString info;

	tNumber(const tNumber &tn, tString mes) {
		setColor1("red");
		core = tn.core;
		info = mes;
		write(tn.info, info);
	}

	tNumber(T cr, tString var) {
		core = cr;
		info = var;
		setColor1("blue");
		add(info);
	}

	~tNumber() {
	}

	void operator=(const tNumber &cr) {
		setColor1("green");
		this->info.out();
		cr.info.out();
		write(cr.info, tString("(=") + cr.info + ")");
		write(this->info, tString("(=") + cr.info + ")");
		this->info = tString("(=") + cr.info + ")";
		this->core = cr.core;

	}

	tNumber operator+(const tNumber &b) {
		setColor1("yellow");
		write(this->info, b.info, this->info + "+" + b.info);
		tNumber ret(this->core + b.core, this->info + "+" + b.info);
		return ret;
	}

	tNumber operator-(const tNumber &b) {
		setColor1("cyan");
		write(this->info, b.info, this->info + "-" + b.info);
		tNumber ret(this->core - b.core, this->info + "-" + b.info);
		return ret;
	}

	tNumber operator*(const tNumber &b) {
		setColor1("orange");
		write(this->info, b.info, this->info + "*" + b.info);
		tNumber ret(this->core * b.core, this->info + "*" + b.info);
		return ret;
	}

	tNumber operator/(const tNumber &b) {
		setColor1("gray");
		write(this->info, b.info, this->info + "/" + b.info);
		tNumber ret(this->core / b.core, this->info + "/" + b.info);
		return ret;
	}

	tNumber operator++() {
		tNumber ret(this->core + 1, this->info + "+1");
		return ret;
	}
};

typedef tNumber<int> tint;
typedef tNumber<long> tlong;
typedef tNumber<unsigned> tunsigned;

template<typename K> ostream& operator<<(ostream &output, const tNumber<K> &D) {
	output << D.core;
	return output;
}

template<typename K> istream& operator>>(istream &input, tNumber<K> &D) {
	input >> D.core;
	return input;
}

}

#endif
