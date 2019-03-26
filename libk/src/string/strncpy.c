#include <string.h>

char *strncpy(char *str1, const char *str2, size_t n)
{
    size_t size = strnlen(str2, n);

    if (size != n)
    {
        memset(str1 + size, '\0', n - size);
    }

    return memcpy(str1, str2, size);
}