#include <string.h>

size_t strspn(const char *str, const char *accept)
{
    const char *ptr;
    const char *acc;

    size_t size = 0;
    for (ptr = str; *ptr != '\0'; ++ptr)
    {
        for (acc = accept; *acc != '\0'; ++acc)
        {
            if (*ptr == *acc)
            {
                break;
            }
        }

        if (*acc == '\0')
        {
            return size;
        }
        else
        {
            ++size;
        }
    }

    return size;
}