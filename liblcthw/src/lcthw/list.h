#ifndef lcthw_List_h
#define lcthw_List_h

#include <stdlib.h>

// list node is value and pointers to previous and next list nodes
typedef struct ListNode {
    struct ListNode *next;
    struct ListNode *prev;
    void *value;
} ListNode;

// list if a structure consisting of 0 or more nodes and pointers
// to the first and last element of the list
typedef struct List {
    int count;
    ListNode *first;
    ListNode *last;
} List;

// allocates memnory for List struct
List *List_create();

// sanity check of list - it's not null, count not below 0, first is not null
int List_check(List * list);

// frees pointers to all list nodes in order; doesn't free pointers to values
void List_destroy(List * list);

// frees pointers to all values stored in list; doesn't free pointers to nodes
void List_clear(List * list);

// fress pointers to values and nodes in one go, 
// effectively removing the List from memory
void List_clear_destroy(List * list);

// returns the number of nodes in list
#define List_count(A) ((A)->count)

// returns value of the first element in list or NULL
#define List_first(A) ((A)->first != NULL ? (A)->first->value : NULL)

// returns value of the last element in list or NULL
#define List_last(A) ((A)->last != NULL ? (A)->last->value : NULL)

// creates new node at the end of the list, puts value in it
void List_push(List * list, void *value);

// removes the last node from the list, returns its value
void *List_pop(List * list);

// creates new node at the beginning of the list, puts value in it
void List_unshift(List * list, void *value);

// removes the first node from the list, returns its value
void *List_shift(List * list);

// removes node from the list, returns its value
void *List_remove(List * list, ListNode * node);

// splits the list on the node, returns a pointer to new list that starts with node
List *List_split(List * list, ListNode * node);

// adds all elements of joiner to the end of main list, frees pointer to joiner
void List_join(List * main, List * joiner);

// returns a pointer to the copy of list
List *List_copy(List * list);

// iterates through list, usaed as
// LIST_FOREACH(list, first, next, cur)
#define LIST_FOREACH(L, S, M, V) ListNode *_node = NULL;\
						   ListNode *V = NULL;\
for(V = _node = L->S; _node != NULL; V = _node = _node->M)

#endif
