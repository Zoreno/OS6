#include <string.h>

void *memchr(const void *str, int c, size_t n)
{
    unsigned char *ptr = (unsigned char *)str;
    unsigned char val = (unsigned char)c;
    size_t i;

    for (i = 0; i < n; ++i, ++ptr)
    {
        if (*ptr == val)
        {
            return ptr;
        }
    }

    return NULL;
}