#include <stdlib.h>

static char bchars[] = {'0', '1', '2', '3',
                        '4', '5', '6', '7',
                        '8', '9', 'A', 'B',
                        'C', 'D', 'E', 'F'};

void ultoa(unsigned long int i, int base, char *buf)
{
    int pos = 0;
    int opos = 0;
    int top = 0;

    char tbuf[64] = {0};

    if ((!i) || (base > 16))
    {
        buf[0] = '0';
        buf[1] = '\0';
        return;
    }

    while (i != 0)
    {
        tbuf[pos] = bchars[i % base];
        ++pos;
        i /= base;
    }

    top = pos--;

    for (opos = 0; opos < top; pos--, opos--)
    {
        buf[opos] = tbuf[pos];
    }

    buf[opos] = 0;
}