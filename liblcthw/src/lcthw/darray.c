#include <lcthw/darray.h>
#include <assert.h>

int DArray_check(DArray * array)
{
    check(array != NULL, "DArray is NULL.");
    check(array->end >= 0, "Last element shouldn't be negative.");
    check(array->max > 0, "Max should be above 0.");
    check(array->expand_rate > 0, "Expand rate incorrect.");
    check(array->contents != NULL, "Contents are invalid.");

    return 0;

error:
    return -1;

}

DArray *DArray_create(size_t element_size, size_t initial_max)
{
    DArray *array = malloc(sizeof(DArray));
    check_mem(array);
    array->max = initial_max;
    check(array->max > 0, "You must set an initial max > 0.");

    array->contents = calloc(initial_max, sizeof(void *));
    check_mem(array->contents);

    array->end = 0;
    array->element_size = element_size;
    array->expand_rate = DEFAULT_EXPAND_RATE;

    return array;

error:
    if (array)
        free(array);
    return NULL;
}

void DArray_clear(DArray * array)
{
    if (DArray_check(array) != 0)
	return;

    int i = 0;
    //debug("element size %d", array->element_size);
    if (array->element_size > 0) {
        for (i = 0; i < array->max; i++) {
	    if (array->contents[i] != NULL) {
	        free(array->contents[i]);
	    }
	}
    }
}

static inline int DArray_resize(DArray * array, size_t newsize)
{
    check(DArray_check(array) == 0, "DArray_check failed.");
    array->max = newsize;
    check(array->max > 0, "The newsize must be > 0.");

    void *contents = realloc(
	    array->contents, array->max * sizeof(void *));
    // check contents and assume realloc doesn't harm the original on error

    check_mem(contents);

    array->contents = contents;

    return 0;
error:
    return -1;
}

int DArray_expand(DArray * array)
{
    check(DArray_check(array) == 0, "DArray_check failed.");
    size_t old_max = array->max;
    check(DArray_resize(array, array->max + array->expand_rate) == 0,
	    "Failed to expand array to new size: %d",
	    array->max + (int)array->expand_rate);

    memset(array->contents + old_max, 0, array->expand_rate + 1);
    return 0;

error:
    return -1;
}

int DArray_mult_expand(DArray * array, int multiplier)
{
    check(DArray_check(array) == 0, "DArray_check failed.");
    size_t old_max = array->max;
    check(DArray_resize(array, array->max * multiplier) == 0,
	    "Failed to espand array to new size: %d",
	    array->max * multiplier);

    memset(array->contents + old_max, 0, array->max * (multiplier - 1) + 1);

    return 0;

error:
    return -1;
}

int DArray_contract(DArray * array)
{
    check(DArray_check(array) == 0, "DArray_check failed.");
    int new_size = array->end < (int)array->expand_rate ?
	    (int)array->expand_rate : array->end;

    return DArray_resize(array, new_size + 1);

error:
    return -1;
}

void DArray_destroy(DArray * array)
{
    if (DArray_check(array) != 0)
	return;

    if (array) {
        if (array->contents)
	    free(array->contents);
	free(array);
    }
}

void DArray_clear_destroy(DArray * array)
{
    DArray_clear(array);
    DArray_destroy(array);
}

int DArray_push(DArray * array, void *el)
{
    check(DArray_check(array) == 0, "DArray_check failed.");
    check(el != NULL, "Can't push NULL into array.");

    array->contents[array->end] = el;
    array->end++;

    if (DArray_end(array) >= DArray_max(array)) {
        return DArray_expand(array);
	//return DArray_mult_expand(array, 2);
    } else {
        return 0;
    }

error:
    return -1;
}

void *DArray_pop(DArray * array)
{
    check(DArray_check(array) == 0, "DArray_check failed.");
    check(array->end - 1 >= 0, "Attempt to pop from empty array.");

    void *el = DArray_remove(array, array->end - 1);
    array->end--;

    if (DArray_end(array) > (int)array->expand_rate
	    && DArray_end(array) % array->expand_rate) {
        DArray_contract(array);
    }

    return el;
error:
    return NULL;
}

// shallow copy of the array, deep copy requires better understanding
// of elements stored
DArray *DArray_copy(DArray * array)
{ 
    check(DArray_check(array) == 0, "Invalid source array.");
    int i = 0;
    DArray *res = DArray_create(array->element_size, array->max);
    res->expand_rate = array->expand_rate;

    for (i = 0; i < array->end; i++) {
	debug("Pushing %d %s", i, (char *)DArray_get(array, i));
        DArray_push(res, DArray_get(array, i));
    }

    return res;

error:
    return NULL;
}
