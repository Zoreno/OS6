#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

extern void *kmalloc(size_t size);

void *malloc(size_t size)
{
    //printf("[Malloc]: %i bytes\n", size);

    return kmalloc(size);
}