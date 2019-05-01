#include "minunit.h"
#include <lcthw/tstree.h>
#include <string.h>
#include <assert.h>
#include <lcthw/bstrlib.h>

TSTree *node = NULL;
char *valueA = "VALUEA";
char *valueB = "VALUEB";
char *value2 = "VALUE2";
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
	    "Failed to insert into tst with fourth name.");

    return NULL;
}

char *test_search_exact()
{
    // tst returns the last one inserted
    void *res = TSTree_search(node, bdata(&test1), blength(&test1));
    mu_assert(res == valueA,
	    "Got the wrong value back, should get A not B.");

    // tst does not find if not exact
    res = TSTree_search(node, "TESTNO", strlen("TESTNO"));
    mu_assert(res == NULL, "Shoud not find anything.");

    return NULL;
}

char *test_search_prefix()
{
    void *res = TSTree_search_prefix(
	    node, bdata(&test1), blength(&test1));
    debug("result: %p, expected: %p", res, valueA);
    mu_assert(res == valueA, "Got wrong valueA by prefix.");

    res = TSTree_search_prefix(node, bdata(&test1), 1);
    debug("result: %p, expected: %p", res, value4);
    mu_assert(res == value4, "Got wrong value4 for prefix of 1.");

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
    TSTree *tree = NULL;
    tree = TSTree_insert(tree, "start", strlen("start"), "start_value");
    tree = TSTree_insert(tree, "sport", strlen("sport"), "sport_value");
    tree = TSTree_insert(tree, "sublime", strlen("sublime"), "sublime_value");
    tree = TSTree_insert(tree, "suburb", strlen("suburb"), "suburb_value");
    tree = TSTree_insert(tree, "subway", strlen("subway"), "subway_value");
    tree = TSTree_insert(tree, "subdomain", strlen("subdomain"), "subdomain_value");

    DArray *res = TSTree_collect(tree, "subconcious", strlen("subconcious"));
    
    mu_assert(DArray_count(res) == 4, "Wrong suffix count");
    
    /*tree = TSTree_insert(tree, "a", strlen("a"), "a_val");
    tree = TSTree_insert(tree, "ab", strlen("ab"), "ab_val");
    tree = TSTree_insert(tree, "ac", strlen("ac"), "ac_val");
    tree = TSTree_insert(tree, "ad", strlen("ad"), "ad_val");
    DArray *res = TSTree_collect_keys(tree, bfromcstr(""));*/
    if (res) {
        int i = 0;
        for (i = 0; i < DArray_count(res); i++) {
            if (DArray_get(res, i)) {
		//printf("%s\n", bdata((bstring)DArray_get(res, i)));
	        bdestroy((bstring)DArray_get(res, i));
	    }
        }
        DArray_destroy(res);
    }
    TSTree_destroy(tree);

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

    char *res = (char *)TSTree_search_suffix(tree, "t.google.com", 12);
    mu_assert(strncmp(res, v3, 5) == 0, "Wrong value after search_suffix");

    //DArray *tmp = TSTree_collect(tree, "moc.elgoog.", strlen("moc.elgoog."));

    TSTree_destroy(tree);

    return NULL;
}

char *test_delete()
{
    TSTree_delete(node, bdata(&test1), blength(&test1), NULL);
    void *res = TSTree_search(node, bdata(&test1), blength(&test1));
    mu_assert(res == NULL, "valueA should be deleted.");

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
