#include "rbt.h"
#include "bst.h"
#include <assert.h>
#include <stdlib.h>
#define BLACK 1
#define RED 0
#define LINEAR 1
#define NOT_LINEAR 0

/*
 * Written by Zach Wassynger on October 18th, 2017. Contains the implementation for
 * a red-black tree that utilizes an underlying BST. Also contains the implementation for
 * a RBTVALUE that holds color, frequency, display and comparator functions, and the generic
 * value. This is stored in the BSTNODE, which is stored in the BST.
 */

typedef struct rbtvalue {
	void *value;
	int freq, color;
	void (*display)(FILE *, void *);
	int (*compare)(void *, void *);
} RBTVALUE;
static RBTVALUE *newRBTVALUE(void *v, void (*d)(FILE *, void *), int (*c)(void *, void *)) {
	RBTVALUE *p = malloc(sizeof(RBTVALUE));
	assert(p != 0);

	p->value = v;
	p->freq = 1;
	p->color = 0;
	p->display = d;
	p->compare = c;
	return p;
}
static int getColor(BSTNODE *node) {
	if(node == NULL)
		return BLACK;
	RBTVALUE *value = getBSTNODE(node);
	return value->color;
}
static void setColor(BSTNODE *node, int c) {
	if(node == NULL)
		return;
	RBTVALUE *value = getBSTNODE(node);
	value->color = c;
}

static void insertionFixup(BST *t, BSTNODE *node);
static void deletionFixup(BST *t, BSTNODE *node);
static BSTNODE *findUncle(BSTNODE *node);
static BSTNODE *findSibling(BSTNODE *node);
static BSTNODE *findNephew(BSTNODE *node);
static BSTNODE *findNiece(BSTNODE *node);
static int checkLinear(BSTNODE *parent, BSTNODE *child);
static void rotate(BST *t, BSTNODE *parent, BSTNODE *child);
static void rotateLeft(BST *t, BSTNODE *node);
static void rotateRight(BST *t, BSTNODE *node);
//static void debugRBTVALUE(BSTNODE *node, char *name);
static void displayRBTVALUE(FILE *fp, void *v);
static int compareRBTVALUE(void *x, void *y);
static void swapRBTVALUE(BSTNODE *x, BSTNODE *y);

struct rbt {
	BST *tree;
	int numWords;
	void (*display)(FILE *, void *);
	int (*compare)(void *, void *);
};

RBT *newRBT(void (*d)(FILE *, void *), int (*c)(void *, void *)) {
	RBT *rb = malloc(sizeof(RBT));
	assert(rb != 0);

	rb->tree = newBST(displayRBTVALUE, compareRBTVALUE, swapRBTVALUE);
	rb->numWords = 0;
	rb->display = d;
	rb->compare = c;
	return rb;
}
void insertRBT(RBT *rbt, void *v) {
	RBTVALUE *value = newRBTVALUE(v, rbt->display, rbt->compare);
	BSTNODE *node = findBST(rbt->tree, value);
	if(node == NULL) {
		node = insertBST(rbt->tree, value);
		insertionFixup(rbt->tree, node);
	}
	else {
		//If the word is already in the tree
		value = getBSTNODE(node);
		value->freq++;
	}
	rbt->numWords++;
}
int findRBT(RBT *rbt, void *v) {
	extern char *getSTRING(void *);
	RBTVALUE *value = newRBTVALUE(v, rbt->display, rbt->compare);
	BSTNODE *node = findBST(rbt->tree, value);
	if(node == NULL)
		return 0;
	value = getBSTNODE(node);
	return value->freq;
}
void deleteRBT(RBT *rbt, void *v) {
	RBTVALUE *value = newRBTVALUE(v, rbt->display, rbt->compare);
	BSTNODE *node = findBST(rbt->tree, value);
	if(node != NULL) {
		value = getBSTNODE(node);
		if(value->freq > 1)
			value->freq--;
		else {
			//When there is only 1 of the phrase - remove node
			node = swapToLeafBST(rbt->tree, node);
			deletionFixup(rbt->tree, node);
			pruneLeafBST(rbt->tree, node);
		}
		rbt->numWords--;
	} else {
		printf("Value ");
		value->display(stdout, value->value);
		printf(" not found.\n");
	}
}
int sizeRBT(RBT *rbt) {
	return sizeBST(rbt->tree);
}
int wordsRBT(RBT *rbt) {
	return rbt->numWords;
}
void statisticsRBT(FILE *fp, RBT *rbt) {
	fprintf(fp, "Words/Phrases: %d\n", wordsRBT(rbt));
	statisticsBST(fp, rbt->tree);
}
void displayRBT(FILE *fp, RBT *rbt) {
	displayBST(fp, rbt->tree);
}

