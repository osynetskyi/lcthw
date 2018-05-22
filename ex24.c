#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dbg.h"

#define MAX_DATA 100

typedef enum EyeColor {
    BLUE_EYES, GREEN_EYES, BROWN_EYES,
    BLACK_EYES, OTHER_EYES
} EyeColor;

const char *EYE_COLOR_NAMES[] = {
    "Blue", "Green", "Brown", "Black", "Other"
};

typedef struct Person {
    int age;
    char first_name[MAX_DATA];
    char last_name[MAX_DATA];
    EyeColor eyes;
    float income;
} Person;

void strip(char *name)
{
    int cur = 0;
    int length = strlen(name);
    while ((cur < length) && (name[cur] != '\0')) {
        if ((name[cur] == ' ') || (name[cur] == '\n')) {
	    memmove(&name[cur], &name[cur+1], length - cur);
	    cur -= 1;
	}

	cur += 1;
    }
}

int name_by_char(char *name, int count)
{
    int cur = 0;
    int rc;
    char read;
    printf("Enter one character a time, end with dot (.)\n");
    while (cur < count - 1) {
        rc = scanf(" %c", &read);
	check(rc > 0, "Need a character.");
	//printf("You've entered %c\n", read);
	if (read != '.') {
	    name[cur] = read;
	    cur += 1;
	} else {
	    cur += 1;
	    break;
	}
    }
    
    name[cur] = '\0';

    return cur;

error:
    return -1;
}

int main(int argc, char *argv[])
{
    Person you = {.age = 0 };
    int i = 0;
    int rc;
    char *in = NULL;
    //char *str = malloc(sizeof(char) * MAX_DATA);

    printf("What's your First Name? ");
    in = fgets(you.first_name, MAX_DATA - 1, stdin);
    //rc = fscanf(stdin, "%50s", you.first_name);
    check(in != NULL, "Failed to read first name.");
    strip(you.first_name);

    printf("What's your Last Name? ");
    rc = name_by_char(you.last_name, MAX_DATA);
    check(rc > 0, "Failed to read last name.");
    /*in = fgets(you.last_name, MAX_DATA - 1, stdin);
    check(in != NULL, "Failed to read last name.");*/
    strip(you.last_name);

    printf("How old are you? ");
    rc = fscanf(stdin, "%d", &you.age);
    //rc = scanf("%d", &you.age);
    check(rc > 0, "You have to enter a number.");
    /*in = fgets(str, MAX_DATA - 1, stdin);
    check(in != NULL, "Failed to read age.");
    you.age = atoi(str);*/

    printf("What color are your eyes:\n");
    for (i = 0; i <= OTHER_EYES; i++) {
        printf("%d) %s\n", i + 1, EYE_COLOR_NAMES[i]);
    }
    printf("> ");

    int eyes = -1;
    rc = fscanf(stdin, "%d", &eyes);
    //rc = scanf("%d", &eyes);
    check(rc > 0, "You have to enter a number.");
    /*in = fgets(str, MAX_DATA - 1, stdin);
    check(in != NULL, "Failed to read eye color.");
    eyes = atoi(str);*/

    you.eyes = eyes - 1;
    check(you.eyes <= OTHER_EYES
	    && you.eyes >= 0, "Do it right, that's not an option.");

    printf("How much do you make an hour? ");
    rc = fscanf(stdin, "%f", &you.income);
    //rc = scanf("%f", &you.income);
    check(rc > 0, "Enter a floating point number.");
    /*in = fgets(str, MAX_DATA - 1, stdin);
    check(in != NULL, "Failed to read income.");
    you.income = atof(str);*/

    printf("----- RESULTS -----\n");

    printf("First Name: %s\n", you.first_name);
    printf("Last Name: %s\n", you.last_name);
    printf("Age: %d\n", you.age);
    printf("Eyes: %s\n", EYE_COLOR_NAMES[you.eyes]);
    printf("Income: %f\n", you.income);

    //free(str);
    return 0;
error:

    //free(str);
    return -1;
}
