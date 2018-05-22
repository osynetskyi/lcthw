#include <lcthw/list_algos.h>
#include <lcthw/dbg.h>

int check_sorted(List *list, List_compare cmp_func)
{
    if ((List_check(list) != 0) || (cmp_func == NULL))
	return -1;

    LIST_FOREACH(list, first, next, cur) {
        if (cur->next && (cmp_func((char *)cur->value, (char *)cur->next->value) > 0))
	    return 1;
    }

    return 0;
}

int List_bubble_sort(List *list, List_compare cmp_func)
{
    if ((List_check(list) != 0) || (cmp_func == NULL))
	return -1;

    if (List_count(list) < 2) {
        return 0;
    }

    unsigned int limit = List_count(list);
    unsigned int last_swap = 0;
    unsigned int i = 0;

    do {
	last_swap = 0;
	i = 0;
	LIST_FOREACH(list, first, next, cur) {
            if (cur->prev == NULL)
		continue;

	    if (cmp_func((char *)cur->prev->value, (char *)cur->value) > 0) {
	        char *temp = cur->prev->value;
		cur->prev->value = cur->value;
		cur->value = temp;
		last_swap = i;
	    }

	    i += 1;
	}
	limit = last_swap;
    } while (limit > 0); 

    #ifndef NDEBUG
        if (check_sorted(list, cmp_func) != 0)
	    return -1;
    #endif

    return 0;
}

List *list_merge(List *left, List *right, List_compare cmp_func)
{
    List *res = List_create();
    char *temp = NULL;

    while ((List_count(left) != 0) && (List_count(right) != 0)) {
        if (cmp_func((char *)left->first->value, (char *)right->first->value) <= 0) {
	    temp = (char *)List_shift(left);
	} else {
	    temp = (char *)List_shift(right);
	}
	List_push(res, temp);
    }

    while (List_count(left) > 0) {
        temp = (char *)List_shift(left);
	List_push(res, temp);
    }

    while (List_count(right) > 0) {
        temp = (char *)List_shift(right);
	List_push(res, temp);
    }

    List_destroy(left);
    List_destroy(right);

    return res;
}

List *list_merge_split(List *list, List_compare cmp_func, int to_free)
{
    if (List_count(list) < 2) {
        return list;
    }

    List *left = List_copy(list);
    List *right = NULL;
    int middle = List_count(list) / 2;
    int i = 0;
    
    LIST_FOREACH(left, first, next, cur) {
        if (i == middle) {
	    right = List_split(left, cur);
	    break;
	}
	i += 1;
    }

    left = list_merge_split(left, cmp_func, 1);
    right = list_merge_split(right, cmp_func, 1);

    if (to_free)
        List_destroy(list);

    return list_merge(left, right, cmp_func);
}

List *List_merge_sort(List *list, List_compare cmp_func)
{
    if ((List_check(list) != 0) || (cmp_func == NULL))
	return NULL;

    List *res = list_merge_split(list, cmp_func, 0);
    #ifndef NDEBUG
        if (check_sorted(res, cmp_func) != 0)
	    return NULL;
    #endif
    return res;
}

int List_insert_sorted(List *list, void *value, List_compare cmp_func)
{
    if ((List_check(list) != 0) || (cmp_func == NULL) || (value == NULL))
	return -1;
    check(check_sorted(list, cmp_func) == 0, "Input list not sorted.");

    if ((List_count(list) == 0) || (cmp_func((char *)value, (char *)List_first(list)) <= 0)) {
        List_unshift(list, value);
	return 0;
    }

    if (cmp_func((char *)value, (char *)List_last(list)) >= 0) {
        List_push(list, value);
	return 0;
    }

    LIST_FOREACH(list, first, next, cur) {
        if(cmp_func((char *)value, (char *)cur->value) < 0) {
	    ListNode *node = calloc(1, sizeof(ListNode));
	    node->value = value;
	    node->prev = cur->prev;
	    cur->prev->next = node;
	    cur->prev = node;
	    node->next = cur;
	    list->count++;
	    break;
	}
    }

    return 0;
error:
    return -1;
}

List *List_merge_sort_bottom_up(List *list, List_compare cmp_func)
{
    if ((List_check(list) != 0) || (cmp_func == NULL))
	return NULL;

    List *res = List_create();
    List *array[32] = { NULL };
    ListNode *cur, *next;
    int i = 0;

    cur = list->first;

    while (cur != NULL) {
        next = cur->next;

	List *wrapper = List_create();
	List_push(wrapper, cur->value);

	for (i = 0; (i < 32) && (array[i] != NULL); i++) {
	    wrapper = list_merge(array[i], wrapper, cmp_func);
	    array[i] = NULL;
	}

	if (i == 32)
	    i -= 1;
	
	array[i] = wrapper;
	cur = next;
    }

    for (i = 0; i < 32; i++) {
	if (array[i] != NULL) {
            res = list_merge(array[i], res, cmp_func);
	}
    }

    return res;
}
