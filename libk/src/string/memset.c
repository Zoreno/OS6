#include <string.h>

void *memset(void *str, unsigned char c, size_t n)
{
    unsigned char *buf = (unsigned char *)str;
    unsigned char value = (unsigned char)c;
    size_t bytes = n;

    for (size_t i = 0; i < bytes; ++i)
    {
        buf[i] = value;
    }

    return str;
}