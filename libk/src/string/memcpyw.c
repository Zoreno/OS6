#include <string.h>
#include <stdint.h>

void *memcpyw(void *str1, const void *str2, size_t n)
{
    uint16_t *destPtr = (uint16_t *)str1;
    uint16_t *srcPtr = (uint16_t *)str2;
    size_t words = n;

    for (size_t i = 0; i < words; ++i)
    {
        destPtr[i] = srcPtr[i];
    }

    return destPtr;
}
