#ifndef _lcthw_TSTree_h
#define _lcthw_TSTree_h

#include <stdlib.h>
#include <lcthw/darray.h>

typedef struct TSTree {
    char splitchar;
    struct TSTree *low;
    struct TSTree *equal;
    struct TSTree *high;
    // void *value;
    DArray *arr;
} TSTree;

//void *TSTree_search(TSTree * root, const char *key, size_t len);
DArray *TSTree_search(TSTree * root, const char *key, size_t len);

//void *TSTree_search_prefix(TSTree * root, const char *key, size_t len);
DArray *TSTree_search_prefix(TSTree * root, const char *key, size_t len);

DArray *TSTree_collect(TSTree * root, const char *key, size_t len);

//typedef void (*TSTree_traverse_cb) (void *value, void *data);
typedef void (*TSTree_traverse_cb) (void *arr, void *data);

TSTree *TSTree_insert(TSTree * node, const char *key, size_t len,
	void *value);

TSTree *TSTree_insert_suffix(TSTree * node, const char *key, size_t len,
	void *value);

DArray *TSTree_search_suffix(TSTree * node, const char *key, size_t len);

void TSTree_delete(TSTree * root, const char *key, size_t len);

void TSTree_traverse(TSTree * node, TSTree_traverse_cb cb, void *data);

void TSTree_destroy(TSTree * root);

#endif
