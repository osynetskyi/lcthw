#define PERFTEST 0
#include "minunit.h"
#include <lcthw/string_algos.h>
#include <lcthw/bstrlib.h>
#include <time.h>
#include <lcthw/stats.h>

struct tagbstring IN_STR = bsStatic(
	"I have ALPHA beta ALPHA and oranges ALPHA");
struct tagbstring ALPHA = bsStatic("ALPHA");
const int TEST_TIME = 1;

#if PERFTEST
struct bstrList *IN_LIST;
#endif

char *rand_string(int size)
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

char *test_find_and_scan()
{
#if PERFTEST
    IN_LIST = bsplit(&IN_STR, ' ');
    bdestroy(IN_LIST->entry[2]);
    bdestroy(IN_LIST->entry[4]);
    bdestroy(IN_LIST->entry[7]);
#endif

    StringScanner *scan = StringScanner_create(&IN_STR);
    mu_assert(scan != NULL, "Failed to make the scanner.");

    int find_i = String_find(&IN_STR, &ALPHA);
    mu_assert(find_i > 0, "Failed to find 'ALPHA' in test string.");

    int scan_i = StringScanner_scan(scan, &ALPHA);
    mu_assert(scan_i > 0, "Failed to find 'ALPHA' with scan");
    mu_assert(scan_i == find_i, "find and scan don't match");

    scan_i = StringScanner_scan(scan, &ALPHA);
    mu_assert(scan_i > find_i,
	    "should find another ALPHA after the first");

    scan_i = StringScanner_scan(scan, &ALPHA);
    mu_assert(scan_i > find_i,
	    "should find another ALPHA after the first");

    mu_assert(StringScanner_scan(scan, &ALPHA) == -1,
	    "shouldn't find it");

    StringScanner_destroy(scan);

    return NULL;
}

#if PERFTEST
char *test_binstr_performance()
{
    int i = 0;
    int found_at = 0;
    unsigned long find_count = 0;
    time_t elapsed = 0;

    time_t start = time(NULL);

    do {
        for (i = 0; i < 1000; i++) {
	    char *rand = rand_string(5);
	    bstring needle = bfromcstr(rand);
	    IN_LIST->entry[2] = needle;
	    IN_LIST->entry[4] = needle;
	    IN_LIST->entry[7] = needle;
	    bstring haystack = bjoinStatic(IN_LIST, " ");
	    
	    found_at = binstr(haystack, 0, needle);
	    mu_assert(found_at != BSTR_ERR, "Failed to find!");
	    find_count++;
	    
	    bdestroy(haystack);
	    bdestroy(needle);
	    free(rand);
	}

	elapsed = time(NULL) - start;
    } while (elapsed <= TEST_TIME);

    debug("BINSTR COUNT: %lu, END TIME: %d, OPS: %f",
	    find_count, (int)elapsed, (double)find_count / elapsed);
    return NULL;
}

char *test_find_performance()
{
    int i = 0;
    int found_at = 0;
    unsigned long find_count = 0;
    time_t elapsed = 0;
    
    time_t start = time(NULL);

    do {
        for (i = 0; i < 1000; i++) { 
	    char *rand = rand_string(5);
	    bstring needle = bfromcstr(rand);
	    IN_LIST->entry[2] = needle;
	    IN_LIST->entry[4] = needle;
	    IN_LIST->entry[7] = needle;
	    bstring haystack = bjoinStatic(IN_LIST, " ");

	    found_at = String_find(haystack, needle);
	    mu_assert(found_at != -1, "Unable to find");
	    find_count++;

	    bdestroy(haystack);
	    bdestroy(needle);
	    free(rand);
	}

	elapsed = time(NULL) - start;
    } while (elapsed <= TEST_TIME);

    debug("FIND COUNT: %lu, END TIME: %d, OPS: %f",
	    find_count, (int)elapsed, (double)find_count / elapsed);

    return NULL;
}

char *test_scan_performance()
{
    int i = 0;
    int found_at = 0;
    unsigned long find_count = 0;
    time_t elapsed = 0;

    time_t start = time(NULL);

    do {
        for (i = 0; i < 1000; i++) {
	    found_at = 0;

	    char *rand = rand_string(5);
	    bstring needle = bfromcstr(rand);
	    IN_LIST->entry[2] = needle;
	    IN_LIST->entry[4] = needle;
	    IN_LIST->entry[7] = needle;
	    bstring haystack = bjoinStatic(IN_LIST, " ");
	    StringScanner *scan = StringScanner_create(haystack);

	    do {
	        found_at = StringScanner_scan(scan, needle);
		find_count++;
	    } while (found_at != -1);

	    StringScanner_destroy(scan);
	    bdestroy(haystack);
	    bdestroy(needle);
	    free(rand);
	}

	elapsed = time(NULL) - start;
    } while (elapsed <= TEST_TIME);

    debug("SCAN COUNT: %lu, END TIME: %d, OPS: %f",
	    find_count, (int)elapsed, (double)find_count / elapsed);

    return NULL;
}

double run_scan_test()
{
    int i = 0;
    int found_at = 0;
    unsigned long find_count = 0, cap = 100000;

    time_t start = time(NULL);
    do {
	for (i = 0; i < 1000; i++) {
	    found_at = 0;

	    char *rand = rand_string(5);
	    bstring needle = bfromcstr(rand);
	    IN_LIST->entry[2] = needle;
	    IN_LIST->entry[4] = needle;
	    IN_LIST->entry[7] = needle;
	    bstring haystack = bjoinStatic(IN_LIST, " ");
	    StringScanner *scan = StringScanner_create(haystack);

	    do {
		found_at = StringScanner_scan(scan, needle);
		find_count++;
	    } while (found_at != -1);

	    StringScanner_destroy(scan);
	    bdestroy(haystack);
	    bdestroy(needle);
	    free(rand);
	}
    
    } while (find_count <= cap);

    return (double)(time(NULL) - start);
}

char *test_stats()
{
    Stats *st = Stats_create();
    int num_runs = 100;
    int i = 0;
    double sample = 0.0;

    for (i = 0; i < num_runs; i++) {
        sample = run_scan_test();
	Stats_sample(st, sample);
    }

    Stats_dump(st);
    free(st);

    return NULL;
}
#endif

char *all_tests()
{
    mu_suite_start();

    mu_run_test(test_find_and_scan);

    // this is an idiom for commenting out sections of code
#if PERFTEST
    mu_run_test(test_scan_performance);
    mu_run_test(test_find_performance);
    mu_run_test(test_binstr_performance);
    mu_run_test(test_stats);

    IN_LIST->entry[2] = bfromcstr("epic fail");
    IN_LIST->entry[4] = bfromcstr("fail epic");
    IN_LIST->entry[7] = bfromcstr("lol");
    bstrListDestroy(IN_LIST);
#endif
    return NULL;
}

RUN_TESTS(all_tests);
