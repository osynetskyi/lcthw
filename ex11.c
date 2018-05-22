#include <stdio.h>

int main(int argc, char *argv[])
{
    int numbers[4] = { 1 };
    char name[4] = { 'a', 'b', 'c', 'd' };
    unsigned int crazy;

    // first, print them out raw
    printf("numbers: %d %d %d %d\n",
	    *(numbers), *(numbers + 1), *(numbers + 2),  *(numbers + 3));

    printf("name each: %c %c %c %c\n",
	    *name, *(name + 1), *(name + 2), *(name + 3));

    printf("name: %s\n", name);

    // setup the numbers
    numbers[0] = 1;
    numbers[1] = 2;
    numbers[2] = 3;
    numbers[3] = 4;

    // setup the name
    name[0] = 'Z';
    name[1] = 'e';
    name[2] = 'd';
    name[3] = '\0';

    // then print them out initialized
    printf("numbers: %c %c %c %c\n",
	    numbers[0], numbers[1], numbers[2], numbers[3]);

    printf("name each: %d %d %d %d\n",
            name[0], name[1], name[2], name[3]);

    // print the name like a string
    printf("name: %s\n", name);

    // another way to use name
    char *another = "Zed";
    
    printf("another: %s\n", another);

    printf("another each: %c %c %c %c\n",
            another[0], another[1], another[2], another[3]);

    //name += 100;
    printf("crazy hack: %d\n", name + 260);
    printf("crazy name: %s\n", name + 260); 

    crazy = 1 * 256 * 256 * 256;

    printf("crazy num: %d\n", crazy);
    //printf("crazy str: %s\n", crazy);

    return 0;
}
