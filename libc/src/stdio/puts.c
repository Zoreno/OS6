#include <stdio.h>

#include <_syscall.h>

int puts(const char *string)
{
    return do_syscall1(0, (int64_t)string);
}