#include <stdint.h>
#include <stdio.h>

extern void (*__preinit_array_start[])(void) __attribute__((weak));
extern void (*__preinit_array_end[])(void) __attribute__((weak));
extern void (*__init_array_start[])(void) __attribute__((weak));
extern void (*__init_array_end[])(void) __attribute__((weak));

void __libc_init_array(void)
{
    size_t count;
    size_t i;

    count = __preinit_array_end - __preinit_array_start;

    printf("Calling %i preinit functions\n", count);

    for (int i = 0; i < count; ++i)
    {
        if (__preinit_array_start[i])
        {
            __preinit_array_start[i]();
        }
    }

    count = __init_array_end - __init_array_start;

    printf("Calling %i init functions\n", count);

    for (int i = 0; i < count; ++i)
    {
        if (__init_array_start[i])
        {
            __init_array_start[i]();
        }
    }
}