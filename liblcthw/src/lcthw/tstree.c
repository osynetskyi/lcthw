#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <lcthw/dbg.h>
#include <lcthw/tstree.h>
#include <lcthw/bstrlib.h>

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
	const char * key, size_t len,
	void *value)
{
    check(key != NULL, "Insert key can't be NULL");
    check(value != NULL, "Insert value can't be NULL");
    check(len > 0, "Insert length must be positive");

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
	    assert(node->value == NULL && "Duplicate insert into tst.");
	    node->value = value;
	}
    } else {
        node->high = TSTree_insert_base(
		root, node->high, key, len, value);
    }

    return node;

error:
    return NULL;
}

TSTree *TSTree_insert(TSTree * node, const char *key, size_t len,
	void *value)
{
    return TSTree_insert_base(node, node, key, len, value);
}

void *TSTree_search(TSTree * root, const char *key, size_t len)
{
    check(key != NULL, "Search key can't be NULL");
    check(len > 0, "Search length must be positive");

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

    if (node) {
	return node->value;
    } else {
        return NULL;
    }

error:
    return NULL;
}

void *TSTree_search_prefix(TSTree * root, const char *key, size_t len)
{
    check(key != NULL, "Prefix search key can't be NULL");
    check(len > 0, "Prefix search length must be positive");

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
	        if (node->value)
		    last = node;
		node = node->equal;
	    }
	} else {
	    node = node->high;
	}
    }

    node = node ? node : last;

    // traverse until we find the first value in the equal chain
    // this is then the first node with this prefix
    while (node && !node->value) {
        node = node->equal;
    }

    return node ? node->value : NULL;

error:
    return NULL;
}

void TSTree_delete(TSTree * root, const char *key, size_t len)
{
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

    if (node) {
        //free(node->value);
	node->value = NULL;
    }
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

    if (node->value)
	cb(node->value, data);
}

DArray *TSTree_collect_keys(TSTree * root, bstring running)
{
    DArray *res = DArray_create(50, 10);

    if (root->value) {
	binsertch(running, blength(running), 1, root->splitchar);
	bstring insert = bstrcpy(running);
	//printf("Found value, inserting into res %s %p\n", bdata(insert), insert);
	DArray_push(res, insert);
    }

    if (root->low) {
        bstring left_running = bstrcpy(running);
	DArray *left_res = TSTree_collect_keys(root->low, left_running);
	DArray_join(res, left_res);
	bdestroy(left_running);
    }

    if (root->high) {
        bstring right_running = bstrcpy(running);
	DArray *right_res = TSTree_collect_keys(root->high, right_running);
	DArray_join(res, right_res);
	bdestroy(right_running);
    }

    if (root->equal) {
	if (!root->value) {
	    binsertch(running, blength(running), 1, root->splitchar);
	}
	DArray *down_res = TSTree_collect_keys(root->equal, running);
        DArray_join(res, down_res);
    }

    return res;
}

DArray *TSTree_collect(TSTree * root, const char *key, size_t len)
{
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
	    last = node;
	    node = node->equal;
	} else {
	    node = node->high;
	}
    }

    DArray *suffixes = NULL;

    if (last && last->equal) {
        char *pref = calloc(1, i + 1);
        for (size_t j = 0; j < i; j++) {
            pref[j] = key[j];
        }

        //printf("Common prefix: %s\n", pref);
        //printf("last splitchar: %c\n", last->splitchar);

        bstring running = bfromcstr(""); 
        bstring prefix = bfromcstr(pref);
        free(pref);

        suffixes = TSTree_collect_keys(last->equal, running);
	if (last->value) {
	    // if give na string that is already a key and also common prefix
	    DArray_push(suffixes, bfromcstr(""));
	}

        for (i = 0; i < suffixes->end; i++) {
            binsert(suffixes->contents[i], 0, prefix, "\0");
            //printf("%s\n", bdata((bstring)suffixes->contents[i])); 
        }
        bdestroy(prefix);
        bdestroy(running);
    }

    return suffixes;
}

TSTree *TSTree_insert_suffix(TSTree * root, const char *key, size_t len, void *value)
{
    char *reversed_key = reverse_string(key, len);
    TSTree *res = TSTree_insert(root, reversed_key, len, value);
    free(reversed_key);

    return res;
}

void *TSTree_search_suffix(TSTree * root, const char *key, size_t len)
{
    char *reversed_key = reverse_string(key, len);
    void *res = TSTree_search_prefix(root, reversed_key, len);
    free(reversed_key);

    return res;
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

    free(node);
}