/*
 * Called after a node is inserted. Follows the "Best Red-Black Tree
 * pseudocode" found on beastie.cs.ua.edu/red-black
 */
static void insertionFixup(BST *t, BSTNODE *node) {
	int exitLoop = 0;
	while(!exitLoop) {
		//test if node is the root
		if(node == getBSTroot(t))
			exitLoop = 1;
		else {
			//Check if parent is black
			BSTNODE *parent = getBSTNODEparent(node);
			BSTNODE *grandparent = getBSTNODEparent(parent);
			BSTNODE *uncle = findUncle(node);
			if(getColor(parent) == BLACK)
				exitLoop = 1;
			else if(getColor(uncle) == RED) {
				setColor(parent, BLACK);
				setColor(uncle, BLACK);
				setColor(grandparent, RED);
				node = grandparent;
			} else {
				if(checkLinear(parent, node) == NOT_LINEAR) {
					rotate(t, parent, node);
					BSTNODE *oldparent = parent;
					parent = node;
					node = oldparent;
				}
				setColor(parent, BLACK);
				setColor(grandparent, RED);
				rotate(t, grandparent, parent);
				exitLoop = 1;
			}
		}
	}
	//Find root and set color to black
	setColor(getBSTroot(t), BLACK);
}
/*
 * Finds the sibling of node's parent.
 */
static BSTNODE *findUncle(BSTNODE *node) {
	if(node == NULL)
		return NULL;
	BSTNODE *parent = getBSTNODEparent(node);
	if(parent == NULL)
		return NULL;
	BSTNODE *grandparent = getBSTNODEparent(parent);
	if(grandparent == NULL)
		return NULL;

	if(parent == getBSTNODEleft(grandparent))
		return getBSTNODEright(grandparent);
	else
		return getBSTNODEleft(grandparent);
}
/*
 * Finds the other child of node's parent.
 */
static BSTNODE *findSibling(BSTNODE *node) {
	if(node == NULL)
		return NULL;
	BSTNODE *parent = getBSTNODEparent(node);
	if(parent == NULL)
		return NULL;

	if(node == getBSTNODEleft(parent))
		return getBSTNODEright(parent);
	else
		return getBSTNODEleft(parent);
}
/*
 * Returns the far child of the sibling of the node.
 * e.g. if node is a left child, the nephew would be the right child of the sibling.
 */
static BSTNODE *findNephew(BSTNODE *node) {
	if(node == NULL)
		return NULL;
	BSTNODE *parent = getBSTNODEparent(node);
	BSTNODE *sibling = findSibling(node);
	if(parent == NULL || sibling == NULL)
		return NULL;

	if(node == getBSTNODEleft(parent))
		return getBSTNODEright(sibling);
	return getBSTNODEleft(sibling);
}
/*
 * Returns the close child of the sibling of the node.
 * e.g. if node is a left child, the niece would be the left child of the sibling.
 */
static BSTNODE *findNiece(BSTNODE *node) {
	if(node == NULL)
		return NULL;
	BSTNODE *parent = getBSTNODEparent(node);
	BSTNODE *sibling = findSibling(node);
	if(parent == NULL || sibling == NULL)
		return NULL;

	if(node == getBSTNODEright(parent))
		return getBSTNODEright(sibling);
	return getBSTNODEleft(sibling);
}
static int checkLinear(BSTNODE *parent, BSTNODE *child) {
	BSTNODE *grandparent = getBSTNODEparent(parent);
	if(grandparent == NULL)
		return LINEAR;
	if(parent == getBSTNODEleft(grandparent)) {
		if(child == getBSTNODEleft(parent))
			return LINEAR;
	} else {
		if(child == getBSTNODEright(parent))
			return LINEAR;
	}
	return NOT_LINEAR;
}
static void rotate(BST *t, BSTNODE *parent, BSTNODE *child) {
	if(child == getBSTNODEleft(parent))
		rotateRight(t, parent);
	else
		rotateLeft(t, parent);
}
static void rotateLeft(BST *t, BSTNODE *node) {
	BSTNODE *right = getBSTNODEright(node);
	setBSTNODEright(node, getBSTNODEleft(right));
	if(getBSTNODEleft(right) != NULL)
		setBSTNODEparent(getBSTNODEleft(right), node);
	setBSTNODEparent(right, getBSTNODEparent(node));
	if(node == getBSTroot(t)) {
		setBSTroot(t, right);
		setBSTNODEparent(right, right);
	} else if(node == getBSTNODEleft(getBSTNODEparent(node)))
		setBSTNODEleft(getBSTNODEparent(node), right);
	else
		setBSTNODEright(getBSTNODEparent(node), right);
	setBSTNODEleft(right, node);
	setBSTNODEparent(node, right);
}
static void rotateRight(BST *t, BSTNODE *node) {
	BSTNODE *left = getBSTNODEleft(node);
	setBSTNODEleft(node, getBSTNODEright(left));
	if(getBSTNODEright(left) != NULL)
		setBSTNODEparent(getBSTNODEright(left), node);
	setBSTNODEparent(left, getBSTNODEparent(node));
	if(node == getBSTroot(t)) {
		setBSTroot(t, left);
		setBSTNODEparent(left, left);
	} else if(node == getBSTNODEleft(getBSTNODEparent(node)))
		setBSTNODEleft(getBSTNODEparent(node), left);
	else
		setBSTNODEright(getBSTNODEparent(node), left);
	setBSTNODEright(left, node);
	setBSTNODEparent(node, left);
}
/*
 * Follows the "Best Red-Black Tree pseudocode" found on beastie.cs.ua.edu/red-black.
 */
