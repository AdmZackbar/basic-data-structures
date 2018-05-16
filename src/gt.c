#include "gt.h"
#include "bst.h"
#include <assert.h>
#include <stdlib.h>
//#include "string.h"//TODO remove

/*
 * Written by Zach Wassynger on October 19th, 2017. Contains the implementation
 * for a green tree, a BST that efficiently stores duplicates. There is also
 * implementation for a GTVALUE, which stores its generic value, display and
 * comparator functions, and frequency.
 */

typedef struct gtvalue {
	void *value;
	int freq;
	void (*display)(FILE *,void *);
	int (*compare)(void *,void *);
} GTVALUE;
static GTVALUE *newGTVALUE(void *value,
		void (*d)(FILE *,void *), int (*c)(void *,void *)) {
	GTVALUE *p = malloc(sizeof(GTVALUE));
	assert(p != 0);

	p->value = value;
	p->freq = 1;
	p->display = d;
	p->compare = c;
	return p;
}

struct gt {
	BST *tree;
	int numWords;
	void (*display)(FILE *,void *);
	int (*compare)(void *,void *);
};

static void displayGTVALUE(FILE *fp, void *value);
static int compareGTVALUE(void *x, void *y);

GT *newGT(void (*d)(FILE *,void *),int (*c)(void *,void *)) {
	GT *p = malloc(sizeof(GT));
	assert(p != 0);

	p->tree = newBST(displayGTVALUE, compareGTVALUE, NULL);
	p->numWords = 0;
	p->display = d;
	p->compare = c;
	return p;
}
void insertGT(GT *gt, void *value) {
	GTVALUE *v = newGTVALUE(value, gt->display, gt->compare);
	BSTNODE *node = findBST(gt->tree, v);
	if(node == NULL)
		node = insertBST(gt->tree, v);
	else {
		v = getBSTNODE(node);
		v->freq++;
	}
	gt->numWords++;
}
int findGT(GT *gt, void *v) {
	BSTNODE *node = findBST(gt->tree, newGTVALUE(v, gt->display, gt->compare));
	if(node == NULL)
		return 0;
	GTVALUE *value = getBSTNODE(node);
	return value->freq;
}
void deleteGT(GT *gt, void *value) {
	GTVALUE *v = newGTVALUE(value, gt->display, gt->compare);
	//printf("before:\n");
	//displayBST(stdout, gt->tree);
	BSTNODE *node = findBST(gt->tree, v);
	if(node != NULL) {
		v = getBSTNODE(node);
		if(v->freq > 1)
			v->freq--;
		else
			node = deleteBST(gt->tree, v);
		gt->numWords--;
	} else {
		printf("Value ");
		v->display(stdout, v->value);
		printf(" not found.\n");
	}
	//printf("after:\n");
	//displayBST(stdout, gt->tree);
	//printf("\n");
}
int sizeGT(GT *gt) {
	return sizeBST(gt->tree);
}
int wordsGT(GT *gt) {
	return gt->numWords;
}
void statisticsGT(FILE *fp, GT *gt) {
	fprintf(fp, "Words/Phrases: %d\n", wordsGT(gt));
	statisticsBST(fp, gt->tree);
}
void displayGT(FILE *fp, GT *gt) {
	displayBST(fp, gt->tree);
}

void displayGTVALUE(FILE *fp, void *value) {
	GTVALUE *v = value;
	v->display(fp, v->value);
	if(v->freq > 1)
		fprintf(fp, "-%d", v->freq);
}
int compareGTVALUE(void *x, void *y) {
	GTVALUE *p = x, *q = y;
	//printf("comparing %s and %s\n", getSTRING(((STRING *)p->value)), getSTRING(((STRING *)q->value)));
	return p->compare(p->value, q->value);
}
