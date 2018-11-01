#include "liblcthw/src/lcthw/stats.h"
#include <stdio.h>
#include <stdlib.h>
#include "liblcthw/src/lcthw/bstrlib.h"

int main(int argc, char *argv[])
{
    FILE *infile;
    infile = fopen("ex43_2.txt", "r");
    Stats **st;
    char *line = NULL;
    size_t len = 0;
    double num = 0.0;
    int res = 1;
    bstring empty = bfromcstr("\0");
    st = malloc(4 * sizeof(Stats *));

    bstring *header = malloc(4 * sizeof(bstring));
    getline(&line, &len, infile);
    line[strcspn(line, "\n")] = 0;
    bstring first_line = bfromcstr(line);
    struct bstrList *chunks = bsplit(first_line, ' ');
    int i = 0, j = 0;
    for (i = 0; i < chunks->qty; i++) {
	if (!biseq(chunks->entry[i], empty)) {
	    header[j] = bstrcpy(chunks->entry[i]);
	    j++;
	}
    }

    bdestroy(first_line);
    bdestroy(empty);
    free(line);
    bstrListDestroy(chunks);

    for (i = 0; i < 4; i++) {
        st[i] = Stats_create();
    }

    while (!feof(infile)) {
        i = 0;
	while (res == 1) {
	    res = fscanf(infile, "%lf", &num);
	    if (res == 1) {
	        Stats_sample(st[i], num);
	        i++;
	    }
	    if (i == 4)
		i = 0;
	}
    }

    for (int i = 0; i < 4; i++) {
        printf("%s\n", header[i]->data);
	Stats_dump(st[i]);
    }

    fclose(infile);
    for (i = 0; i < 4; i++)
	free(st[i]);
    free(st);

    for (j = 0; j < 4; j++) {
        bdestroy(header[j]);
    }
    free(header);

    return 0;
}
