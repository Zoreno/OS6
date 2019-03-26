#include <string.h>

void *memmem(const void *haystack, size_t haystack_len,
             const void *needle, size_t needle_len)
{
    const char *last_possible =
        (const char *)haystack + haystack_len - needle_len;

    if (needle_len == 0)
    {
        return (void *)haystack;
    }

    if (haystack_len < needle_len)
    {
        return NULL;
    }

    for (const char *start = (const char *)haystack;
         start <= last_possible;
         ++start)
    {
        if (start[0] == ((const char *)needle)[0] &&
            !memcmp((const void *)&start[1],
                    (const void *)((const char *)needle + 1),
                    needle_len - 1))
        {
            return (void *)start;
        }
    }

    return NULL;
}