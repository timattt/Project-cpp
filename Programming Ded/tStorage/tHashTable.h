#ifndef T_HASH_TABLE
#define T_HASH_TABLE

#include "tList.h"
#include "../tUtilities/tFileHandler.h"

using namespace tFileHandler;

namespace tStorage {

template<unsigned size, unsigned (*hash)(const char*, unsigned)> class tHashTable {

	tList<tPair<const char*, unsigned>> core[size];

public:

	//! Puts element into this hash table. You can give length to byte array. But if not then tStrlen
	//! function will be used to determine it.
	void tAdd(const char *elem, unsigned sz) {
		unsigned hs = hash(elem, sz);
		core[hs % size].tAddLast( { elem, sz });
	}

	//!Gives csv information and stores it into given file.
	void tGenerateCSV(tFile *dest) {
		dest->tStartMapping(size * 20);

		dest->tWriteLine("ListNum\tListLen");
		for (unsigned i = 0; i < size; i++) {
			dest->tWriteNum<unsigned>(i);
			dest->tWrite("\t");
			dest->tWriteNum<unsigned>(core[i].tGetSize());
			dest->tWritec('\n');
		}

		dest->tStopMapping();
	}

};

}

#endif
