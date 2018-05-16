#include "bst.h"
#include "queue.h"
#include <assert.h>
#include <stdlib.h>

/*
 * Written by Zach Wassynger on October 4th, 2017. Contains implementation for a
 * Binary Search Tree that can hold BSTNODEs.
 * Also contains implementation for the BSTNODE that the BST uses. Has parent, left,
 * and right pointers. Stores its generic value as a void pointer.
 */

struct bstnode {
	void *value;
	BSTNODE *parent, *left, *right;
};

static BSTNODE *newBSTNODE(void *value) {
	BSTNODE *node = malloc(sizeof(BSTNODE));
	assert(node != 0);

	node->value = value;
	node->left = NULL;
	node->right = NULL;
	node->parent = NULL;

	return node;
}

void *getBSTNODE(BSTNODE *node) {
	assert(node != 0);
	return node->value;
}
void setBSTNODE(BSTNODE *node, void *value) {
	assert(node != 0);
	node->value = value;
}
BSTNODE *getBSTNODEleft(BSTNODE *node) {
	assert(node != 0);
	return node->left;
}
void setBSTNODEleft(BSTNODE *node, BSTNODE *left) {
	assert(node != 0);
	node->left = left;
}
BSTNODE *getBSTNODEright(BSTNODE *node) {
	assert(node != 0);
	return node->right;
}
void setBSTNODEright(BSTNODE *node, BSTNODE *right) {
	assert(node != 0);
	node->right = right;
}
BSTNODE *getBSTNODEparent(BSTNODE *node) {
	assert(node != 0);
	return node->parent;
}
void setBSTNODEparent(BSTNODE *node, BSTNODE *parent) {
	assert(node != 0);
	node->parent = parent;
}

/*
 * The generic node swapper used when no swapper function is given.
 */
static void bstSwapper(BSTNODE *x, BSTNODE *y) {
	void *temp = x->value;
	x->value = y->value;
	y->value = temp;
}
static int inorderSizeBST(BSTNODE *node);

struct bst {
	void (*display)(FILE *, void *);
	int (*compare)(void *, void *);
	void (*swap)(BSTNODE *,BSTNODE *);

	BSTNODE *root;
	int size;
};

BST *newBST(void (*d)(FILE *,void *), int (*c)(void *,void *), void (*s)(BSTNODE *,BSTNODE *)) {
	BST *p = (BST *) malloc(sizeof(BST));
	assert(p != 0);

	//Initialize BST fields
	p->display = d;
	p->compare = c;
	p->root = NULL;
	p->size = 0;
	if(s == NULL)
		p->swap = bstSwapper;
	else
		p->swap = s;

	return p;
}
void setBSTroot(BST *bst, BSTNODE *newRoot) {
	newRoot->parent = newRoot;
	bst->root = newRoot;
}
BSTNODE *getBSTroot(BST *bst) {
	return bst->root;
}
BSTNODE *insertBST(BST *bst, void *p) {
	BSTNODE *newNode = newBSTNODE(p);
	if(bst->root == NULL) {
		bst->root = newNode;
		newNode->parent = newNode;
	}
	else {
		int done = 0;
		BSTNODE *temp = bst->root;
		while(done == 0) {
			if(bst->compare(newNode->value,temp->value) <= 0) {
				//When the key is less than temp's key
				if(temp->left == NULL) {
					//Sets the left child of temp to the created node
					temp->left = newNode;
					newNode->parent = temp;
					done = 1;
				} else {
					//Resets at the top with temp being the left child of old temp
					temp = temp->left;
				}
			} else {
				if(temp->right == NULL) {
					temp->right = newNode;
					newNode->parent = temp;
					done = 1;
				} else {
					temp = temp->right;
				}
			}
		}
	}
	bst->size++;
	return newNode;
}
BSTNODE *findBST(BST *bst, void *p) {
	BSTNODE *temp = bst->root;
	//Loops until the node pointer stores NULL
	while(temp != 0) {
		//Makes and stores the value from the comparator
		int comparison = bst->compare(p,temp->value);
		//printf("comparison: %d\n", comparison);
		if(comparison == 0) {
			//Returns the value associated with the key
			return temp;
		} else if(comparison < 0) {
			//Loops with the left child
			temp = temp->left;
		} else {
			//Loops with the right child
			temp = temp->right;
		}
	}
	//Value was not found, return NULL
	return NULL;
}
BSTNODE *deleteBST(BST *bst, void *value) {
	BSTNODE *node = findBST(bst, value);
	if(node == NULL) {
		printf("Value ");
		bst->display(stdout, value);
		printf(" not found.\n");
		return NULL;
	}
	node = swapToLeafBST(bst, node);
	pruneLeafBST(bst, node);
	node->parent = NULL;
	node->left = NULL;
	node->right = NULL;
	return node;
}
static BSTNODE *findPredecessor(BSTNODE *node) {
	node = node->left;
	while(node != NULL && node->right != NULL)
		node = node->right;
	return node;
}
static BSTNODE *findSuccessor(BSTNODE *node) {
	node = node->right;
	while(node != NULL && node->left != NULL)
		node = node->left;
	return node;
}
BSTNODE *swapToLeafBST(BST *t, BSTNODE *node) {
	if(node->left == NULL && node->right == NULL)
		return node;
	BSTNODE *pred = findPredecessor(node);
	if(pred != NULL) {
		t->swap(pred, node);
		return swapToLeafBST(t, pred);
	}

	BSTNODE *succ = findSuccessor(node);
	t->swap(succ, node);
	return swapToLeafBST(t, succ);
}
void pruneLeafBST(BST *bst, BSTNODE *node) {
	//Checks if the node is a leaf
	if(node->left != NULL || node->right != NULL)
		node = swapToLeafBST(bst, node);
	if(node == bst->root) {
		bst->root = NULL;
		bst->size = 0;
		return;
	}
	BSTNODE *parent = node->parent;
	//Checks if the node is a left or right child
	if(node == parent->left)
		parent->left = NULL;
	else
		parent->right = NULL;

	bst->size--;
}

