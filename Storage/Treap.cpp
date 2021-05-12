#include "Treap.hpp"

struct treap {
	// X
	TREAP_ELEMENT_TYPE x;
	// Y
	TREAP_ELEMENT_TYPE y;
	
	// This treap size
	int size;
	
	treap_p left;
	treap_p right;	
};
 
int treapSizeOf(treap_p tr) {
	if (tr == NULL) {
		return 0;
	} else {
		return tr->size;
	}
}

int treapUpdate(treap_p t) {
	if (t) {
		t->size = 1 + treapSizeOf(t->left) + treapSizeOf(t->right);
		// Here any sub treap data may be updated
	}
	
	return 0;
}

int treapPush(treap_p t) {
	if (t) {
		// Update here all multiple effects such as reverse.
	}
	
	return 0;
}

int treapSplit(treap_p tr, treap_p * l, treap_p * r, TREAP_ELEMENT_TYPE x) {
	if (!l || !r) {
		errno = EFAULT;
		return -1;		
	}

	treapPush(tr);
	
	if (!tr) {
		*r = *l = NULL;
	} else if (x < tr->x) {
		treapSplit(tr->left, l, &tr->left, x);
		*r = tr;
	} else {
		treapSplit(tr->right, &tr->right, r, x);
		*l = tr;
	}
	
	treapUpdate(tr);
	
	errno = 0;
	return 0;
}

int treapMerge(treap_p l, treap_p r, treap_p * tr) {
	if (!tr) {
		errno = EFAULT;
		return -1;
	}
	
	treapPush(l);
	treapPush(r);
	
	if (!r || !l) {
		if (!l) {
			*tr = r;
		} else {
			*tr = l;
		}
	} else if (l->y < r->y) {
		treapMerge(l, r->left, &r->left);
		*tr = r;
	} else {
		treapMerge(l->right, r, &l->right);
		*tr = l;
	}
	
	treapUpdate(*tr);
	
	errno = 0;
	return 0;
}

int treapInsertVertex(treap_p * tr, treap_p ins) {
	if (!tr || !ins) {
		errno = EFAULT;
		return -1;
	}

	treapPush(*tr);
	
	if (!(*tr)) {
		*tr = ins;
	} else if (ins->y < (*tr)->y) {
		if (ins->x < (*tr)->x) {
			treapInsertVertex(&(*tr)->left, ins);
		} else {
			treapInsertVertex(&(*tr)->right, ins);
		}
	} else {
		treapSplit(*tr, &ins->left, &ins->right, ins->x);
		*tr = ins;
	}	

	treapUpdate(*tr);
	
	return 0;
}

int treapErase(treap_p * tr, TREAP_ELEMENT_TYPE x) {
	if (!tr) {
		errno = EFAULT;
		return -1;
	}
	if (!(*tr)) {
		return 0;
	}
	if ((*tr)->x == x) {
		treap_p trash = *tr;
		treapMerge((*tr)->left, (*tr)->right, tr);
		trash->left = trash->right = NULL;
		treapDelete(&trash);
	} else if (x < (*tr)->x) {
		treapErase(&(*tr)->left, x);
	} else {
		treapErase(&(*tr)->right, x);
	}
	
	treapUpdate(*tr);
	
	return 0;
}

TREAP_ELEMENT_TYPE treapGetCount(treap_p tr, int pos) {
	treapPush(tr);
	treapUpdate(tr);
	if (!tr) {
		errno = 0;
		return -1;
	}
	int cur = 0;
	cur = treapSizeOf(tr->left);
	if (pos == cur) {
		return tr->x;
	}
	if (pos < cur) {
		return treapGetCount(tr->left, pos);
	} else {
		return treapGetCount(tr->right, pos - cur - 1);
	}
}

int treapPrintElements(treap_p tr) {
	printf("Treap: [");
	for (int i = 0, size = treapSizeOf(tr); i < size; i++) {
		int val = 0;
		val = treapGetCount(tr, i);
		printf("%d", val);
		if (i + 1 < size) {
			printf(", ");
		}
	}
	printf("]\n");
	
	return 0;
}

treap_p treapMakeVertex(TREAP_ELEMENT_TYPE x, TREAP_ELEMENT_TYPE y) {
	treap_p result = (treap_p)calloc(sizeof(struct treap), 1);
	if (result == NULL) {
		return (treap_p)-1;
	}
	result->x = x;
	result->y = y;
	return result;
}

