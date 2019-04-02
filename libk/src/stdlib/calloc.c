#include <stdlib.h>
#include <stdint.h>
#include <string.h>

void *calloc(size_t nitems, size_t size)
{
    void *mem = malloc(nitems * size);

    if (mem)
    {
        memset(mem, 0, nitems * size);
    }

    return mem;
}