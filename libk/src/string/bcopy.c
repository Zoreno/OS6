#include <string.h>

void bcopy (const void *src, void *dest, size_t length)
{
    memmove (dest, src, length);
}
