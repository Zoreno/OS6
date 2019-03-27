#include <stdio.h>

int putchar(char c)
{
    if (!stdout)
    {
        return -1;
    }

    return fputc(c, stdout);
}