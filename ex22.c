#include <stdio.h>
#include "ex22.h"
#include "dbg.h"

int THE_SIZE = 1000;

static int THE_AGE = 37;

int get_age()
{
    return THE_AGE;
}

void set_age(int age)
{
    THE_AGE = age;
}

double *update_ratio(double new_ratio)
{
    static double ratio = 1.4;

    double *old_ratio = &ratio;
    ratio = new_ratio;

    return old_ratio;
}

void print_size()
{
    log_info("I think size is: %d", THE_SIZE);
}

int pass_by_value(int arg)
{
    return arg * 2;
}

void pass_by_reference(int *arg)
{
    *arg *= 2;
}

void recursive_overflow(int lol)
{
    int placeholder = 1;
    placeholder += 1;

    recursive_overflow(lol + 1);
}
