#include "example_value.h"
#include <stdlib.h>        //For malloc and free
#include <assert.h>        //For assert

/*
 * Contains the implementation of the example value. This structure can be used in generic data structures.
 */

struct example_value {     //Implementation of the defined struct in the header file
        int store;
};

EXAMPLE_VALUE *newEXAMPLE_VALUE(int c) {
        EXAMPLE_VALUE *v = malloc(sizeof(EXAMPLE_VALUE));        //Allocate memory for the new struct
        assert(v != 0);        //Ensure malloc could actually assign memory
        
        v->store = c;        //Assign values
        return v;        //Return the pointer to the struct
}
void freeEXAMPLE_VALUE(void *v) {
        EXAMPLE_VALUE *e = v;        //Cast the void pointer to the value pointer for simplicity later
        
        //Here you would free up any additional memory(e.g. pointers within the struct)
        
        free(v);        //Free the actual struct
}