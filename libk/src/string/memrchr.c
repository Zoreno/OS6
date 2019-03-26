#include <string.h>

// TODO: Test this function to make sure that the limits are correct
void *memchr(const void *str, int c, size_t n)
{
    unsigned char *ptr = ((unsigned char *)str) + n - 1;
    unsigned char val = (unsigned char)c;
    size_t i;

    for (i = 0; i < n; ++i, --ptr)
    {
        if (*ptr == val)
        {
            return ptr;
        }
    }

    return NULL;
}