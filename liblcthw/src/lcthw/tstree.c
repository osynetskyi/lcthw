#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <lcthw/dbg.h>
#include <lcthw/tstree.h>
#include <lcthw/list.h>

char *reverse_string(const char *str, int count)
{
    //printf("reversing %s\n", str);
    char *res = calloc(1, count + 1);
    int i = 0;
    for (i = 0; i < count; i++) {
        res[i] = str[count - i - 1];
    }
    //printf("result is %s\n", res);

    return res;
}

static inline TSTree *TSTree_insert_base(TSTree * root, TSTree * node,
	const char *key, size_t len,
	void *value)
{
    assert(key != NULL && "Key passed to TSTree_insert_base can't be NULL.");

    if (node == NULL) {
        node = (TSTree *) calloc(1, sizeof(TSTree));

	if (root == NULL) {
	    root = node;
	}

	node->splitchar = *key;
    }

    if (*key < node->splitchar) {
        node->low = TSTree_insert_base(
		root, node->low, key, len, value);
    } else if (*key == node->splitchar) {
        if (len > 1) {
	    node->equal = TSTree_insert_base(
		    root, node->equal, key + 1, len - 1, value);
	} else {
	    if (node->arr == NULL) {
	        node->arr = DArray_create(10, 10);
	    }
	    DArray_push(node->arr, value);
	    // assert(node->value == NULL && "Duplicate insert into tst.");
	    // node->value = value;
	}
    } else {
        node->high = TSTree_insert_base(
		root, node->high, key, len, value);
    }

    return node;
}

TSTree *TSTree_insert(TSTree * node, const char *key, size_t len,
	void *value)
{
    return TSTree_insert_base(node, node, key, len, value);
}

TSTree *find_node(TSTree * root, const char *key, size_t len)
{
    assert(key != NULL && "Key passed to TSTree_search can't be NULL.");

    TSTree *node = root;
    size_t i = 0;

    while (i < len && node) {
        if (key[i] < node->splitchar) {
	    node = node->low;
	} else if (key[i] == node->splitchar) {
	    i++;
	    if (i < len)
		node = node->equal;
	} else {
	    node = node->high;
	}
    }

    return node;
}

DArray *TSTree_search(TSTree * root, const char *key, size_t len)
{
    TSTree *node = find_node(root, key, len);

    if (node) {
        return node->arr;
    } else {
        return NULL;
    }
}

DArray *TSTree_search_prefix(TSTree * root, const char *key, size_t len)
{
    assert(key != NULL && "Key passed to TSTree_search_prefix can't be NULL.");
    if (len == 0)
	return NULL;

    TSTree *node = root;
    TSTree *last = NULL;
    size_t i = 0;

    while (i < len && node) {
        if (key[i] < node->splitchar) {
	    node = node->low;
	} else if (key[i] == node->splitchar) {
	    i++;
	    if (i < len) {
	        if (node->arr)
		    last = node;
		node = node->equal;
	    }
	} else {
	    node = node->high;
	}
    }

    node = node ? node : last;

    // travserse until we find the first value in the equal chain
    // this is then the first node with this prefix
    while (node && !node->arr) {
        node = node->equal;
    }

    return node ? node->arr : NULL;
}

List *recurse(TSTree * root, List *running)
{
    List *result = List_create();
    int i = 0;

    if (root->arr != NULL) {
	int count = List_count(running);
	char *res = calloc(1, count + 2);
	LIST_FOREACH(running, first, next, cur) {
	    res[i] = (int)(cur->value);
	    i++;
	}
	res[i] = root->splitchar;
	res[i + 1] = 0;
	List_push(result, res);
    }

    List *combine = NULL;

    if (root->low) {
        combine = recurse(root->low, List_copy(running));
	List_join(result, combine);
    }

    if (root->high) {
        combine = recurse(root->high, List_copy(running));
	List_join(result, combine);
    }

    if (root->equal) {
	List *running2 = List_copy(running);
	List_push(running2, &(root->splitchar));
        combine = recurse(root->equal, running2);
	List_join(result, combine);
    }

    List_destroy(running);

    return result;
}

