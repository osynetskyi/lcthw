#ifndef _lcthw_Stack_Darray_h
#define _lcthw_Stack_Darray_h

#include <lcthw/darray.h>

#define STACK_FOREACH(S, V) int iter = 0;\
				       void *V = NULL;\
for (iter = DArray_end(S) - 1; (iter >= 0) && (V = DArray_get(S, iter--)); )

typedef DArray Stack;

// creates a stack
#define Stack_create() DArray_create(sizeof(void *), 16)

// destroys stack passed as argument
#define Stack_destroy DArray_destroy

// returns element at the top of the stack without removing it
#define Stack_peek DArray_last

// returns a number of elements in a stack
#define Stack_count DArray_count

// pushes an element onto stack
#define Stack_push DArray_push

// removes topmost element from stack and returns it
#define Stack_pop DArray_pop

#endif
