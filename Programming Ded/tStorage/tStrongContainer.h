#ifndef T_STRONGCONTAINER
#define T_STRONGCONTAINER

#include "tContainer.h"

namespace tStorage {

template<typename T, unsigned size, unsigned (*hash)(char*,
		unsigned) = tDefaultHash> class tStrongContainer {

private:
	tContainer<T, size, hash> core;
	tContainer<T, size, hash> copy;

	//! Same as std assert.
	void tAssert(bool val) {
		if (!val) {
			tThrowException();
		}
	}

	//! Just throws exception and tells some information about this class.
	void tThrowException() {
		throw "tStrongContainer broken! Printing info: "
				+ (std::string) (tToString());
	}

	//! Checks core to be valid. If not then tThrowException function is used.
	void tCheckValid() {
		if (!core.tIsValid()) {
			tThrowException();
		}
	}

	//! Between preSave and postSave functions any changes for core can be undone.
	void preSave() {
		tCheckValid();
		core.tCopyTo(copy);
	}

	//! Between preSave and postSave functions any changes for core can be undone.
	void postSave() {
		if (!core.tIsValid()) {
			copy.tCopyTo(core);
			tThrowException();
		}
	}

protected:

public:
	//! Gives information about this container.
	const char* tToString() {
		return core.tToString();
	}

	tStrongContainer() :
			core( { }), copy( { }) {
	}

	//!Inserts element to given index.
	void tInsert(const T &element, unsigned index) {
		tAssert(index < size);
		preSave();
		core.tWriteTo_e(index, element);
		postSave();
	}

	//! Gives element from given index.
	const T& tGet(unsigned index) {
		tAssert(index < size);
		preSave();
		const T &result = core.tGetFrom_e(index);
		postSave();

		return result;
	}

	//! Like for each loop.
	void tForEach(void (*consumer)(const T&)) {
		tAssert(consumer != NULL);
		preSave();
		core.tForEach(consumer);
		postSave();
	}

	//!Cleans memeory from from index to to index.
	void tCleanMemory(unsigned from, unsigned to) {
		tAssert(from < to && from < size && to < size);
		preSave();
		core.tCleanMemoryFromTo_b(from, to);
		postSave();
	}

};

}

#endif
