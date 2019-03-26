#include <string.h>

void *memsetl(void *str, unsigned int c, size_t n)
{
    unsigned int *buf = (unsigned int *)str;
    unsigned int value = (unsigned int)c;
    size_t dwords = n;

    for (size_t i = 0; i < dwords; ++i)
    {
        buf[i] = value;
    }

    return str;
}