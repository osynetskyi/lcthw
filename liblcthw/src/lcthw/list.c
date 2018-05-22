#include <lcthw/list.h>
#include <lcthw/dbg.h>
#include <assert.h>

List *List_create()
{
    return calloc(1, sizeof(List));
}

int List_check(List * list)
{
    assert(list != NULL && "List can't be NULL.");
    check(List_count(list) >= 0, "Can't have less than zero elements.");

    if (List_count(list) > 0) {
	//debug("Count is %d", List_count(list));
        check(list->first != NULL, "First node can't be NULL.");
    }

    return 0;

error:
    return -1;
}

void List_destroy(List * list)
{
    if (List_check(list) != 0)
        return;

    LIST_FOREACH(list, first, next, cur) {
        if (cur->prev) {
	    free(cur->prev);
	}
    }

    free(list->last);
    free(list);
}

void List_clear(List * list)
{
    if (List_check(list) != 0)
	return;

    LIST_FOREACH(list, first, next, cur) {
        free(cur->value);
    }
}

void List_clear_destroy(List * list)
{
    if (List_check(list) != 0)
	return;

    debug("List count is %d", List_count(list));

    LIST_FOREACH(list, first, next, cur) {
        if (cur->prev) {
	    free(cur->prev);
	}
	debug("clearing %s", (char *)cur->value);
	free(cur->value);
    }

    free(list->last);
    free(list);
}

void List_push(List * list, void *value)
{
    if (List_check(list) != 0)
	return;

    ListNode *node = calloc(1, sizeof(ListNode));
    check_mem(node);

    node->value = value;

    if (list->first == NULL) {
        list->first = node;
	list->last = node;
    } else {
	list->last->next = node;
	node->prev = list->last;
	list->last = node;
    }

    list->count++;

error:
    return;
}

void *List_pop(List * list)
{
    if (List_check(list) != 0)
	return NULL;

    ListNode *node = list->last;
    return node != NULL ? List_remove(list, node) : NULL;
}

void List_unshift(List * list, void *value)
{
    if (List_check(list) != 0)
	return;

    ListNode *node = calloc(1, sizeof(ListNode));
    check_mem(node);

    node->value = value;

    if (list->first == NULL) {
        list->first = node;
	list->last = node;
    } else {
	node->next = list->first;
	list->first->prev = node;
	list->first = node;
    }

    list->count++;

error:
    return;
}

void *List_shift(List * list)
{
    if (List_check(list) != 0)
	return NULL;

    ListNode *node = list->first;
    return node != NULL ? List_remove(list, node) : NULL;
}

void *List_remove(List * list, ListNode * node)
{
    if (List_check(list) != 0)
	return NULL;

    void *result = NULL;

    check(list->first && list->last, "List is empty.");
    check(node, "node can't be NULL");

    if (node == list->first && node == list->last) {
        list->first = NULL;
	list->last = NULL;
    } else if (node == list->first)  {
        list->first = node->next;
	check(list->first != NULL,
		"Invalid list, somehow got a first that is NULL.");
	list->first->prev = NULL;
    } else if (node == list->last) {
        list->last = node->prev;
	check(list->last != NULL,
		"Invalid list, somehow got a next that is NULL.");
	list->last->next = NULL;
    } else {
        ListNode *after = node->next;
	ListNode *before = node->prev;
	after->prev = before;
	before->next = after;
    }

    list->count--;
    result = node->value;
    free(node);

error:
    return result;
}

List *List_split(List * list, ListNode * node)
{
    if (List_check(list) != 0)
	return NULL;

    int i = 0;

    LIST_FOREACH(list, first, next, cur) {
        if (cur == node)
	    break;
	i++;
    }

    if (i == List_count(list)) {
        log_err("Node not found");
	return NULL;	
    }

    List *res = List_create();

    res->count = list->count - i;
    res->first = node;
    res->last = list->last;

    list->last = node->prev;
    list->last->next = NULL;
    list->count = i;

    node->prev = NULL;

    return res;
}

void List_join(List * main, List * joiner)
{
    if ((List_check(main) != 0) || (List_check(joiner) != 0))
	return;

    main->last->next = joiner->first;
    joiner->first->prev = main->last;
    main->last = joiner->last;
    main->count += joiner->count;
    free(joiner);
}

List *List_copy(List * src)
{
    if (List_check(src) != 0)
	return NULL;

    List *res = List_create();

    LIST_FOREACH(src, first, next, cur) {
	List_push(res, cur->value);
    }  

    return res;
}
