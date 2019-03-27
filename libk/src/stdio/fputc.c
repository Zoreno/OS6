#include <stdio.h>

int fputc(char c, FILE *file)
{
    if (!file)
    {
        return -1;
    }

    if (!file->write)
    {
        return -1;
    }

    file->write(c);

    return 0;
}