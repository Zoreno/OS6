#include <debug/backtrace.h>

#include <stdint.h>
#include <stdio.h>

#include <exec/elf64.h>

void print_backtrace(unsigned long long int *reg)
{
    int depth = 0;

    while (*reg)
    {
        printf("bt [%i]: ", depth);

        kernel_lookup_symbol((void *)*(reg + 1));

        printf("\n");

        reg = (unsigned long long int *)*reg;

        ++depth;
    }
}

void backtrace()
{
#ifndef OS6_DEBUG

    printf("Backtrace not available in release build\n");

#else

    register unsigned long long int *_rbp __asm__("rbp");

    print_backtrace(_rbp);

#endif
}