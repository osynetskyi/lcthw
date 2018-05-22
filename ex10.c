#include <stdio.h>

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("ERROR: You need at least one argument.\n");
	// this is how you abort a program
	return 1;
    }

    int i = 0;
    int j;
    for (j = 1; j < argc; j++) {
        for (i = 0; *(*(argv + j) + i) != '\0'; i++) {
            char letter = ',';
	    letter = *(*(argv + j) + i);

	    if (letter > 64 && letter < 91) {
	        letter += 32;
	    }

	    /*switch (letter) {
	        case 'a':
	            printf("%d: 'A'\n", i);
		    break;

	        case 'e':
		    printf("%d: 'E'\n", i);
		    break;

	        case 'i':
		    printf("%d: 'I'\n", i);
		    break;

	        case 'o':
		    printf("%d: 'O'\n", i);
		    break;

	        case 'u':
		    printf("%d: 'U'\n", i);
		    break;

	        case 'y':
		    // why i > 2? is this a bug?
		    if (i > 2) {
		        // it's only sometimes Y
		        printf("%d: 'Y'\n", i);
			break;
		    }
		    //break;

	        default:
		    printf("%d: %c is not a vowel\n", i, letter);
		*/

	        if (letter == 'a') {
		    printf("%d: 'A'\n", i);
		} else if (letter == 'e') {
		    printf("%d: 'E'\n", i);
		} else if (letter == 'i') {
		    printf("%d: 'I'\n", i);
		} else if (letter == 'o') {
		    printf("%d: 'O'\n", i);
		} else if (letter == 'u') {
		    printf("%d: 'U'\n", i);
		} else if (letter == 'y') {
		    if (i > 2) {
	                printf("%d: 'Y'\n", i);
		    }
		} else {
		    printf("%d: %c is not a vowel\n", i, letter);
		}
	   // }
        }
    }

    return 0;
}
	    
