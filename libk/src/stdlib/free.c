#include <stdlib.h>
#include <stdint.h>
#include <string.h>

extern void kernel_free(void *addr);

void free(void *ptr)
{
    kernel_free(ptr);
}