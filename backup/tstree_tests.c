#include "minunit.h"
#include <lcthw/tstree.h>
#include <string.h>
#include <assert.h>
#include <lcthw/bstrlib.h>
#include <lcthw/list.h>

TSTree *node = NULL;
char *valueA = "VALUEA";
char *valueB = "VALUEB";
char *value2 = "VALUE2";
char *value3 = "VALUE3";
char *value4 = "VALUE4";
char *reverse = "VALUER";
int traverse_count = 0;

struct tagbstring test1 = bsStatic("TEST");
struct tagbstring test2 = bsStatic("TEST2");
struct tagbstring test3 = bsStatic("TSET");
struct tagbstring test4 = bsStatic("T");

char *test_insert()
{
    node = TSTree_insert(node, bdata(&test1), blength(&test1), valueA);
    mu_assert(node != NULL, "Failed to insert into tst.");

    node = TSTree_insert(node, bdata(&test2), blength(&test2), value2);
    mu_assert(node != NULL,
	    "Failed to insert into tst with second name.");

    node = TSTree_insert(node, bdata(&test3), blength(&test3), reverse);
    mu_assert(node != NULL,
	    "Failed to insert into tst with reverse name.");

    node = TSTree_insert(node, bdata(&test4), blength(&test4), value4);
    mu_assert(node != NULL,
	    "Failed to insert into tst with second name.");
    
    node = TSTree_insert(node, bdata(&test4), blength(&test4), value3);
    mu_assert(node != NULL,
	    "Failed to insert second value for the same key.");

    return NULL;
}

char *test_search_exact()
{
    // tst returns the last one inserted
    DArray *res = TSTree_search(node, bdata(&test1), blength(&test1));
    mu_assert(DArray_first(res) == valueA,
	    "Got the wrong value back, should get A not B.");

    // tst does not find if not exact
    res = TSTree_search(node, "TESTNO", strlen("TESTNO"));
    mu_assert(res == NULL, "SHould not find anything.");

    // see count for test4, should be 2
    res = TSTree_search(node, bdata(&test4), blength(&test4));
    mu_assert(DArray_count(res) == 2, "Wrong DArray count.");

    /*List *lol = recurse(node, List_create());
    printf("Returned %d results.\n", List_count(lol));
    LIST_FOREACH(lol, first, next, cur) {
        printf("%s\n", cur->value);
    }
    List_clear_destroy(lol);*/

    return NULL;
}

char *test_search_prefix()
{
    DArray *res = TSTree_search_prefix(
	    node, bdata(&test1), blength(&test1));
    mu_assert(DArray_first(res) == valueA, "Got wrong valueA by prefix.");

    res = TSTree_search_prefix(node, bdata(&test1), 1);
    mu_assert(DArray_first(res) == value4, "Got wrong value4 for prefix of 1.");

    res = TSTree_search_prefix(node, "TE", strlen("TE"));
    mu_assert(res != NULL, "Should find for short prefix.");

    res = TSTree_search_prefix(node, "TE--", strlen("TE--"));
    mu_assert(res != NULL, "Should find for partial prefix.");

    return NULL;
}

void TSTree_traverse_test_cb(void *value, void *data)
{
    assert(value != NULL && "Should not get NULL value.");
    assert(data == valueA && "Expecting valueA as the data.");
    traverse_count++;
}

char *test_traverse()
{
    traverse_count = 0;
    TSTree_traverse(node, TSTree_traverse_test_cb, valueA);
    debug("traverse count is: %d", traverse_count);
    mu_assert(traverse_count == 4, "Didn't find 4 keys.");

    return NULL;
}

char *test_collect()
{
    DArray *res = TSTree_collect(node, "T--", strlen("T--"));
    int i = 0;
    /*printf("count: %d\n", DArray_count(res));
    for (i = 0; i < DArray_count(res); i++) {
	printf("%s\n", DArray_get(res, i));
    }*/
    mu_assert(DArray_count(res) == 3, "Wrong count in res DArray.");

    DArray_clear_destroy(res);
    return NULL;
}

char *test_suffix()
{
    TSTree *tree = NULL;
    char *v1 = "Google";
    char *v2 = "Gmail";
    char *v3 = "Gchat";
    char *v4 = "Bing";
    tree = TSTree_insert_suffix(tree, "google.com", 10, v1);
    tree = TSTree_insert_suffix(tree, "mail.google.com", 15, v2);
    tree = TSTree_insert_suffix(tree, "chat.google.com", 15, v3);
    tree = TSTree_insert_suffix(tree, "bing.com", 8, v4);

    DArray *res = TSTree_search_suffix(tree, "t.google.com", 12);
    //for (int i = 0; i < DArray_count(res); i++) {
    //    printf("darray elem: %s\n", DArray_get(res, i));
    //}

    TSTree_destroy(tree);
    return NULL;
}

char *test_delete()
{
    TSTree_delete(node, bdata(&test4), blength(&test4));
    DArray *res = TSTree_search(node, bdata(&test4), blength(&test4));
    mu_assert(res == NULL, "Should be deleted.");

    return NULL;
}

char *test_destroy()
{
    TSTree_destroy(node);

    return NULL;
}

char *all_tests()
{
    mu_suite_start();

    mu_run_test(test_insert);
    mu_run_test(test_search_exact);
    mu_run_test(test_search_prefix);
    mu_run_test(test_traverse);
    mu_run_test(test_collect);
    mu_run_test(test_suffix);
    mu_run_test(test_delete);
    mu_run_test(test_destroy);

    return NULL;
}

RUN_TESTS(all_tests);
