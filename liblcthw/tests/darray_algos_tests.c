#include "minunit.h"
#include <lcthw/darray_algos.h>
#include <time.h>

static char *rand_string(int size)
{
    int i = 0;
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    char *str = calloc(1, size + 1);
    if (size) {
        --size;
	for (i = 0; i < size; i++) {
	    int key = rand() % (int) (sizeof(charset) - 1);
	    str[i] = charset[key];
	}
    }

    str[size] = '\0';

    return str;
}

int testcmp(char **a, char **b)
{
    //debug("%s %s\n", *a, *b);
    return strcmp(*a, *b);
}

DArray *create_words()
{
    srand(time(NULL));
    int count = 90;
    int length = 10;
    DArray *result = DArray_create(length, count);
    //DArray *result = DArray_create(0, 5);
    //char *words[] = { "asdfasfd",
//	"werwar", "13234", "asdfasfd", "oioj" };
    int i = 0;

    for (i = 0; i < count; i++) {
        DArray_push(result, rand_string(length));
    }

    /*for (i = 0; i < 5; i++) {
        DArray_push(result, words[i]);
    }*/

    return result;
}

int is_sorted(DArray * array)
{
    int i = 0;

    for (i = 0; i < DArray_count(array) - 1; i++) {
        if (strcmp(DArray_get(array, i), DArray_get(array, i + 1)) > 0) {
	    return 0;
	}
    }

    return 1;
}

char *run_sort_test(int (*func) (DArray * array, DArray_compare), 
	const char *name)
{
    DArray *words = create_words();
    mu_assert(!is_sorted(words), "Words should start not sorted.");

    clock_t start, end;
    float seconds = 0;

    debug("--- Testing %s sorting algorithm", name);
    start = clock();
    int rc = func(words, (DArray_compare) testcmp);
    end = clock();
    mu_assert(rc == 0, "sort failed");
    mu_assert(is_sorted(words), "didn't sort it");
    seconds = (float)(end - start) / CLOCKS_PER_SEC;
    debug("%s performance: %1.10f sec", name, seconds);

    DArray_clear_destroy(words);

    return NULL;
}

char *test_qsort()
{
    return run_sort_test(DArray_qsort, "qsort");
}

char *test_qsort_my()
{
    return run_sort_test(DArray_qsort_my, "my qsort");
}

char *test_heapsort()
{
    return run_sort_test(DArray_heapsort, "heapsort");
}

char *test_mergesort()
{
    return run_sort_test(DArray_mergesort, "mergesort");
}

char *test_mergesort_my()
{
    return run_sort_test(DArray_mergesort_my, "my mergesort");
}

char *test_heapsort_my()
{
    return run_sort_test(DArray_heapsort_my, "my heapsort");
}

char *test_sort_add()
{
    DArray *words = create_words();
    int old_end = words->end;
    int rc = DArray_sort_add(words, (DArray_compare) testcmp, rand_string(10));
    mu_assert(rc == 0, "Sort_add failed.");
    mu_assert(words->end == old_end + 1, "Count should be bigger.");
    mu_assert(is_sorted(words), "Should be sorted after insert.");
    
    DArray_clear_destroy(words);

    return NULL;
}

char *test_find()
{
    DArray *words = create_words();
    char *needle = rand_string(10);
    DArray_sort_add(words, (DArray_compare) testcmp, needle);
    int res = DArray_find(words, (DArray_compare) testcmp, needle);
    mu_assert(res != 2, "Error when searching.");
    mu_assert(res < words->end, "Too big for index");
    mu_assert(testcmp(&needle, (char **)(&words->contents[res])) == 0, 
	    "Elements don't match.");

    DArray_clear_destroy(words);
    return NULL;
}

char *all_tests()
{
    mu_suite_start();

    mu_run_test(test_qsort);
    mu_run_test(test_heapsort);
    mu_run_test(test_mergesort);
    mu_run_test(test_qsort_my);
    mu_run_test(test_heapsort_my);
    mu_run_test(test_mergesort_my);
    mu_run_test(test_sort_add);
    mu_run_test(test_find);

    return NULL;
}

RUN_TESTS(all_tests);
