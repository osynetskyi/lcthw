#ifndef _lcthw_TSTree_h
#define _lcthw_TSTree_h

#include <stdlib.h>
#include <lcthw/darray.h>
#include <lcthw/bstrlib.h>

typedef struct TSTree {
    char splitchar;
    struct TSTree *low;
    struct TSTree *equal;
    struct TSTree *high;
    void *value;
} TSTree;

typedef void (*TSTree_delete_cb) (void *value);

void *TSTree_search(TSTree * root, const char *key, size_t len);

void *TSTree_search_prefix(TSTree * root, const char *key, size_t len);

TSTree *TSTree_insert_suffix(TSTree * root, const char *key, size_t len, 
	void *value);

void *TSTree_search_suffix(TSTree * root, const char *key, size_t len);

void TSTree_delete(TSTree * root, const char *key, size_t len, TSTree_delete_cb cb);

typedef void (*TSTree_traverse_cb) (void *value, void *data);

TSTree *TSTree_insert(TSTree * node, const char *key, size_t len,
	void *value);

void TSTree_traverse(TSTree * node, TSTree_traverse_cb cb, void *data);

DArray *TSTree_collect(TSTree * root, const char *key, size_t len);

DArray *TSTree_collect_keys(TSTree * root, bstring running);

void TSTree_destroy(TSTree * root);

#endif
