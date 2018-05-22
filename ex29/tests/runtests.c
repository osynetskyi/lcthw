#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../src/dbg.h"
#include <glob.h>

int main(int argc, char *argv[])
{
    unsigned int i = 0;
    int rc = 0;
    int offset = 12;
    char *pattern = "tests/*_tests";
    char *template = "$VALGRIND ./ 2>>tests/tests.log";
    char *command = NULL;
    glob_t *globbuf = NULL;

    printf("Running unit tests:\n");

    globbuf = calloc(1, sizeof(glob_t));
    check_mem(globbuf);
    glob(pattern, 0, NULL, globbuf);

    for(i = 0; i < globbuf->gl_pathc; i++) {
       command = calloc(1, 75);
       strncpy(command, template, offset);
       command[offset] = '\0';
       strcat(command, globbuf->gl_pathv[i]);
       strcat(command, template + offset);
       rc = system(command);
       free(command);

       if (rc == 0) {
           printf("%s PASS\n", globbuf->gl_pathv[i]);
       } else {
           printf("ERROR in test %s: here's tests/tests.log\n", globbuf->gl_pathv[i]);
	   printf("------");
	   system("tail tests/tests.log");
	   exit(1);
       }
    }

    globfree(globbuf);
    free(globbuf);

    return 0;

error:
    if (globbuf) {
	globfree(globbuf);
        free(globbuf);
    }

    if (command) {
	free(command);
    }

    return 1;
}
