#ifndef __EXAMPLE_VALUE_INCLUDED__                //Standard guards
#define __EXAMPLE_VALUE_INCLUDED__

typedef struct example_value EXAMPLE_VALUE        //Defines the struct as an easier to express value

EXAMPLE_VALUE *newEXAMPLE_VALUE(int c);           //Creates the struct with the given parameters(in this case an int)
void freeEXAMPLE_VALUE(void *p);                  //Frees the given struct(N.B. void * is used so that the free function can be passed in elsewhere

//Additional methods might include getters, setters, etc.

#endif
