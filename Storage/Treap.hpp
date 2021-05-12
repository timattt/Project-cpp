#pragma once

// Includes
#include "stdio.h"
#include "errno.h"
#include "stdlib.h"

// Utilitites
#define TREAP_ELEMENT_TYPE int

// Usage
// Treap
// declarations
#define TREAP treap_p
#define EMPTY_TREAP NULL

// functions
#define NEW_TREAP_VERTEX(X, Y) treapMakeVertex(X, Y)
#define TREAP_SPLIT(TREAP, LEFT, RIGHT, X) treapSplit(TREAP, &(LEFT), &(RIGHT), X)
#define TREAP_MERGE(L, R, RESULT) treapMerge(L, R, &(RESULT))
#define TREAP_INSERT_ELEMENT(TREAP, X) treapRandomInsert(&(TREAP), X)
#define TREAP_INSERT_VERTEX(TREAP, VERT) treapInsertVertex(&(TREAP), VERT)
#define TREAP_ERASE(TREAP, X) treapErase(&(TREAP), X)
#define TREAP_CONTAINS(TREAP, X) treapContains(TREAP, X)
#define TREAP_PRINT(TREAP) treapPrintElements(TREAP)
#define TREAP_GET_COUNT(TREAP, POS) treapGetCount(TREAP, POS)
#define TREAP_DELETE(TREAP) treapDelete(&(TREAP))
#define TREAP_FOR_EACH(TREAP, CONS, DATA) treapForEach(TREAP, CONS, DATA)


// Implicit treap
// declarations
#define ITREAP itreap_p
#define EMPTY_ITREAP NULL

// functions
#define NEW_ITREAP_VERTEX(X, Y) NEW_TREAP_VERTEX(X, Y)
#define ITREAP_SPLIT(TREAP, LEFT, RIGHT, POS) itreapSplit(TREAP, &(LEFT), &(RIGHT), POS)
#define ITREAP_MERGE(L, R, RESULT) TREAP_MERGE(L, R, RESULT)
#define ITREAP_INSERT_VERTEX(TREAP, X, POS) itreapInsertVertex(&(TREAP), X, POS)
#define ITREAP_INSERT_ELEMENT(TREAP, X, POS) itreapRandomInsert(&(TREAP), X, POS)
#define ITREAP_ERASE(TREAP, POS) itreapErase(&(TREAP), POS)
#define ITREAP_GET(TREAP, POS) treapGetCount(TREAP, POS)
#define ITREAP_PRINT(TREAP) TREAP_PRINT(TREAP)
#define ITREAP_DELETE(TREAP) treapDelete(&(TREAP))
#define ITREAP_FOR_EACH(TREAP, CONS, DATA) TREAP_FOR_EACH(TREAP, CONS, DATA)

// Declaration
typedef struct treap* treap_p;
typedef struct treap* itreap_p;

struct treap;

// Prototypes
//! Gives size of tree that represents this vertex.
//! If parameter is NULL then 0 is returned.
int treapSizeOf(treap_p tr);
//! Here comes updates for all treap data.
//! Update goes only for this vertex. Not for the whole subtree.
//! If treap is NULL then nothing happens.
//! Returns -1 if error happens.
int treapUpdate(treap_p t);
//! Here goes update for special data such as reverse.
//! If treap is NULL then nothing happens.
//! Returns -1 if error happens.
int treapPush(treap_p t);
//! This function splits given treap into two where in will be everything <= x and in second 
//! everything will be > x
//! If error occures -1 is returned and errno is set.
int treapSplit(treap_p tr, treap_p * l, treap_p * r, TREAP_ELEMENT_TYPE x);
//! Merges two treaps into one. If tr is NULL then -1 is returned.
int treapMerge(treap_p l, treap_p r, treap_p * tr);
//! Inserts ONE vertex into given treap. If treap is NULL -1 is returned.
int treapInsertVertex(treap_p * tr, treap_p ins);
//! Erases ONE vertex with given element. If treap is NULL -1 is returned.
int treapErase(treap_p * tr, TREAP_ELEMENT_TYPE x);
//! Gives i element in sorted order.
//! If there is no such element then -1 is returned.
TREAP_ELEMENT_TYPE treapGetCount(treap_p tr, int pos);
//! Prints all treap elements into stdio in sorted order.
int treapPrintElements(treap_p tr);
//! Creates new vertex.
//! if error occured then -1 is returned and errno is set.
treap_p treapMakeVertex(TREAP_ELEMENT_TYPE x, TREAP_ELEMENT_TYPE y);
//! If this treap contains given element then returns 1, else returns 0.
int treapContains(treap_p tr, TREAP_ELEMENT_TYPE x);
//! Simple information function for treap.
int treapDfs(treap_p tr);
//! Inserts element into treap. And generates second element for pair randomly.
//! This function will return -1 and EPERM if this element is already in this treap.
//! But multiple insertion is actually posible, but not with this function. 
int treapRandomInsert(treap_p * tr, TREAP_ELEMENT_TYPE x);
//! Splits this IMPLICIT treap into two by position.
//! So in l will be [0, pos). And in r will be [pos, n)
//! If l or r is NULL then -1 is returned.
int itreapSplit(itreap_p tr, itreap_p * l, itreap_p * r, int pos);
//! Inserts vertex into IMPLICIT treap so it will have given position.
//! If tr is NULL -1 is returned.
int itreapInsertVertex(itreap_p * tr, itreap_p ins, int pos);
//! Erases vertex from given position.
//! if tr is NULL -1 is returned.
int itreapErase(itreap_p * tr, int pos);
//! Inserts element and generates vertex randomly.
int itreapRandomInsert(itreap_p * tr, TREAP_ELEMENT_TYPE x, int pos);
//! Recursevely deletes this treap.
//! returns -1 if something bad happens.
int treapDelete(itreap_p * tr);
//! Goes through all elements in this collection and calls consumer function.
int treapForEach(treap_p tr, void (*consumer)(treap_p, TREAP_ELEMENT_TYPE, void * data), void * data);
