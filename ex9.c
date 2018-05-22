#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    int i;
    if (argc < 2) {
        printf("Not enough arguments provided!");
    } else {
	i = atoi(argv[1]);
        while (i > 0) {
            printf("%d ", i);
	    i--;
        }
    }
    // need this to add a final newline
    printf("\n");

    return 0;
}
