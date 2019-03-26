#include <string.h>
#include <stdint.h>

char *strchr(const char *str, int c)
{
    char val = (char)c;

    while (*str)
    {
        if (*str == val)
        {
            return (char *)str;
        }

        ++str;
    }

    return NULL;
}