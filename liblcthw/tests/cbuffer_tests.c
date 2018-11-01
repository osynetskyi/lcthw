#include "minunit.h"
#include <lcthw/cbuffer.h>
#include <lcthw/ringbuffer.h>
#include <lcthw/stats.h>
#include <string.h>
#include <time.h>

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

static CBuffer *buf = NULL;

char *test_create()
{
    buf = CBuffer_create(4096);
    mu_assert(buf != NULL, "Failed to create cbuffer.");

    return NULL;
}

char *test_destroy()
{
    CBuffer_destroy(buf);

    return NULL;
}

char *test_write()
{
    char *str = "this is no test.";
    int res = CBuffer_write(buf, str, 16);
    mu_assert(res == 16, "Failed to write to buffer.");
    mu_assert(buf->end == 16, "Buffer should end at 16.");

    return NULL;
}

char *test_read()
{
    char *recv = malloc(17);
    char *compare = "this is no test.";
    int res = CBuffer_read(buf, recv, 16);
    mu_assert(res == 16, "Failed to read from buffer.");
    mu_assert(buf->start == 16, "Wrong start after read.");
    CBuffer_clear(buf);
    mu_assert((buf->start == 0) && (buf->end == 0), "Pointers should be 0\
		 after clear.");

    recv[16] = 0;
    mu_assert(strncmp(recv, compare, 16) == 0, "Read the wrong value");
    free(recv);

    return NULL;
}

char *test_performance()
{
    int num_samples = 200000;
    CBuffer *cb = CBuffer_create(4096);
    RingBuffer *rb = RingBuffer_create(4096);
    Stats *st_ring = Stats_create();
    Stats *st_circ = Stats_create();
    time_t start_ring, start_circ;
    int i = 0, j = 0;
    char **samples = calloc(1, num_samples * sizeof(char *));
    for (i = 0; i < num_samples; i++) {
        samples[i] = rand_string(100);
    }

    for (j = 0; j < 50; j++) {
        start_ring = time(NULL);
        for (i = 0; i < num_samples; i++) {
	    char *recv = calloc(1, 50);
            RingBuffer_write(rb, samples[i], 100);
	    RingBuffer_read(rb, recv, 50);
	    RingBuffer_clear(rb);
	    free(recv);
        }
        Stats_sample(st_ring, (double)(time(NULL) - start_ring));

        start_circ = time(NULL);
        for (i = 0; i < num_samples; i++) {
            char *recv = calloc(1, 50);
	    CBuffer_write(cb, samples[i], 100);
 	    CBuffer_read(cb, recv, 50);
	    CBuffer_clear(cb);
	    free(recv);
        }
        Stats_sample(st_circ, (double)(time(NULL) - start_circ));
    }

    printf("Ring buffer statistics:\n");
    Stats_dump(st_ring);
    printf("Circular buffer statistics:\n");
    Stats_dump(st_circ);

    for (i = 0; i < num_samples; i++) {
        free(samples[i]);
    }
    free(samples);
    CBuffer_destroy(cb);
    RingBuffer_destroy(rb);
    free(st_ring);
    free(st_circ);
    return NULL;
}

char *all_tests()
{
    mu_suite_start();

    mu_run_test(test_create);
    mu_run_test(test_write);
    mu_run_test(test_read);
    //mu_run_test(test_performance);
    mu_run_test(test_destroy);

    return NULL;
}

RUN_TESTS(all_tests);
