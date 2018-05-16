#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "da.h"

/*
 * Written by Zach Wassynger on 8/27/17.
 *
 * The dynamic array class provides a struct with methods that
 * can hold elements in a resizeable array of void pointers.
 */

/*
 * Holds the display method and necessary info about the
 * underlying array.
 */
struct da {
	void (*display)(FILE *, void*);
	void **values;
	int size;
	int capacity;
};

/*
 * Creates a new dynamic array, given a method to display its
 * elements.
 */
DA *newDA(void (*d)(FILE *,void *)) {
	DA *array;

	array = (DA *) malloc(sizeof(DA));

	//Ensures that the dynamic array's memory was allocated
	assert(array != 0);

	array->display = d;
	array->size = 0;
	array->capacity = 1;
	array->values = (void **) malloc(sizeof(void *));

	//Ensures that the underlying array's memory was allocated correctly
	assert(array->values != 0);

	return array;
}

/*
 * Adds an element to the back of the array.
 */
void insertDA(DA *items, void *value) {
	if(items->size == items->capacity) {
		items->capacity = items->capacity*2;
		void **p = (void **) realloc(items->values,
		(sizeof(void *)*items->capacity));

		//Checks if the array's memory was reallocated correctly
		assert(p != 0);
		//Assigns the new pointer to the dynamic array
		items->values = p;
	}
	items->values[items->size++] = value;
}

/*
 * Removes the last element of the array and returns it.
 */
void *removeDA(DA *items) {
	//Stores the to-be-removed value in a temp pointer
	void *p = items->values[--items->size];
	//Removes the value by inserting NULL
	items->values[items->size] = NULL;

	//Checks if the array should be downsized
	if(items->size<(items->capacity/4.0) && items->capacity > 1) {
		items->capacity = items->capacity/2;
		assert(items->capacity != 0);
		void **p = (void **) realloc(items->values,
		(sizeof(void *)*items->capacity));

		//Checks if the array's memory was reallocated correctly
		assert(p != 0);
		items->values = p;
	}
	return p;
}

/*
 * Adds all the values from the donor array to the recipient array,
 * and wipes the donor array.
 */
void unionDA(DA *recipient,DA *donor) {
	//Adds all the values in the donor array to the recipient array
	for(int i=0; i<donor->size; i++) {
		insertDA(recipient,donor->values[i]);
	}
	//Removes all the values in the donor's array
	while(donor->size > 0) {
		removeDA(donor);
	}
}

/*
 * Finds and returns the element at the given index.
 */
void *getDA(DA *items,int index) {
	assert(index >= 0);
	assert(index < items->size);
	return items->values[index];
}

/*
 * Sets the given value at the given index. Returns the
 * old value.
 */
void *setDA(DA *items, int index, void *value) {
	assert(index >= 0);
	assert(index <= items->size);
	void *p = NULL;
	if(index == items->size) {
		insertDA(items,value);
		return p;
	}
	p = getDA(items,index);
	items->values[index] = value;
	return p;
}

/*
 * Extracts the underlying array and resets the dynamic array.
 */
void **extractDA(DA *items) {
	//Checks if the size of the DA is 0 to avoid the assert
	if(items->size == 0) {
		return 0;
	}

	void **p = (void **) realloc(items->values,(sizeof(void *)*items->size));
	assert(p != 0);

	items->values = (void **) malloc(sizeof(void *));
	items->size = 0;
	items->capacity = 1;
	return p;
}

/*
 * Returns the size of the array.
 */
int sizeDA(DA *items) {
	return items->size;
}

/*
 * Prints out the elements in the array, along with the size of the
 * empty space.
 */
void visualizeDA(FILE *fp, DA *items) {
	displayDA(fp,items);
	fprintf(fp,"[%d]",(items->capacity-sizeDA(items)));
}

/*
 * Prints out all the elements in the array.
 */
void displayDA(FILE *fp, DA *items) {
	fprintf(fp,"[");
    for(int i=0; i<(sizeDA(items)-1); i++) {
        items->display(fp, getDA(items, i));
        fprintf(fp,",");
    }
    //Prints out the final value in order to not have a comma after it
    if(sizeDA(items)>0) {
        items->display(fp, getDA(items, sizeDA(items)-1));
    }
	fprintf(fp,"]");
}