static void deletionFixup(BST *t, BSTNODE *node) {
	int exitLoop = 0;
	while(!exitLoop) {
		if(node == getBSTroot(t))
			exitLoop = 1;
		else if(getColor(node) == RED)
			exitLoop = 1;
		else {
			BSTNODE *parent = getBSTNODEparent(node);
			BSTNODE *sibling = findSibling(node);
			if(getColor(sibling) == RED) {
				setColor(parent, RED);
				setColor(sibling, BLACK);
				rotate(t, parent, sibling);
			} else if(getColor(findNephew(node)) == RED) {
				BSTNODE *nephew = findNephew(node);
				setColor(sibling, getColor(parent));
				setColor(nephew, BLACK);
				setColor(parent, BLACK);
				rotate(t, parent, sibling);
				exitLoop = 1;
			} else if(getColor(findNiece(node)) == RED) {
				BSTNODE *niece = findNiece(node);
				setColor(niece, BLACK);
				setColor(sibling, RED);
				rotate(t, sibling, niece);
			} else {
				setColor(sibling, RED);
				node = parent;
			}
		}
	}
	setColor(node, BLACK);
}

/*
static void debugRBTVALUE(BSTNODE *node, char *name) {
	printf("%s: ", name);
	if(node == NULL) {
		printf("NULL node.\n");
		return;
	}
	BSTNODE *otherNode;
	RBTVALUE *val = getBSTNODE(node);
	printf("Value=");
	val->display(stdout,val->value);
	otherNode = getBSTNODEparent(node);
	if(otherNode == NULL)
		printf("\tNull parent");
	else {
		val = getBSTNODE(otherNode);
		printf("\tParent value=");
		val->display(stdout,val->value);
	}
	otherNode = getBSTNODEleft(node);
	if(otherNode == NULL)
		printf("\tNull left child");
	else {
		val = getBSTNODE(otherNode);
		printf("\tLeft child value=");
		val->display(stdout,val->value);
	}
	otherNode = getBSTNODEright(node);
	if(otherNode == NULL)
		printf("\tNull right child");
	else {
		val = getBSTNODE(otherNode);
		printf("\tRight child value=");
		val->display(stdout,val->value);
	}
	printf("\n");
}
*/

static void displayRBTVALUE(FILE *fp, void *v) {
	RBTVALUE *value = v;
	value->display(fp, value->value);
	if(value->freq > 1)
		fprintf(fp, "-%d", value->freq);
	if(value->color == BLACK)
		fprintf(fp, "-B");
	else
		fprintf(fp, "-R");
}
static int compareRBTVALUE(void *x, void *y) {
	RBTVALUE *p = x, *q = y;
	return p->compare(p->value, q->value);
}
/*
 * Swaps the frequency and value of two RBTVALUEs contained in their respective
 * BSTNODEs. Node pointers and color remain unaffected.
 */
static void swapRBTVALUE(BSTNODE *x, BSTNODE *y) {
	RBTVALUE *v1 = getBSTNODE(x), *v2 = getBSTNODE(y);

	void *tempVal = v1->value;
	v1->value = v2->value;
	v2->value = tempVal;

	int tempFreq = v1->freq;
	v1->freq = v2->freq;
	v2->freq = tempFreq;
}
