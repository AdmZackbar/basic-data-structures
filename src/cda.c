#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "cda.h"

/*
 * Written by Zach Wassynger on 8/30/17.
 *
 * The circular dynamic array class provides methods that
 * manipulate an array of void pointers. Because it is circular,
 * elements can be inserted/removed in the front or the back.
 * The array will resize itself when it needs to.
 */

/*
 * Holds the display method, as well as necessary information
 * about its values.
 */
struct cda {
	void (*display)(FILE *, void*);
	void **values;
	int startIndex;
	int endIndex;
	int size;
	int capacity;
};

/*
 * Creates a new circular dynamic array. Needs to be passed
 * a method to display its values.
 */
CDA *newCDA(void (*d)(FILE *,void *)) {
	CDA *array;

	array = (CDA *) malloc(sizeof(CDA));

	//Checks that the dynamic circular array has memory allocated
	assert(array != 0);

	array->display = d;
	array->values = (void **) malloc(sizeof(void *));
	//Checks that the underlying array has memory allocated
	assert(array->values != 0);
	array->startIndex = 0;
	array->endIndex = 0;
	array->size = 0;
	array->capacity = 1;

	return array;
}

/*
 * Calculates the correct index when it is passed an index that
 * is out of bounds.
 */
static int indexWrapper(CDA *items, int index) {
	int remainder = index % items->capacity;
	return remainder < 0 ? remainder + items->capacity : remainder;
}

/*
 * Resizes the array by the given factor.
 */
static void resizeCDA(CDA *items, double factor) {
	void **temp = items->values;
	items->values = (void **) malloc(sizeof(void *) *
	(items->capacity*factor));

	assert(items->values != 0);

	for(int i=0; i<items->size; i++) {
		items->values[i]=temp[indexWrapper(items,items->startIndex+i)];
	}

	free(temp);
	items->capacity = items->capacity * factor;
	items->startIndex = 0;
	items->endIndex = items->size-1;

	//Ensures endIndex will not be -1
	if(items->size == 0) {
		items->endIndex = 0;
	}
}

/*
 * Inserts an element into the front of the array.
 */
void insertCDAfront(CDA *items, void *value) {
	if (items->size == 0) {
			items->values[items->size++] = value;
			items->startIndex = 0;
			items->endIndex = 0;
			return;
	}
	if(items->size == items->capacity) {
		resizeCDA(items, 2.0);
	}

	items->startIndex = indexWrapper(items, items->startIndex-1);
	items->values[items->startIndex] = value;
	items->size++;
}

/*
 * Inserts an element into the back of the array.
 */
void insertCDAback(CDA *items, void *value) {
	if (items->size == 0) {
			items->values[items->size++] = value;
			items->startIndex = 0;
			items->endIndex = 0;
			return;
	}
	if(items->size == items->capacity) {
		resizeCDA(items, 2.0);
	}

	items->endIndex = indexWrapper(items,items->endIndex+1);
	items->values[items->endIndex] = value;
	items->size++;
}

/*
 * Removes the first element from the array.
 */
void *removeCDAfront(CDA *items) {
	assert(items->size > 0);
	void *value = items->values[items->startIndex];
	items->values[items->startIndex] = 0;
	items->startIndex = indexWrapper(items, items->startIndex+1);
	items->size--;

	if(items->size < items->capacity/4.0 && items->capacity > 1) {
		resizeCDA(items, 0.5);
	}

	return value;
}

/*
 * Removes the last element from the array.
 */
void *removeCDAback(CDA *items) {
	assert(items->size > 0);
	void *value = items->values[items->endIndex];
	items->values[items->endIndex] = 0;
	items->endIndex = indexWrapper(items, items->endIndex-1);
	items->size--;

	if(items->size < items->capacity/4.0 && items->capacity > 1) {
		resizeCDA(items, 0.5);
	}

	return value;
}

/*
 * Adds all the values from the donor array to the recipient
 * array, and removes all the values from the donor array.
 */
void unionCDA(CDA *recipient,CDA *donor) {
	for(int i=donor->size; i>0; i--) {
		insertCDAback(recipient,removeCDAfront(donor));
	}
}

/*
 * Returns the value at the given index.
 */
void *getCDA(CDA *items,int index) {
	assert(index >= 0 && index < items->size);

	return items->values[indexWrapper(items,items->startIndex+index)];
}

/*
 * Sets the given value at the given index. Returns the
 * previous value at the index.
 */
void *setCDA(CDA *items,int index,void *value) {
	assert(index >= -1 && index <= items->size);

	if(index == items->size) {
		insertCDAback(items,value);
	} else if(index == -1) {
		insertCDAfront(items,value);
	} else {
		int trueIndex = indexWrapper(items,items->startIndex+index);
		void *oldValue = items->values[trueIndex];
		items->values[trueIndex] = value;

		return oldValue;
	}
	return 0;
}

/*
 * Extracts the underlying void pointer array. Resets the given CDA
 * to an empty array.
 */
void **extractCDA(CDA *items) {
	//Checks if size is zero to avoid assert statement
	if(items->size == 0) {
		return 0;
	}

	void **exactArray = (void **) malloc(sizeof(void *) * items->size);
	assert(exactArray != 0);

	for(int i=0; i<items->size; i++) {
		exactArray[i] = getCDA(items,i);
	}
	void **temp = items->values;
	items->values = (void **) malloc(sizeof(void *));
	assert(items->values != 0);
	items->size = 0;
	items->capacity = 1;
	items->startIndex = items->endIndex = 0;

	free(temp);
	return exactArray;
}

/*
 * Returns the size of the array.
 */
int sizeCDA(CDA *items) {
	return items->size;
}

/*
 * Prints out all the values in the array, plus how much empty space.
 */
void visualizeCDA(FILE *fp, CDA *items) {
	displayCDA(fp,items);
	fprintf(fp,"(%d)",(items->capacity-sizeCDA(items)));
}

/*
 * Prints out all the values in the array.
 */
void displayCDA(FILE *fp, CDA *items) {
	fprintf(fp, "(");
	for(int i=0; i<(sizeCDA(items)-1); i++) {
		items->display(fp, getCDA(items, i));
		fprintf(fp, ",");
	}
	if(sizeCDA(items)>0) {
		items->display(fp, getCDA(items, sizeCDA(items)-1));
	}
	fprintf(fp, ")");
}
