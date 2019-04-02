#include <stdlib.h>
#include <stdint.h>
#include <string.h>

extern void *kernel_malloc(size_t size);

void *malloc(size_t size)
{
    return kernel_malloc(size);
}