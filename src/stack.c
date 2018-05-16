#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "stack.h"
#include "da.h"

/*
 * Written by Zach Wassynger on 9/3/17.
 *
 * The stack class utilizes a DA to contain its elements. It can push
 * elements to the back of the array, them pop them off.
 */

/*
 * Contains a DA as the container for the elements and methods.
 */
struct stack {
	DA *array;
	void (*display)(FILE *, void*);
};

/*
 * Creates a new stack by instantiating a DA.
 */
STACK *newSTACK(void (*d)(FILE *,void *)) {
	STACK *s = (STACK *) malloc(sizeof(STACK));
	assert(s != 0);

	s->array = newDA(d);
	s->display = d;

	return s;
}

/*
 * Adds an item to the stack.
 */
void push(STACK *items, void *value) {
	insertDA(items->array,value);
}

/*
 * Removes and returns the last element in the stack.
 */
void *pop(STACK *items) {
	assert(sizeSTACK(items) > 0);
	return removeDA(items->array);
}

/*
 * Returns the last element in the stack.
 */
void *peekSTACK(STACK *items) {
	assert(sizeSTACK(items) > 0);
	return getDA(items->array,sizeDA(items->array)-1);
}

/*
 * Returns the size of the array.
 */
int sizeSTACK(STACK *items) {
	return sizeDA(items->array);
}

/*
 * Displays all the elements in the stack in the order that
 * they would be pushed out.
 */
void displaySTACK(FILE *fp, STACK *items) {
	fprintf(fp,"|");
	for(int i=sizeSTACK(items)-1; i>0; i--) {
		items->display(fp,getDA(items->array,i));
		fprintf(fp,",");
	}
	if(sizeSTACK(items)>0) {
		items->display(fp,getDA(items->array, 0));
	}
	fprintf(fp,"|");
}

/*
 * Prints out the underlying DA array.
 */
void visualizeSTACK(FILE *fp, STACK *items) {
	displayDA(fp, items->array);
}
