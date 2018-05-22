#include <stdio.h>

void print_pointers(int *ages, char **names, int count)
{
    int *cur_age = ages;
    char **cur_name = names;

    printf("Separate function print\n");

    while ((cur_age - ages) < count) {
        printf("%s lived for %d years\n", *cur_name, *cur_age);
	cur_name++;
	cur_age++;
    }
}

int main(int argc, char *argv[])
{
    // create two arrays we care about
    int ages[] = { 23, 43, 12, 89, 2 };
    char *names[] = {
        "Alan", "Frank",
	"Mary", "John", "Lisa"
    };

    // safely get the size of ages
    int count = sizeof(ages) / sizeof(int);
    int i = 0;
   
    // first way using indexing
    for (i = count - 1; i >= 0; i--) {
        printf("%s has %d years alive.\n", *(names + i), *(ages + i));
    }

    printf("---\n");

    // setup the pointers to the start of the arrays
    int *cur_age = ages;
    char **cur_name = names;

    // second way using pointers
    for (i = count - 1; i >= 0; i--) {
        printf("%s is %d years old.\n",
	        cur_name[i], cur_age[i]);
    }

    printf("---\n");

    // third way, pointers are just arrays
    for (i = count - 1; i >= 0; i--) {
        printf("%s is %d years old again.\n", *(cur_name + i), *(cur_age + i));
    }

    printf("---\n");

    // fourth way with pointers in a stupid complex way
    for (cur_name = names + count - 1, cur_age = ages + count - 1;
	    cur_age >= ages; cur_name--, cur_age--) {
        printf("%s lived %d years so far.\n", *cur_name, *cur_age);
    }

    printf("---\n");

    print_pointers(ages, names, count);

    printf("---\n");

    // printing out pointer addresses
    for (cur_name = names, cur_age = ages;
	    (cur_age - ages) < count; cur_age++, cur_name++) {
        printf("cur_name: %p, cur_age: %p\n", cur_name, cur_age);
    }

    return 0;
}
