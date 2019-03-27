#include <stdio.h>
#include <string.h>

int fputs(const char *str, FILE *file)
{

    if (!file || !str)
    {
        return -1;
    }

    if (!file->write)
    {
        return -1;
    }

    char c;

    while ((c = *str++) != '\0')
    {
        file->write(c);
    }

    return 0;
}
