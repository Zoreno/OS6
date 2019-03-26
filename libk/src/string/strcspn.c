#include <string.h>

size_t strcspn(const char *str1, const char *reject)
{
    if (reject[0] == '\0' || reject[1] == '\0')
    {
        return strchr(str1, reject[0]) - str1;
    }

    unsigned char table[256];
    unsigned char *p = memset(table, 0, 256);

    unsigned char *s = (unsigned char *)reject;
    unsigned char tmp;

    do
    {
        p[tmp = *s++] = 1;
    } while (tmp);

    unsigned char *str = (unsigned char *)str1;
    s = (unsigned char *)str1;

    while (*s)
    {
        if (p[*s])
        {
            break;
        }

        ++s;
    }

    return s - str;
}