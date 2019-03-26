#include <string.h>

void *memsetw(void *str, unsigned short c, size_t n)
{
    unsigned short *buf = (unsigned short *)str;
    unsigned short value = (unsigned short)c;
    size_t words = n;

    for (size_t i = 0; i < words; ++i)
    {
        buf[i] = value;
    }

    return str;
}