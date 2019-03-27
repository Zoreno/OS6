#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <ctype.h>

unsigned long long int strtoull(const char *str, char **endptr, int base)
{
    const char *s = str;
    unsigned long long acc;
    int c;
    unsigned long long cutoff;
    int neg = 0;
    int any;
    int cutlim;

    do
    {
        c = *s++;
    } while (isspace(c));

    if (c == '-')
    {
        neg = 1;
        c = *s++;
    }
    else if (c == '+')
    {
        c = *s++;
    }

    if ((base == 0 || base == 16) && c == '0' && (*s == 'x' || *s == 'X'))
    {
        c = s[1];
        s += 2;
        base = 16;
    }
    else if ((base == 0 || base == 2) && c == '0' && (*s == 'b' || *s == 'B'))
    {
        c = s[1];
        s += 2;
        base = 2;
    }

    else if (base == 0)
    {
        if (c == '0')
        {
            base = 8;
        }
        else
        {
            base = 10;
        }
    }

    cutoff = (unsigned long)ULLONG_MAX / (unsigned long)base;
    cutlim = (unsigned long)ULLONG_MAX % (unsigned long)base;

    for (acc = 0, any = 0;; c = *s++)
    {
        if (isdigit(c))
        {
            c -= '0';
        }
        else if (isalpha(c))
        {
            if (isupper(c))
            {
                c -= 'A' - 10;
            }
            else
            {
                c -= 'a' - 10;
            }
        }
        else
        {
            break;
        }

        if (c >= base)
        {
            break;
        }

        if ((any < 0) || (acc > cutoff) || ((acc == cutoff) && (c > cutlim)))
        {
            any = -1;
        }
        else
        {
            any = 1;
            acc *= base;
            acc += c;
        }
    }

    if (any < 0)
    {
        acc = ULLONG_MAX;
    }

    else if (neg)
    {
        acc = -acc;
    }

    if (endptr != 0)
    {
        *endptr = (char *)(any ? (s - 1) : str);
    }

    return acc;
}