int treapContains(treap_p tr, TREAP_ELEMENT_TYPE x) {	
	if (!tr) {
		return 0;
	}
	treapUpdate(tr);
	treapPush(tr);
	
	int res = 0;
	if (x == tr->x) {
		return 1;
	}
	if (x < tr->x) {
		res = treapContains(tr->left, x);
	} else {
		res = treapContains(tr->right, x);
	}
	
	return res;
}

int treapDfs(treap_p tr) {
	if (!tr) {
		return 0;
	}
	printf("{\n");
	printf("\t[%d, %d] size: %d\n", tr->x, tr->y, tr->size);
	if (tr->left)
		printf("\tl: [%d, %d] -> [%d, %d]\n", tr->x, tr->y, tr->left->x, tr->left->y);
	if (tr->right)
		printf("\tr: [%d, %d] -> [%d, %d]\n", tr->x, tr->y, tr->right->x, tr->right->y);
	printf("}\n");
	treapDfs(tr->left);
	treapDfs(tr->right);
	return 0;
}

int treapRandomInsert(treap_p * tr, TREAP_ELEMENT_TYPE x) {
	if (!tr) {
		errno = EFAULT;
		return -1;
	}
	TREAP_ELEMENT_TYPE randy = rand();
	if (treapContains(*tr, x) > 0) {
		errno = EPERM; 
		return -1;
	} else {
		treap_p ins = treapMakeVertex(x, randy);
		if (ins == (treap_p)-1) {
			return -1;
		}
		treapInsertVertex(tr, ins);
	}
	return 0;
}

int itreapSplit(itreap_p tr, itreap_p * l, itreap_p * r, int pos) {
	if (!l || !r) {
		errno = EFAULT;
		return -1;
	}
	treapPush(tr);
	if (!tr) {
		*r = *l = NULL;
	} else {
		int cur = treapSizeOf(tr->left) + 1;
		if (pos < cur) {
			itreapSplit(tr->left, l, &(tr->left), pos);
			*r = tr;
		} else {
			itreapSplit(tr->right, &(tr->right), r, pos - cur);
			*l = tr;
		}
	}
	
	treapUpdate(tr);
	return 0;
}

int itreapInsertVertex(itreap_p * tr, itreap_p ins, int pos) {
	if (!tr || !ins) {
		errno = EFAULT;
		return -1;
	}
	itreap_p l = NULL;
	itreap_p r = NULL;
	itreapSplit(*tr, &l, &r, pos);
	treapMerge(l, ins, &l);
	treapMerge(l, r, tr);
	return 0;
}

int itreapErase(itreap_p * tr, int pos) {
	if (!tr) {
		errno = EFAULT;
		return -1;
	}

	itreap_p l = NULL;
	itreap_p r = NULL;
	
	itreapSplit(*tr, &l, &r, pos);
	
	itreap_p l1 = NULL;
	itreap_p r1 = NULL;
	
	itreapSplit(r, &l1, &r1, 1);
	treapMerge(l, r1, tr);
	
	treapDelete(&l1);
	
	return 0;
}

int itreapRandomInsert(itreap_p * tr, TREAP_ELEMENT_TYPE x, int pos) {
	if (!tr) {
		errno = EFAULT;
		return -1;
	}
	TREAP_ELEMENT_TYPE randy = rand();
	treap_p ins = treapMakeVertex(x, randy);
	if (ins == (treap_p)-1) {
		return -1;
	}
	itreapInsertVertex(tr, ins, pos);
	return 0;
}

int treapDelete(treap_p * tr) {
	if (!tr) {
		errno = EFAULT;
		return -1;
	}
	if (!(*tr)) {
		return 0;
	}
	treap_p l = (*tr)->left;
	treap_p r = (*tr)->right;
	free(*tr);
	
	treapDelete(&l);
	treapDelete(&r);
	*tr = NULL;
	return 0;
}

int treapForEach(treap_p tr, void (*consumer)(treap_p, TREAP_ELEMENT_TYPE, void*data), void*data) {
	if (!consumer) {
		errno = EFAULT;
		return -1;
	}
	for (int i = 0, size = treapSizeOf(tr); i < size; i++) {
		TREAP_ELEMENT_TYPE x = 0;
		x = treapGetCount(tr, i);
		consumer(tr, x, data);
	}
	
	return 0;
}


