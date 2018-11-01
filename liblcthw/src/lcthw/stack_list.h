#ifndef _lcthw_Stack_List_h
#define _lcthw_Stack_List_h

#include <lcthw/list.h>

#define STACK_FOREACH(L, V) LIST_FOREACH(L, last, prev, V)

typedef List Stack;

// creates a stack
#define Stack_create List_create

// destroys stack passed as argument
#define Stack_destroy List_destroy

// returns element at the top of the stack without removing it
#define Stack_peek List_last

// returns a number of elements in a stack
#define Stack_count List_count

// pushes an element onto stack
#define Stack_push List_push

// removes topmost element from stack and returns it
#define Stack_pop List_pop

#endif