/*
 * Returns the size of the BST by performing an in-order
 * traversal recursively.
 */
static int inorderSizeBST(BSTNODE *node) {
	//Base case - size of the null node is 0
	if(node == NULL)
		return 0;
	return 1 + inorderSizeBST(node->left) + inorderSizeBST(node->right);
}
/*
 * Returns the size of the bst. Should run in constant time except
 * when the root pointer has been forcibly updated. Then an
 * in order traversal must be performed to find the new size.
 */
int sizeBST(BST *bst) {
	if(bst->size == -1)
		bst->size = inorderSizeBST(bst->root);
	return bst->size;
}

/*
 * Finds the smallest subtree by recursively moving down each level to find a leaf node.
 * Then comparing left and right trees when needed.
 */
static int findSmallestSubtree(BSTNODE *node, int level) {
	if(node == NULL)
		return level;
	int left = findSmallestSubtree(node->left, level+1);
	int right = findSmallestSubtree(node->right, level+1);
	if(left < right)
		return left;
	else
		return right;
}
static int findLargestSubtree(BSTNODE *node, int level) {
	if(node == NULL)
		return level;
	int left = findLargestSubtree(node->left, level+1);
	int right = findLargestSubtree(node->right, level+1);
	if(left > right)
		return left;
	else
		return right;
}
/*
 * Prints out the number of nodes, minumum depth, and maximum depth.
 */
void statisticsBST(FILE *fp, BST *bst) {
	fprintf(fp, "Nodes: %d\n", sizeBST(bst));
	fprintf(fp, "Minimum depth: %d\n", findSmallestSubtree(bst->root, 0));
	fprintf(fp, "Maximum depth: %d\n", findLargestSubtree(bst->root, 0));
}

/*
 * Wrapper for BSTNODE, also holds a level value stored as an int.
 */
typedef struct bstnodemap {
	BSTNODE *node;
	int level;
} nodeMap;
/*
 * Creates the node map struct.
 */
static nodeMap *newNodeMap(BSTNODE *node, int level) {
	nodeMap *m = malloc(sizeof(nodeMap));
	assert(m != 0);

	m->node = node;
	m->level = level;
	return m;
}

/*
 * Displays all the nodes held in the BST in a breadth-first traversal.
 * Runs in linear time.
 */
void displayBST(FILE *fp, BST *bst) {
	if(bst->root == NULL) {
		fprintf(fp, "EMPTY\n");
		return;
	}

	QUEUE *nodes = newQUEUE(NULL);

	enqueue(nodes, newNodeMap(bst->root, 0));

	fprintf(fp, "0: ");
	while(sizeQUEUE(nodes) > 0) {
		nodeMap *nodeM = dequeue(nodes);
		BSTNODE *node = nodeM->node;
		int level = nodeM->level;
		//Check if node is leaf
		if(node->left == NULL && node->right == NULL)
			fprintf(fp, "=");
		bst->display(fp, node->value);
		fprintf(fp, "(");
		if(node->parent == NULL)
			bst->display(fp, node->value);
		else
			bst->display(fp, node->parent->value);
		fprintf(fp, ")-");

		//Determine if node is root, left or right child
		if(node == bst->root) {}
		else if(node == node->parent->left)
			fprintf(fp, "l");
		else
			fprintf(fp, "r");
		//Add both its children to the queue
		if(node->left != NULL)
			enqueue(nodes, newNodeMap(node->left, level+1));
		if(node->right != NULL)
			enqueue(nodes, newNodeMap(node->right, level+1));

		if(sizeQUEUE(nodes) > 0 && ((nodeMap *)peekQUEUE(nodes))->level > level)
			fprintf(fp, "\n%d: ", level+1);
		else if(sizeQUEUE(nodes) == 0)
			fprintf(fp, "\n");
		else
			fprintf(fp, " ");
	}
}
