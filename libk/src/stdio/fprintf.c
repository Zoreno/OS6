#include <stdio.h>

int fprintf(FILE *file, const char *format, ...)
{
    char buf[256] = {0};
    int ret;

    va_list ap;

    va_start(ap, format);

    ret = vsprintf(buf, format, ap);

    va_end(ap);

    fputs(buf, file);

    return ret;
}