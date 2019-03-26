#include <string.h>

size_t strnlen(const char *str, size_t maxlen)
{
    if (maxlen == 0)
    {
        return 0;
    }

    size_t i;

    for (i = 0; i < maxlen; ++i)
    {
        if (!(*str))
        {
            return i;
        }
    }

    return maxlen;
}