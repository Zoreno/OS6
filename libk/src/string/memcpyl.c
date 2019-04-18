#include <string.h>
#include <stdint.h>

void *memcpyl(void *str1, const void *str2, size_t n)
{
    uint32_t *destPtr = (uint32_t *)str1;
    uint32_t *srcPtr = (uint32_t *)str2;
    size_t dwords = n;

    for (size_t i = 0; i < dwords; ++i)
    {
        destPtr[i] = srcPtr[i];
    }

    return destPtr;
}
