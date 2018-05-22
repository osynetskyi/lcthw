#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <glob.h>
#include <assert.h>
#include "dbg.h"

typedef int (*search_fcn)(char **terms, int count, char *string);

void die(const char *message) 
{
    if (errno) {
        perror(message);
    } else {
        printf("ERROR: %s\n", message);
    }

    exit(1);
}

// checks if string contains all the terms, returns 1 on success,
// 0 otherwise and -1 on error
int search_and(char **terms, int count, char *string)
{
    check(terms != NULL && string != NULL, "Invalid search terms or string.");
    int i = 0;
    for (i = 0; i < count; i++) {
       if (strstr(string, terms[i]) == NULL) {
           return 0;
       }
    }

    return 1;

error:
    return -1;
}

// checks if string contains any of the terms, returns 1 on success
// and 0 otherwise
int search_or(char **terms, int count, char *string)
{
    check(terms != NULL && string != NULL, "Invalid search terms or string.");
    int i = 0;
    for (i = 0; i < count; i++) {
        if(strstr(string, terms[i]) != NULL) {
	    return 1;
	}		
    }

    return 0;

error:
    return -1;
}

int scan_file(char **terms, int count, char *filename, search_fcn search)
{
    FILE *infile;
    char *line = NULL;
    unsigned int len = 0;
    ssize_t read;
    int line_num = 1;
    int found = 0;

    check(filename != NULL, "Invalid file name.");
    check(terms != NULL, "Nothing to search for.");
    check(search != NULL, "No search function.");
    
    infile = fopen(filename, "r");
    check(infile != NULL, "Couldn't open file.");

    while ((read = getline(&line, &len, infile)) != -1) {
	if (strcmp(line, "\n") == 0) {
	    continue;
	}
	line = strtok(line, "\n");
	found = search(terms, count, line);
	check(found != -1, "String search failed");
	if (found) {
            printf("%s:%d:%s\n", filename, line_num, line);
	}
	line_num += 1;
    }

    fclose(infile);
    if (line) {
        free(line);
    }

    return 0;

error:
    if (line) {
        free(line);
    }
    return -1;
}

int find(char **terms, int count, search_fcn search)
{
    FILE *config;
    char *line = NULL;
    unsigned int len = 0;
    ssize_t read;
    int res;
    int rc;
    int i = 0;
    glob_t *globbuf = NULL;

    check(terms != NULL, "Nothing to search for.");
    check(search != NULL, "No search function.");

    config = fopen(".logfind", "r");
    check(config != NULL, "Can't open config file.");

    while ((read = getline(&line, &len, config)) != -1) {
	 if (strcmp(line, "\n") == 0) {
	     continue;
	 }
         line = strtok(line, "\n");
	 globbuf = calloc(1, sizeof(glob_t));
	 check_mem(globbuf);
	 rc = glob(line, 0, NULL, globbuf);
	 check((rc == 0 || rc == GLOB_NOMATCH), "Glob failed.");
	 for (i = 0; i < globbuf->gl_pathc; i++) {
	     res = scan_file(terms, count, globbuf->gl_pathv[i], search);
	     check(res == 0, "Search failed in %s", globbuf->gl_pathv[i]);
	 }
	 globfree(globbuf);
	 free(globbuf);
    }

    fclose(config);
    if (line) {
        free(line);
    }
    
    return 0;

error:
    if (line) {
        free(line);
    }
    if (globbuf) {
        globfree(globbuf);
	free(globbuf);
    }

    return -1;
}

int main(int argc, char *argv[])
{
    char **terms;
    int count;
    int res = 0;
    search_fcn search;

    if (!(argc > 1)) {
        die("USAGE: ./logfind [-o] term1 [term2 ...]");
    }

    if (strcmp(argv[1], "-o") == 0) {
	if (argc == 2) {
	    die("USAGE: ./logfind [-o] term1 [term2 ...]");
	}
	terms = &argv[2];
	count = argc - 2;
	search = search_or;
    } else {
	terms = &argv[1];
	count = argc - 1;
	search = search_and;
    }

    res = find(terms, count, search);
    check(res == 0, "Search failed.");

    return 0;

error:
    return -1;
}
