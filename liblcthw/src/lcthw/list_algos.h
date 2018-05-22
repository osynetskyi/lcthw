#ifndef lcthw_List_algos_h
#define lcthw_List_algos_h

#include <lcthw/list.h>

// comparing function for list elements
typedef int (*List_compare)(const void *elem1, const void *elem2);

// bubble sort function - works in-place, returns error code
int List_bubble_sort(List *list, List_compare cmp_func);

// merge sort function - creates a new sorted list, leaves original intact
List *List_merge_sort(List *list, List_compare cmp_func);

// inserts the element into the right position of the sorted list
int List_insert_sorted(List *list, void *value, List_compare cmp_func);

// bottom-up merge sort
List *List_merge_sort_bottom_up(List *list, List_compare cmp_func);

#endif
