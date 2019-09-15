#include "tContainer.h"

namespace tStorage {

/*
 *	Author timattt
 */

//!Stack created by timattt
//!It is armed with simple memory protection.
//!
template<typename T, int size, int (*hash)(char*, int) = tDefaultHash> class tStack: public tContainer<
		T, size, (*hash)> {

private:
	int total_objects = 0;
	using tContainer<T, size, (*hash)>::tWriteTo;
	using tContainer<T, size, (*hash)>::tGetFrom;
public:

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

	int tGetSize() {
		return total_objects;
	}
};
}
