#include "minunit.h"
#include <lcthw/ringbuffer.h>
#include <lcthw/bstrlib.h>
#include <string.h>

static RingBuffer *buf = NULL;

char *test_create()
{
    buf = RingBuffer_create(16);
    mu_assert(buf != NULL, "Failed to create ringbuffer.");

    return NULL;
}

char *test_destroy()
{
    RingBuffer_destroy(buf);

    return NULL;
}

char *test_write()
{
    char *str = "this is no test.";
    int res = RingBuffer_write(buf, str, 16);
    mu_assert(res == 16, "Failed to write to buffer.");
    mu_assert(RingBuffer_full(buf), "Buffer should be full.");

    res = RingBuffer_write(buf, str, 2);
    mu_assert(res == -1, "Writing to full buffer should fail.");

    return NULL;
}

char *test_read()
{
    char *recv = malloc(6);
    char *compare = "this ";
    int res = RingBuffer_read(buf, recv, 5);
    mu_assert(res == 5, "Failed to read from buffer.");
    mu_assert(buf->start == 5, "Wrong start after read.");

    recv[5] = 0;
    mu_assert(strncmp(recv, compare, 5) == 0, "Read the wrong value");
    free(recv);

    return NULL;
}

char *test_gets()
{
    bstring res = NULL;
    bstring compare = bfromcstr("is no test.");
    res = RingBuffer_gets(buf, 11);
    mu_assert(res != NULL, "Failed to get bstring from buffer.");
    mu_assert(biseq(res, compare) == 1, "Got the wrong bstring out.");
    mu_assert(RingBuffer_empty(buf), "Buffer should be empty");

    bdestroy(compare);
    bdestroy(res);
    return NULL;
}

char *test_read_empty()
{
    char *recv = malloc(1);
    int res = RingBuffer_read(buf, recv, 1);
    mu_assert(res == -1, "Reading from an empty buffer should fail.");

    free(recv);
    return NULL;
}

char *test_puts_getall()
{
    bstring str = bfromcstr("teststr");
    int res = RingBuffer_puts(buf, str);
    mu_assert(res != -1, "Failed to put bstring to ringbuffer.");
    mu_assert(buf->end == blength(str), "Wrong end of the buffer.");

    bstring recv = RingBuffer_get_all(buf);
    mu_assert(recv != NULL, "Failed to get all from buffer.");
    mu_assert(biseq(str, recv) == 1, "Wrong value after RingBuffer_get_all");

    bdestroy(recv);
    bdestroy(str);
    return NULL;
}

char *all_tests()
{
    mu_suite_start();

    mu_run_test(test_create);
    mu_run_test(test_write);
    mu_run_test(test_read);
    mu_run_test(test_gets);
    mu_run_test(test_read_empty);
    mu_run_test(test_puts_getall);
    mu_run_test(test_destroy);

    return NULL;
}

RUN_TESTS(all_tests);
