#pragma once

#include "../Storage/Container.hpp"

template<typename T, unsigned size, unsigned (*hash)(char*,
		unsigned) = tDefaultHash> class tStrongContainer {

private:
	tContainer<T, size, hash> core;
	tContainer<T, size, hash> copy;

	//! Checks core to be valid. If not then tThrowException function is used.
	void tCheckValid() {
		CHECK (!core.tIsValid(), "Not valid!");
	}

	//! Between tPreSave and tPostSave functions any changes for core can be undone.
	void tPreSave() {
		tCheckValid();
		core.tCopyTo(copy);
	}

	//! Between tPreSave and tPostSave functions any changes for core can be undone.
	void tPostSave() {
		if (!core.tIsValid()) {
			copy.tCopyTo(core);
			THROW_T_EXCEPTION("Bad post save!");
		}
	}

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
		ENSURE(index < size, "Bad index");
		tPreSave();
		core.tWriteTo_e(index, element);
		tPostSave();
	}

	//! Gives element from given index.
	T& tGet(unsigned index) {
		ENSURE(index < size, "Bad index");
		tPreSave();

		T &result = core.tGetFrom_e(index);
		tPostSave();

		return result;
	}

	//! Like for each loop.
	void tForEach(void (*consumer)(const T&)) {
		tAssert(consumer != NULL);
		tPreSave();
		core.tForEach(consumer);
		tPostSave();
	}

	//!Cleans memeory from from index to to index.
	void tCleanMemory(unsigned from, unsigned to) {
		ENSURE(from < to && from < size && to < size, "Bad interval");
		tPreSave();
		core.tCleanMemoryFromTo_b(from, to);
		tPostSave();
	}

};