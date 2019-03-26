#include <stdlib.h>

void itoa(int i, int base, char *buf)
{
    if (base > 16)
    {
        buf[0] = '0';
        buf[1] = '\0';
        return;
    }

    if (i < 0)
    {
        *buf++ = '-';
        i *= -1;
    }

    uitoa((unsigned int)i, base, buf);
}