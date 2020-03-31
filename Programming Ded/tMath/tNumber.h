#ifndef T_INT
#define T_INT

#include <iostream>
#include "../tUtilities/tString.h"

using namespace std;
using namespace tUtilities;

namespace tMath {

template<typename T = int> class tNumber {
public:
	T core;

	tNumber() {
		std::cout << "Contructor 1\n";
	}

	tString tToString() {
		return {core};
	}

	tNumber(T cr) {
		core = cr;
		std::cout << "Contructor 2, " << core << "\n";
	}

	~tNumber() {
		std::cout << "Destructor, " << core << "\n";
	}

	tNumber operator=(T cr) {
		return {cr};
	}

	tNumber operator+(tNumber b) {
		return {this->core + b.core};
	}

	tNumber operator-(tNumber b) {
		return {this->core - b.core};
	}

	tNumber operator*(tNumber b) {
		return {this->core * b.core};
	}

	tNumber operator/(tNumber b) {
		return {this->core / b.core};
	}

	tNumber operator++() {
		return {this->core + 1};
	}

	bool operator<(tNumber b) {
		return this->core < b.core;
	}

	bool operator>(tNumber b) {
		return this->core > b.core;
	}

	tNumber operator+=(tNumber b) {
		return {this->core + b.core};
	}

	tNumber operator-=(tNumber b) {
		return {this->core - b.core};
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
