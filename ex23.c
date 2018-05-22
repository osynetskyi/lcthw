#define pack_of_eight(top, delta) \
if (delta != 0) {\
case (top-delta):\
;\
}\
*to++ = *from++;\
case (top-delta-1):\
*to++ = *from++;\
case (top-delta-2):\
*to++ = *from++;\
case (top-delta-3):\
*to++ = *from++;\
case (top-delta-4):\
*to++ = *from++;\
case (top-delta-5):\
*to++ = *from++;\
case (top-delta-6):\
*to++ = *from++;\
case (top-delta-7):\
*to++ = *from++;

#define device_8(size) \
int device(char *from, char *to, int count) \
{\
    {\
        int n = (count + size - 1) / size;\
	switch (count % size) {\
	    case 0:\
	        do {\
	        pack_of_eight(size, 0)\
		if (size > 8) {\
		pack_of_eight(size, 8)\
		if (size > 16) {\
		pack_of_eight(size, 16)\
		if (size > 24) {\
		pack_of_eight(size, 24)\
		}\
		}\
		}\
		} while (--n > 0);\
	}\
    }\
    \
    return count;\
}

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <float.h>
#include "dbg.h"

typedef int (*copyfunc) (char *from, char *to, int count);

device_8(32)

int normal_copy(char *from, char *to, int count)
{
    int i = 0;

    for (i = 0; i < count; i++) {
        to[i] = from[i];
    }

    return i;
}

int duffs_device(char *from, char *to, int count)
{
    {
	int n = (count + 7) / 8;

	switch (count % 8) {
	    case 0:
		do {
		    /*pack_of_eight(32, 0)
		    pack_of_eight(32, 8)
		    pack_of_eight(32, 16)
		    pack_of_eight(32, 24)*/
	            *to++ = *from++;
		    case 7:
		    *to++ = *from++;
		    case 6:
		    *to++ = *from++;
		    case 5:
		    *to++ = *from++;
		    case 4:
		    *to++ = *from++;
		    case 3:
		    *to++ = *from++;
		    case 2:
		    *to++ = *from++;
		    case 1:
		    *to++ = *from++;
		} while (--n > 0);
	}
    }

    return count;
}

int zeds_device(char *from, char *to, int count)
{
    {
        int n = (count + 7) / 8;

	switch (count % 8) {
	    case 0:
again:	  *to++ = *from++;

	    case 7:
	  *to++ = *from++;
	    case 6:
	  *to++ = *from++;
	    case 5:
	  *to++ = *from++;
	    case 4:
	  *to++ = *from++;
	    case 3:
	  *to++ = *from++;
	    case 2:
	  *to++ = *from++;
	    case 1:
	  *to++ = *from++;
	  if (--n > 0)
              goto again;

	}
    }

    return count;
}

int valid_copy(char *data, int count, char expects)
{
    int i = 0;
    for (i = 0; i < count; i++) {
        if (data[i] != expects) {
	    log_err("[%d] %c != %c", i, data[i], expects);
	    return 0;
	}
    }

    return 1;
}

int array_min(double *array, int count) {
    int min = 0;
    int curr = 0;
    while (curr < count) {
        if (array[curr] < array[min]) {
	    min = curr;
	}
	curr += 1;
    }

    return min;
}

int main(int argc, char *argv[])
{
    char from[1000] = { 'a' };
    char to[1000] = { 'c' };
    int rc = 0;
    int i = 0;
    clock_t start;
    clock_t end;

    copyfunc subjects[4] = { normal_copy, duffs_device, zeds_device, device };
    char *labels[4] = {"Normal copy", "Duff's device", "Zed's device", "Macro device" };
    double times[4] = { DBL_MAX };

    // setup the from to have some stuff
    memset(from, 'x', 1000);
    // set it to a failure mode
    memset(to, 'y', 1000);
    check(valid_copy(to, 1000, 'y'), "Not initialized right.");

    for (i = 0; i < 4; i++) {
        // reset
	memset(to, 'y', 1000);
	rc = 0;

	// test the subject
	start = clock();
	rc = subjects[i](from, to, 1000);
	end = clock();
	check(rc == 1000, "%s failed.", labels[i]);
	check(valid_copy(to, 1000, 'x'), "Validation of %s failed.", labels[i]);
	times[i] = (double)(end - start) / CLOCKS_PER_SEC;
	//printf("%s execution took %1.10f\n", labels[i], times[i]);
	//printf("%s result: %s\n", labels[i], to);
    }

    int winner = array_min(times, 4);

    printf("And the winner is... %s with the result of %1.10f!\n", labels[winner], times[winner]);

    start = clock();
    memset(to, 'y', 1000);
    end = clock();
    printf("Memset: %1.20f\n", (double)(end - start) / CLOCKS_PER_SEC);

    start = clock();
    memcpy(to, from, 1000);
    end = clock();
    printf("Memcpy: %1.20f\n", (double)(end - start) / CLOCKS_PER_SEC);

    memset(to, 'y', 1000);
    start = clock();
    memmove(to, from, 1000);
    end = clock();
    printf("Memmove: %1.20f\n", (double)(end - start) / CLOCKS_PER_SEC);

    return 0;
error:
    return 1;
}
