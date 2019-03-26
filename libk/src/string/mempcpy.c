#include <string.h>
#include <stdint.h>

void *mempcpy(void *str1, const void *str2, size_t n)
{
    uint8_t *destPtr = (uint8_t *)str1;
    uint8_t *srcPtr = (uint8_t *)str2;
    size_t bytes = n;

    for (size_t i = 0; i < bytes; ++i)
    {
        destPtr[i] = srcPtr[i];
    }

    return destPtr + n;
}
