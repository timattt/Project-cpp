#ifndef TREAP
#define TREAP

#include <bits/stdc++.h>
#include "../tUtilities/tString.h"

using namespace tUtilities;
using namespace std;

namespace tStorage {

tList<int> free_priors = { };
unsigned free = 0;

void gen_free_priors(unsigned cap) {
	int *arr = new int[cap];
	for (unsigned i = 0; i < cap; i++) {
		arr[i] = free + i;
	}
	for (unsigned i = 0; i < cap; i++) {
		unsigned a = rand() % cap;
		unsigned b = rand() % cap;
		tSwap(arr[a], arr[b]);
	}

	for (unsigned i = 0; i < cap; i++) {
		free_priors.tAddLast(arr[i]);
	}

	free += cap;
	delete arr;
}

int free_prior() {
	if (free_priors.tGetSize() == 0) {
		gen_free_priors(10000);
	}
	return free_priors.tRemoveLast();
}

}

struct tTreap {
	int x;
	int y;

	tTreap *left;
	tTreap *right;

	int size;

	tTreap(int x_, int y_) :
			x(x_), y(y_), left(NULL), right(NULL), size(0) {
	}

	tTreap(int x_) :
			x(x_), y(tStorage::free_prior()), left(NULL), right(NULL), size(0) {
	}
};

int SizeOf(tTreap *t) {
	return (t == NULL ? 0 : t->size);
}

void update(tTreap *t) {
	if (t == NULL) {
		return;
	}

	t->size = 1 + SizeOf(t->left) + SizeOf(t->right);
}

// [0, x] and (x, size)
void split(tTreap *tr, tTreap *&l, tTreap *&r, int x) {
	if (!tr) {
		l = r = NULL;
	} else if (x < tr->x) {
		split(tr->left, l, tr->left, x);
		r = tr;
	} else {
		split(tr->right, tr->right, r, x);
		l = tr;
	}

	update(l);
	update(r);
}

// ALL l.x < ALL r.x
void merge(tTreap *l, tTreap *r, tTreap *&tr) {
	if (!l || !r) {
		tr = (!l ? r : l);
	} else if (l->y > r->y) {
		merge(l->right, r, l->right);
		tr = l;
	} else {
		merge(l, r->left, r->left);
		tr = r;
	}

	update(tr);
}

void insert(tTreap *&tr, tTreap *ins) {
	if (!tr) {
		tr = ins;
	} else if (ins->y > tr->y) {
		split(tr, ins->left, ins->right, ins->x);
		tr = ins;
	} else if (ins->x < tr->x) {
		insert(tr->left, ins);
	} else {
		insert(tr->right, ins);
	}

	update(tr);
}

tTreap* tKThFromStart(tTreap *tr, unsigned i) {
	if (!tr) {
		return NULL;
	}
	unsigned sz = SizeOf(tr->left);
	if (sz == i) {
		return tr;
	}
	if (i < sz) {
		return tKThFromStart(tr->left, i);
	} else {
		return tKThFromStart(tr->right, i - sz - 1);
	}
}


// [0, pos) and [pos, size)
void split_imp(tTreap *tr, tTreap *&l, tTreap *&r, unsigned pos) {
	if (!tr) {
		l = r = NULL;
	} else {
		unsigned cur = SizeOf(tr->left) + 1;

		if (cur <= pos) {
			split_imp(tr->right, tr->right, r, pos - cur);
			l = tr;
		} else {
			split_imp(tr->left, l, tr->left, pos);
			r = tr;
		}
	}

	update(l);
	update(r);
}

void insert_imp(tTreap *&tr, tTreap *ins, unsigned pos) {
	tTreap *l = NULL;
	tTreap *r = NULL;

	split_imp(tr, l, r, pos);

	merge(l, ins, l);
	merge(l, r, tr);
}

tString tToString(tTreap *tr) {
	tString res = { };

	res += "TREAP: tGetSize: ";
	res += tString(SizeOf(tr));
	res += " [";

	for (unsigned i = 0, sz = SizeOf(tr); i < sz; i++) {
		tTreap *cur = tKThFromStart(tr, i);
		res += '(';
		res += tString(cur->x);
		res += ", ";
		res += tString(cur->y);
		res += ')';
		if (i + 1 < sz) {
			res += ", ";
		}
	}

	res += ']';

	return res;
}

void erase_imp(tTreap *&tr, unsigned pos) {
	tTreap *l = NULL;
	tTreap *r = NULL;

	split_imp(tr, l, r, pos);

	tTreap *l1 = NULL;
	tTreap *r1 = NULL;

	split_imp(r, l1, r1, 1);

	merge(l, r1, tr);
}

void erase(tTreap *&tr, int x) {
	if (!tr) {
		tThrowException("NO SUCH ELEMENT IN TREAP!");
	}
	if (tr->x == x) {
		merge(tr->left, tr->right, tr);
	} else if (x < tr->x) {
		erase(tr->left, x);
	} else {
		erase(tr->right, x);
	}

	update(tr);
}

#endif
