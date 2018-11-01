#include "liblcthw/src/lcthw/stats.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    FILE *infile;
    Stats *st = Stats_create();
    infile = fopen("ex43_1.txt", "r");
    double sample = 0.0;

    while (!feof(infile)) {
        fscanf(infile, "%lf\n", &sample);
	Stats_sample(st, sample);
    }

    Stats_dump(st);
    free(st);
    fclose(infile);

    return 0;
}
