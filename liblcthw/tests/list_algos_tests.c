#include "minunit.h"
#include <lcthw/list_algos.h>
#include <assert.h>
#include <string.h>
#include <time.h>

//char *values[] = { "XXXX", "1234", "abcd", "xjvef", "NDSS" };

//#define NUM_VALUES 5

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

void print(List *list)
{
    LIST_FOREACH(list, first, next, cur) {
        debug("%s", (char *)cur->value);
    }
}

List *create_words()
{
    srand(time(NULL));
    int num = rand() % 10;
    //int num = 8;

    int i = 0;
    List *words = List_create();

    /*for (i = 0; i < NUM_VALUES; i++) {
        List_push(words, values[i]);
    }*/
    for (i = 0; i < num; i++) {
        List_push(words, rand_string(rand() % 10 + 2));
    }

    /*LIST_FOREACH(words, first, next, cur) {
        printf("%s\n", (char *)cur->value);
    }*/

    return words;
}

int is_sorted(List * words)
{
    LIST_FOREACH(words, first, next, cur) {
        if (cur->next && strcmp(cur->value, cur->next->value) > 0) {
	    debug("%s %s", (char *)cur->value,
		    (char *)cur->next->value);
	    return 0;
	}
    }

    return 1;
}

char *test_bubble_sort()
{
    List *words = create_words();

    // should work on a list that needs sorting
    int rc = List_bubble_sort(words, (List_compare) strcmp);
    mu_assert(rc == 0, "Bubble sort failed.");
    mu_assert(is_sorted(words),
	    "Words are not sorted after bubble sort.");

    //print(words);

    // should work on an already sorted list
    rc = List_bubble_sort(words, (List_compare) strcmp);
    mu_assert(rc == 0, "Bubble sort of already sorted failed.");
    mu_assert(is_sorted(words),
	    "Words should be sort if already bubble sorted.");

    List_clear_destroy(words);

    // shall work on an empty list
    words = List_create();
    rc = List_bubble_sort(words, (List_compare) strcmp);
    mu_assert(rc == 0, "Bubble sort failed on empty list.");
    mu_assert(is_sorted(words), "Words should be sorted if empty.");

    List_destroy(words);

    return NULL;
}

char *test_merge_sort()
{
    List *words = create_words();

    // should work on a list that needs sorting
    List *res = List_merge_sort(words, (List_compare) strcmp);
    mu_assert(res, "Merge sort failed.");
    mu_assert(is_sorted(res), "Words are not sorted after merge sort.");

    //print(res);

    List *res2 = List_merge_sort(res, (List_compare) strcmp);
    mu_assert(res2, "Merge sort failed.");
    mu_assert(is_sorted(res),
	    "Should still be sorted after merge sort.");
    
    // if list is 1 element long, double free is possible
    if (words != res) {
        List_destroy(res2);
        List_destroy(res);
    }

    List_clear_destroy(words);
    return NULL;
}

char *test_merge_sort_bottom_up()
{
    List *words = create_words();

    List *res = List_merge_sort_bottom_up(words, (List_compare) strcmp);
    mu_assert(res, "Bottom up merge sort failed.");
    mu_assert(is_sorted(res), "Words are not sorted after bottom up merge sort.");

    //print(res);

    if (words != res) {
        List_destroy(res);
    }

    List_clear_destroy(words);

    return NULL;
}

char *test_time()
{
    List *words;
    int i = 0;
    int iterations = 10000;
    float seconds = 0;
    float total_bubble = 0;
    float total_merge = 0;
    clock_t start, end;

    for (i = 0; i < iterations; i++) {
        words = create_words();
	
	start = clock();
	int rc = List_bubble_sort(words, (List_compare) strcmp);
	end = clock();
	seconds = (float)(end - start) / CLOCKS_PER_SEC;

	mu_assert(rc == 0, "Bubble sort failed.");
	mu_assert(is_sorted(words), 
		"Words are not sorted after bubble sort");
	List_clear_destroy(words);
	total_bubble += seconds;
    }

    for (i = 0; i < iterations; i++) {
        words = create_words();

	start = clock();
	List *res = List_merge_sort(words, (List_compare) strcmp);
	end = clock();
	seconds = (float)(end - start) / CLOCKS_PER_SEC;

	mu_assert(res, "Merge sotr failed.");
	mu_assert(is_sorted(res), "Words are not sorted after merge sort.");

	List_destroy(res);
	List_clear_destroy(words);
	total_merge += seconds;
    }

    printf("Bubble sort: %3.10f or %3.10f sec per iteration\n", total_bubble, (float)(total_bubble / iterations));
    printf("Merge sort: %3.10f or %3.10f sec per iteration\n", total_merge, (float)(total_merge / iterations));

    return NULL;
}

void print_list(List *list)
{
    LIST_FOREACH(list, first, next, cur) {
        printf("%s\n", (char *)cur->value);
    }
}

char *test_insert_sorted()
{
    List *words = create_words();
    int count = List_count(words);
    char *new_item = rand_string(50);

    int rc = List_bubble_sort(words, (List_compare) strcmp);
    mu_assert(rc == 0, "Bubble sort failed.");
    mu_assert(is_sorted(words),
	    "Words are not sorted after bubble sort.");

    //printf("Before insert: \n");
    //print_list(words);

    rc = List_insert_sorted(words, new_item, (List_compare) strcmp);
    
    //printf("After insert: \n");
    //print_list(words);

    mu_assert(rc == 0, "Insert after sort failed.");
    mu_assert(List_count(words) == count+1, "Wrong count.");
    mu_assert(is_sorted(words), "Should remain sorted.");

    List_clear_destroy(words);

    return NULL;
}

char *all_tests()
{
    mu_suite_start();

    mu_run_test(test_bubble_sort);
    mu_run_test(test_merge_sort);
    //mu_run_test(test_time);
    mu_run_test(test_insert_sorted);
    mu_run_test(test_merge_sort_bottom_up);

    return NULL;
}

RUN_TESTS(all_tests);
