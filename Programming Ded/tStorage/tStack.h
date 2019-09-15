#ifndef T_STACK
#define T_STACK

#include "tContainer.h"

namespace tStorage {

/*
 *	Author timattt
 */

//!Stack created by timattt
//!It is armed with memory protection. Hash is always checked.
//!
template<typename T, int size, int (*hash)(char*, int) = tDefaultHash> class tStack: private tContainer<
		T, size, (*hash)> {
private:
	int total_objects = 0;
	using tContainer<T, size, (*hash)>::tWriteTo;
	using tContainer<T, size, (*hash)>::tGetFrom;
public:
	using tContainer<T, size, (*hash)>::tSeeBits;
	//!Inserts object on the top of this stack.
	void tPush(const T &el) {
		assert(total_objects + 1 != size);
		tWriteTo(total_objects, el);
		total_objects++;
	}

	//!Delete object from top.
	T tPop() {
		assert(total_objects != 0);
		total_objects--;
		T res = tGetFrom(total_objects);
		return res;
	}

	//!Returns quantity of objects in this stack.
	int tGetSize() {
		return total_objects;
	}
};
}

#endif
