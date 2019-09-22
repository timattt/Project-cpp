#ifndef T_STACK
#define T_STACK

#include "tStrongContainer.h"

namespace tStorage {

/*
 *	Author timattt
 */

//!Stack created by timattt
//!It is armed with memory protection. Hash is always checked.
//!
template<typename T, unsigned size, unsigned (*hash)(char*,
		unsigned) = tDefaultHash>
class tStack: private tStrongContainer<T, size, (*hash)> {
private:
	unsigned total_objects = 0;
	using tStrongContainer<T, size, (*hash)>::tInsert;
	using tStrongContainer<T, size, (*hash)>::tGet;
public:
	using tStrongContainer<T, size, (*hash)>::tToString;
	//!Inserts object on the top of this stack.
	void tPush(const T &el) {
		assert(total_objects != size);
		tInsert(el, total_objects);
		total_objects++;
	}

	//!Delete object from top.
	T tPop() {
		assert(total_objects != 0);
		total_objects--;
		T res = tGet(total_objects);
		return res;
	}

	//!Returns quantity of objects in this stack.
	unsigned tGetSize() {
		return total_objects;
	}

	//!Invokes consumer function for every element in this stack.
	void tForEach(void (*consumer)(const T&)) {
		for (int i = total_objects - 1; i != -1; i--) {
			consumer(this->tGet(i));
		}
	}

	//! To make this stack beautiful. This thing just invokes tPop() function.
	void operator>>(T &el) {
		el = tPop();
	}

	//! To make this stack beautiful. This thing just invokes tPush(el) function.
	void operator<<(const T &el) {
		tPush(el);
	}
};
}

#endif
