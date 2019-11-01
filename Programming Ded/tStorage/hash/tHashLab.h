#ifndef T_HASH_LAB
#define T_HASH_LAB

#include "../tHashTable.h"

using namespace tStorage;

#define T_HASH(NAME, CODE) unsigned NAME (const char * elem, unsigned len) {CODE;}
#include "BarGraphProperties.tprop"
#undef T_HASH

const unsigned DICTIONARY_SIZE = 10000;
const unsigned LINE_LEN = 30;

void tGenBarGraph() {
	char li[LINE_LEN];
#define T_HASH(NAME, CODE) {\
	std::cout << "Generating CSV for " << #NAME << "\n";\
	tHashTable<DICTIONARY_SIZE, NAME > ht;	\
	for (unsigned i = 0; i < DICTIONARY_SIZE; i++) {\
		for (unsigned j = 0; j < LINE_LEN; j++) {\
			li[j] = rand();\
		}\
		ht.tAdd(li, (rand() % LINE_LEN) + 1);\
	}\
	tFile * dest = new tFile(#NAME ".txt");\
	ht.tGenerateCSV(dest);\
	delete dest; \
}
#include "BarGraphProperties.tprop"
#undef T_HASH
}

#endif
