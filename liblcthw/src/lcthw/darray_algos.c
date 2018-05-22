#include <lcthw/darray_algos.h>
#include <stdlib.h>
#include <bsd/stdlib.h>

int DArray_qsort(DArray * array, DArray_compare cmp)
{
    qsort(array->contents, DArray_count(array), sizeof(void *), cmp);
    return 0;
}

int DArray_heapsort(DArray * array, DArray_compare cmp)
{
    return heapsort(array->contents, DArray_count(array),
	    sizeof(void *), cmp);
}

int DArray_mergesort(DArray * array, DArray_compare cmp)
{
    return mergesort(array->contents, DArray_count(array),
	    sizeof(void *), cmp);
}

void DArray_swap(DArray * array, int i, int j)
{
    void *temp = array->contents[i];
    array->contents[i] = array->contents[j];
    array->contents[j] = temp;
}

int partition(DArray * array, DArray_compare cmp, int lo, int hi)
{
    //debug("partitioning from %d to %d", lo, hi);
    void *pivot = DArray_get(array, hi);
    int i = lo - 1;
    int j = 0;
    for (j = lo; j < hi; j++) {
        if (cmp(&pivot, &array->contents[j]) > 0) {
	    i += 1;
	    DArray_swap(array, i, j);
	}
    }
    DArray_swap(array, i+1, hi);
    return i + 1;
}

int quicksort(DArray * array, DArray_compare cmp, int lo, int hi)
{
    check(lo >= 0 && hi < array->end, "Wrong bounds.");
    //debug("qsorting from %d to %d", lo, hi);
    if (lo < hi) {
        int p = partition(array, cmp, lo, hi);
	int rc = quicksort(array, cmp, lo, p - 1);
	check(rc == 0, "quicksort failed.");
	rc = quicksort(array, cmp, p + 1, hi);
	check(rc == 0, "quicksort failed.");
    }

    return 0;

error:
    return -1;
}

int DArray_qsort_my(DArray * array, DArray_compare cmp)
{
    return quicksort(array, cmp, 0, array->end - 1);
}

int iParent(int i)
{
    return (int)((i - 1) / 2);
}

int iLeftChild(int i)
{
    return (2 * i + 1);
}

int darray_sift_down(DArray *array, DArray_compare cmp, int start, int end)
{
    int root = start;
    while (iLeftChild(root) <= end) {
        int child = iLeftChild(root);
	int swap = root;

	if (cmp(&array->contents[child], &array->contents[swap]) > 0) {
	    swap = child;
	}
	
	if ((child+1 <= end) && (cmp(&array->contents[child+1], &array->contents[swap]) > 0)) {
	    swap = child + 1;
	}

	if (swap == root) {
	    return 0;
	} else {
	    DArray_swap(array, root, swap);
	    root = swap;
	}
    }

    return 0;
}

int darray_heapify(DArray * array, DArray_compare cmp)
{
    int start = iParent(array->end - 1); 
    while (start >= 0) {
        darray_sift_down(array, cmp, start, array->end - 1);
	start -= 1;
    }

    return 0;
}

int DArray_heapsort_my(DArray * array, DArray_compare cmp)
{
    int rc = darray_heapify(array, cmp);
    check(rc == 0, "Heapify failed.");
    int end = array->end - 1;
    while (end > 0) {
        DArray_swap(array, end, 0);
	end -= 1;
	darray_sift_down(array, cmp, 0, end);
    }

    return 0;

error:
    return -1;
}

int darray_merge(DArray *A, int begin, int middle, int end, 
		DArray *B, DArray_compare cmp)
{
    //debug("merging %d %d %d", begin, middle, end);
    int i = begin;
    int j = middle;
    int k = 0;

    for (k = begin; k < end; k++) {
        if (i < middle && (j >= end || cmp(&A->contents[j], &A->contents[i]) >= 0 )) {
            DArray_set(B, k, DArray_get(A, i));
	    i += 1; 
	} else {
	    DArray_set(B, k, DArray_get(A, j));
	    j += 1;
	}
    }

    return 0;
}

int darray_merge_split(DArray *B, int begin, int end, DArray *A, DArray_compare cmp)
{
    if (end - begin < 2)
	return 0;

    //debug("merge split from %d to %d", begin, end);
    int middle = (begin + end) / 2;
    int rc = darray_merge_split(A, begin, middle, B, cmp);
    check(rc == 0, "Merge sorting 1st half failed.");
    rc = darray_merge_split(A, middle, end, B, cmp);
    check(rc == 0, "Merge sorting 2n half failed.");
    rc = darray_merge(B, begin, middle, end, A, cmp);
    check(rc == 0, "Merging failed.");

    return 0;

error:
    return -1;
}

int DArray_mergesort_my(DArray * array, DArray_compare cmp)
{
    DArray *temp = DArray_copy(array);
    int rc = darray_merge_split(temp, 0, array->end, array, cmp);
    check(rc == 0, "Merge sort failed.");

    DArray_destroy(temp);
    return 0;

error:
    if (temp)
        DArray_destroy(temp);
    return -1;
}

int DArray_sort_add(DArray * array, DArray_compare cmp, void *el)
{
    int rc = DArray_push(array, el);
    check(rc == 0, "Push failed.");
    rc = DArray_qsort(array, cmp);
    check(rc == 0, "Qsort failed.");

    return 0;

error:
    return -1;
}

int DArray_is_sorted(DArray * array, DArray_compare cmp)
{
    check(DArray_check(array) == 0, "Invalid darray");
    int i = 0;
    for (i = 0; i < array->end - 1; i++) {
        if (cmp(&array->contents[i], &array->contents[i+1]) > 0) {
	    return 0;
	}
    }
    
    return 1;

error:
    return -1;
}

int DArray_find(DArray * array, DArray_compare cmp, void *el)
{
    check(DArray_check(array) == 0, "Invalid input array.");
    if (array->end == 0)
	return -1;

    if (!DArray_is_sorted(array, cmp)) {
        DArray_qsort(array, cmp);
    }

    int lo = 0;
    int hi = array->end;
    
    while (lo <= hi) {
        int middle = (int)((hi + lo) / 2);
	check(array->contents[middle], "Middle element is NULL.");
	int test = cmp(&el, &array->contents[middle]);
	if (test > 0) {
	    lo = middle + 1;
	} else if (test < 0) {
	    hi = middle - 1;
	} else {
	    return middle;
	}
    }

    return -1;

error:
    return -2;
}

int DArray_check_sorted(DArray * array, DArray_compare cmp)
{
    int i = 0;

    for (i = 0; i < DArray_count(array) - 1; i++) {
        if (cmp(DArray_get(array, i), DArray_get(array, i + 1)) > 0) {
	    return 0;
	}
    }

    return 1;
}


