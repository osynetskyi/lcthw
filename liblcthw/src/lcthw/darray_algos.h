#ifndef darray_algos_h
#define darray_algos_h

#include <lcthw/darray.h>

typedef int (*DArray_compare) (const void *a, const void *b);

int DArray_qsort(DArray * array, DArray_compare cmp);

int DArray_heapsort(DArray * array, DArray_compare cmp);

int DArray_mergesort(DArray * array, DArray_compare cmp);

// my implementtaion of qsort
int DArray_qsort_my(DArray * array, DArray_compare cmp);

// my implementation of heapsort
int DArray_heapsort_my(DArray * array, DArray_compare cmp);

// my implementation of mergesort
int DArray_mergesort_my(DArray * array, DArray_compare cmp);

// adds el to array and sorts it
int DArray_sort_add(DArray * array, DArray_compare cmp, void *el);

// uses binary search to find el in a sorted array, returns index or -1
// returns -2 on error
// if array is not sorted initially, qsort is used
int DArray_find(DArray * array, DArray_compare cmp, void *el);

int DArray_check_sorted(DArray * array, DArray_compare cmp);

#endif
