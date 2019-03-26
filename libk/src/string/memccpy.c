#include <string.h>
#include <stdint.h>

void *memccpy(void *dest, const void *src, int c, size_t n)
{
    uint8_t *destPtr = (uint8_t *)dest;
    uint8_t *srcPtr = (uint8_t *)src;
    uint8_t cVal = (uint8_t)c;
    size_t bytes = n;

    for (size_t i = 0; i < bytes; ++i)
    {
        destPtr[i] = srcPtr[i];

        if (srcPtr[i] == cVal)
        {
            return srcPtr + i + 1;
        }
    }

    return NULL;
}