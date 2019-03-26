#include <stdlib.h>

void ltoa(long int i, int base, char *buf)
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

    ultoa((unsigned long int)i, base, buf);
}