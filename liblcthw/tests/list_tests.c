#include "minunit.h"
#include <lcthw/list.h>
#include <assert.h>
#include <string.h>

static List *list = NULL;
static List *another = NULL;
static List *yet_another = NULL;
char *test1 = "test1 data";
char *test2 = "test2 data";
char *test3 = "test3 data";

char *test_create()
{
    list = List_create();
    mu_assert(list != NULL, "Failed to create list.");

    return NULL;
}

char *test_destroy()
{
    List_destroy(list);
    if (another != NULL) {
        List_destroy(another);
    }

    return NULL;
}

char *test_push_pop()
{
    List_push(list, test1);
    mu_assert(List_last(list) == test1, "Wrong last value.");

    List_push(list, test2);
    mu_assert(List_last(list) == test2, "Wrong last value.");

    List_push(list, test3);
    mu_assert(List_last(list) == test3, "Wrong last value.");
    mu_assert(List_count(list) == 3, "Wrong count on push.");

    char *val = List_pop(list);
    mu_assert(val == test3, "Wrong value on pop.");

    val = List_pop(list);
    mu_assert(val == test2, "Wrong value on pop.");

    val = List_pop(list);
    mu_assert(val == test1, "Wrong value on pop.");
    mu_assert(List_count(list) == 0, "Wrong count after pop.");

    return NULL;
}

char *test_unshift()
{
    List_unshift(list, test1);
    mu_assert(List_first(list) == test1, "Wrong first value.");

    List_unshift(list, test2);
    mu_assert(List_first(list) == test2, "Wrong first value");

    List_unshift(list, test3);
    mu_assert(List_first(list) == test3, "Wrong last value.");
    mu_assert(List_count(list) == 3, "Wrong count on unshift.");

    return NULL;
}

char *test_remove()
{
    // we only need to test the middle remove case since push/shift
    // already tests the other cases
    
    char *val = List_remove(list, list->first->next);
    mu_assert(val == test2, "Wrong removed element.");
    mu_assert(List_count(list) == 2, "Wrong count after remove.");
    mu_assert(List_first(list) == test3, "Wrong first after remove.");
    mu_assert(List_last(list) == test1, "Wrong last after remove.");

    return NULL;
}

char *test_shift()
{
    mu_assert(List_count(list) != 0, "Wrong count before shift.");

    char *val = List_shift(list);
    mu_assert(val == test3, "Wrong value on shift.");

    val = List_shift(list);
    mu_assert(val == test1, "Wrong value on shift.");
    mu_assert(List_count(list) == 0, "Wrong count after shift.");

    return NULL;
}

char *test_copy()
{
    another = List_copy(list);

    mu_assert(another != NULL, "Copy failed.");
    mu_assert(List_count(another) == List_count(list), "Counts are different.");
    mu_assert(another->first != list->first, "Pointers should be different.");
    mu_assert(strcmp(List_first(list), List_first(another)) == 0, "First values don't match after copy.");
    mu_assert(strcmp(List_last(list), List_last(another)) == 0, "Last values don't match after copy.");

    return NULL;
}

char *test_split()
{
    mu_assert(List_count(another) != 0, "Wrong count before split.");

    yet_another = List_split(another, another->first->next);
    mu_assert(List_count(another) == 1, "Wrong count in parent after split.");
    mu_assert(List_count(yet_another) == 2, "Wrong count in child after split.");
    mu_assert(strcmp(List_first(yet_another), test2) == 0, "Wrong first element of the child.");

    return NULL;
}

char *test_join()
{
   List_join(another, yet_another);
   mu_assert(List_count(another) == 3, "Wrong count after join.");
   mu_assert(strcmp(List_first(another), test3) == 0, "Wrong first element after join.");
   mu_assert(strcmp(List_last(another), test1) == 0, "Wrong last element after join.");

   return NULL;
}

char *all_tests()
{
    mu_suite_start();

    mu_run_test(test_create);
    mu_run_test(test_push_pop);
    mu_run_test(test_unshift);
    mu_run_test(test_copy);
    mu_run_test(test_remove);
    mu_run_test(test_split);
    mu_run_test(test_join);
    mu_run_test(test_shift);
    mu_run_test(test_destroy);

    return NULL;
}

RUN_TESTS(all_tests);
