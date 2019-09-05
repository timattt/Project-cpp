#include <bits/stdc++.h>
using namespace std;

void* operator new(size_t s, char *p) {
	return p;
}

const int total_canaries = 2;
const int canaries_value = 111;

template<typename T, int size> struct tStack {
	char mem[size * sizeof(T) + 2 * total_canaries];
	char *begin;
	char *end;
	char *current;

	tStack() :
			begin(mem), end(mem + size * sizeof(T) + total_canaries), current(
					mem) {
		for (int i = 0; i < total_canaries; i++) {
			mem[i] = canaries_value;
			mem[i + size * sizeof(T) + total_canaries] = canaries_value;
		}
	}

	bool checkCanaries() {
		bool result = 0;

		for (int i = 0; i < total_canaries; i++) {
			if (mem[i] != canaries_value
					|| mem[i + size * sizeof(T) + total_canaries]
							!= canaries_value) {
				result = 1;
			}
		}

		if (result) {
			cerr << "Memory broken through!";
			assert(false);
		}

		return result;
	}

	void push(const T &el) {
		checkCanaries();
		current = (char*) new (current) T(el);
		current += sizeof(T);
	}

	T pop() {
		checkCanaries();
		T res = (*(T*) current);
		current -= sizeof(T);
		return res;
	}
};

