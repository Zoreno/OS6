#include <string.h>

void *memfrob(void *s, size_t n)
{
    char *p = (char *)s;

    while (n-- > 0)
    {
        *p++ ^= 42;
    }

    return s;
}