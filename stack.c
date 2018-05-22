#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>

struct Stack {
    int size;
    int top;
    int *values;
};

void Stack_delete(struct Stack *stack);

void die(char *message, struct Stack *stack)
{
    if (errno) {
        perror(message);
    } else {
        printf("ERROR: %s\n", message);
    }

    Stack_delete(stack);
    exit(1);
}

struct Stack *Stack_create(int size)
{
    struct Stack *stack = malloc(sizeof(struct Stack));
    if (!stack)
	die("Memory error", stack);

    stack->size = size;
    stack->top = 0;
    stack->values = malloc(sizeof(int) * stack->size);

    return stack;
}

void Stack_delete(struct Stack *stack)
{
    if (stack) {
        if (stack->values) {
	    free(stack->values);
	}
        free(stack);
    }
}

void Stack_push(struct Stack *stack, int item)
{
    if (stack->top >= stack->size) {
        die("Stack overflow", stack);
    } else {
        stack->values[stack->top] = item;
        stack->top += 1;
    }
}

int Stack_pop(struct Stack *stack)
{

    if (stack->top == 0) {
        die("Stack is empty", stack);
    }

    int res = stack->values[stack->top - 1];
    stack->top -= 1;
    return res;
}

int main(int argc, char *argv[])
{
    struct Stack *test = Stack_create(5);
    int i = 0;
    srand(time(NULL));
    for (i = 0; i < 5; i++) {
	int item = rand();
	Stack_push(test, item);
	printf("Pushed %d\n", item);
    }
    //Stack_push(test, 10);
    for (i = 0; i < 5; i++) {
        printf("Popped %d\n", Stack_pop(test));
    }
    //int res = Stack_pop(test);
    //printf("Popped %d\n", res);
    Stack_delete(test);
    return 0;
}
