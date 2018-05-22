#include <stdio.h>
#include <stdint.h>

int main(int argc, char *argv[])
{
    uint32_t i = 0;
    printf("Size of i: %d, value: %d\n", sizeof(i), i);
    printf("Max size available in the system: %02x\n", UINTMAX_MAX);
    printf("Max signed available: %02x\n", INTMAX_MAX);

    return 0;
}