DArray *TSTree_collect(TSTree * root, const char *key, size_t len)
{
    assert(key != NULL && "Key passed to TSTree_collect can't be NULL.");
    if (len == 0)
	return NULL;

    TSTree *node = root;
    //TSTree *last = NULL;
    size_t i = 0;
    char *common = calloc(1, len + 1);

    while (i < len && node) {
        if (key[i] < node->splitchar) {
	    node = node->low;
	} else if (key[i] == node->splitchar) {
	    common[i] = key[i];
	    i++;
	    if (i < len) {
	        //if (node->arr)
		//    last = node;
		node = node->equal;
	    }
	} else {
	    node = node->high;
	}
    }

    //node = node ? node : last;
    node = root;
    for (i = 0; i < strlen(common); i++) {
        node = node->equal;
    }

    //printf("Common prefix: %s, %c\n", common, node->splitchar);

    List *list = NULL;
    DArray *res = NULL;
    int count = 0;
    size_t length = 0; 
    if (!node) {
        res = DArray_create(strlen(common) + 1, 1);
	DArray_push(res, common);
    } else {
        list = recurse(node, List_create());
	//printf("returned cnt: %d\n", List_count(list));
        LIST_FOREACH(list, first, next, el) {
	    //printf("from list: %s\n", el->value);
	    count++;
	    if (strlen(el->value) > length)
		length = strlen(el->value);
            //printf("%s\n", el->value);
        }

	res = DArray_create(strlen(common) + length + 1, count + 1);
	for (el = _node = list->first; _node != NULL; el = _node = _node->next) {
	    char *tmp = calloc(1, strlen(common) + length + 1);
	    for (i = 0; i < strlen(common); i++) {
	        tmp[i] = common[i];
	    }
	    for (i = 0; i < strlen(el->value); i++) {
	        tmp[i + strlen(common)] = ((char *)el->value)[i];
	    }
	    DArray_push(res, tmp);
	}

        List_clear_destroy(list);
        free(common);
    }

    return res;
}

TSTree *TSTree_insert_suffix(TSTree * node, const char *key, size_t len,
	void *value)
{
    TSTree *res = NULL;
    char *reversed = reverse_string(key, len);
    //printf("Inserting %s\n", reversed);
    res = TSTree_insert(node, (const char *)reversed, len, value);
    free(reversed);
    return res;
}

DArray *TSTree_search_suffix(TSTree * node, const char *key, size_t len) 
{
    DArray *res = NULL;
    char *reversed = reverse_string(key, len);
    //printf("Searching for %s\n", reversed);
    res = TSTree_search_prefix(node, (const char *)reversed, len);
    free(reversed);
    return res;
}

void TSTree_traverse(TSTree * node, TSTree_traverse_cb cb, void *data)
{
    if (!node)
	return;

    if (node->low)
	TSTree_traverse(node->low, cb, data);

    if (node->equal) {
        TSTree_traverse(node->equal, cb, data);
    }

    if (node->high)
	TSTree_traverse(node->high, cb, data);

    if (node->arr)
	cb(node->arr, data);
}

void TSTree_delete(TSTree * root, const char *key, size_t len)
{
    TSTree *node = find_node(root, key, len);

    if (node) {
	DArray_destroy(node->arr);
	node->arr = NULL;
    }
}

void TSTree_destroy(TSTree * node)
{
    if (node == NULL)
	return;

    if (node->low)
	TSTree_destroy(node->low);
    
    if (node->equal) {
	TSTree_destroy(node->equal);
    }

    if (node->high)
	TSTree_destroy(node->high);

    if (node->arr)
	DArray_destroy(node->arr);

    free(node);
}
