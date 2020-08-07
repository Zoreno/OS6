#include <stdlib.h>

// TODO: Implement a proper heap
unsigned char mem_area[10 * 1024];

void *malloc(size_t size)
{
    static unsigned char *mem_base = mem_area;

    size = ((size + 7) / 8) * 8;

    mem_base += size;

    return mem_base - size;
}