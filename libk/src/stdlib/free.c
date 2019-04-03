#include <stdlib.h>
#include <stdint.h>
#include <string.h>

extern void kfree(void *addr);

void free(void *ptr)
{
    kfree(ptr);
}