#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "queue.h"
#include "cda.h"

/*
 * Written by Zach Wassynger on 9/3/17.
 *
 * The queue class utilizes a CDA to contain its elements.
 */

/*
 * Stores a CDA as the container for the elements and methods.
 */
struct queue {
	CDA *array;
	void (*display)(FILE *, void*);
};

/*
 * Creates a new queue by creating a CDA.
 */
QUEUE *newQUEUE(void (*d)(FILE *,void *)) {
	QUEUE *p = (QUEUE *) malloc(sizeof(QUEUE));
	assert(p != 0);

	p->array = newCDA(d);
	p->display = d;

	return p;
}

/*
 * Adds an element to the queue.
 */
void enqueue(QUEUE *items, void *value) {
	insertCDAback(items->array,value);
}

/*
 * Removes and returns the first element in the queue.
 */
void *dequeue(QUEUE *items) {
	assert(sizeCDA(items->array) > 0);
	return removeCDAfront(items->array);
}

/*
 * Returns the first element in the queue.
 */
void *peekQUEUE(QUEUE *items) {
	assert(sizeCDA(items->array) > 0);
	return getCDA(items->array,0);
}

/*
 * Returns the size of the queue.
 */
int sizeQUEUE(QUEUE *items) {
	return sizeCDA(items->array);
}

/*
 * Displays all the items in the queue.
 */
void displayQUEUE(FILE *fp, QUEUE *items) {
	fprintf(fp,"<");
	for(int i=0; i<(sizeQUEUE(items)-1); i++) {
		items->display(fp, getCDA(items->array, i));
		fprintf(fp,",");
	}
	if(sizeCDA(items->array)>0) {
		items->display(fp, getCDA(items->array, sizeQUEUE(items)-1));
	}
	fprintf(fp,">");
}

/*
 * Prints the CDA contained by the queue.
 */
void visualizeQUEUE(FILE *fp, QUEUE *items) {
	displayCDA(fp,items->array);
}
