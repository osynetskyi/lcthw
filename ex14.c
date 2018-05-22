#include <stdio.h>
#include <ctype.h>
#include <string.h>

void print_letters(char arg[], int len)
{
    int i = 0;

    for (i = 0; arg[i] != '\0' && i < len; i++) {
        char ch = arg[i];

	if (isdigit((int)ch) || isalpha((int)ch)) {
            printf("'%c' == %d ", ch, ch);
	}
    }

    printf("\n");
}

int main(int argc, char *argv[])
{
    int i = 0;
    int len;
    for (; i < argc; i++) {
	len = strlen(argv[i]);
        print_letters(argv[i], len);
    }

    return 0;
}
