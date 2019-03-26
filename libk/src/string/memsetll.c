#include <string.h>

void *memsetll(void *str, unsigned long long int c, size_t n)
{
    unsigned long long int *buf = (unsigned long long int *)str;
    unsigned long long int value = (unsigned long long int)c;
    size_t dwords = n;

    for (size_t i = 0; i < dwords; ++i)
    {
        buf[i] = value;
    }

    return str;
}