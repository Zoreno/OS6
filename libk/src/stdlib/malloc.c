#include <stdlib.h>
#include <stdint.h>
#include <string.h>

extern void *kmalloc(size_t size);

void *malloc(size_t size)
{
    return kmalloc(size);
}