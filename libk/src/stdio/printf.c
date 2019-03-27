#include <stdio.h>

int printf(const char *format, ...)
{
    char buf[256] = {0};
    int ret;

    va_list ap;

    va_start(ap, format);

    ret = vsprintf(buf, format, ap);

    va_end(ap);

    puts(buf);

    return ret;
}