#include <stdio.h>

int main(int argc, char *argv[])
{
    int i = 0;
    int num_states = 4;
    
    char *states[] = {
        "California", "Oregon",
	NULL, "Texas"
    };

    argv[1] = states[1];
    states[2] = argv[2];
	

    // go through each string in argv
    // why am I skipping argv[0]?
    for (i = 1; i < argc; i++) {
        printf("arg %d: %s\n", i, argv[i]);
    }

    //let's make our own array of strings
    /*char *states[] = {
        "California", "Oregon",
	NULL, "Texas"
    };*/
    
    i = -1;

    for (; i++,i < num_states;) {
        printf("state %d: %s\n", i, states[i]);
    }

    return 0;
}

