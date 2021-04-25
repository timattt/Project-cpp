#include "Utilities.hpp"

tException::tException(int ln, const char * fn, const char * mes) : lineNumber(ln), fileName(fn), message{0} {
	memcpy(message, mes, tMin<int>(strlen(mes), ERROR_MESSAGE_LENGTH));
}

void tException::print() {
	printf("tException with message [%s] found in file %s on line %d\n", this->message, this->fileName, this->lineNumber);
}

char tReadCharFromLine() {
	std::cout.flush();
	char c = getchar();
	while (getchar() != '\n')
		;
	return c;
}

// For the encapsulation!
void* _MEM_::operator new(size_t s, char *p) {
	return p;
}

_MEM_::_MEM_() {}

double tFloor(double val) {
	return (double) ((long long) (val));
}
void tSwapBuffers(char *a, char *b, unsigned len) {
	for (unsigned i = 0; i < len; i++) {
		char tmp = a[i];
		a[i] = b[i];
		b[i] = tmp;
	}
}


double tCeiling(double val) {
	double floor = tFloor(val);
	return (val - floor > 0 ? floor + 1 : floor);
}
