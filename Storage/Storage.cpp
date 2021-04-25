#include "../Storage/Storage.hpp"

unsigned tDefaultHash(char *arg, const unsigned sz) {
	unsigned result = 0;

	unsigned pow = 1;
	for (unsigned i = 0; i < sz; i++) {
		result += pow * arg[i];
		pow *= Y;
	}

	return result;
}
