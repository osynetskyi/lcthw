#include <stdio.h>

/* This is a comment. */
int main(int argc, char *argv[])
{
    int distance = 100;
    int test = (10 + 7) / 8;

    printf("test %d\n", test);
    // this is also a comment
    printf("this is %d times more complex than a hello world.\n", distance);
    printf("and this is a number 9: %d\n", 9);
    printf("You are %*d miles away.\n", 1, 3);

    return 0;
}
