#include <stdio.h>

int puts(const char *str)
{
    if (!stdout)
    {
        return -1;
    }

    return fputs(str, stdout);
}