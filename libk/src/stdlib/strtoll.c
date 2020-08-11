/**
 * @file strtoll.c
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-06-22
 * 
 * @brief 
 * 
 * @copyright Copyright (C) 2019,
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https: //www.gnu.org/licenses/>.
 * 
 */

#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <ctype.h>

long long int strtoll(const char *str, char **endptr, int base)
{
    const char *s = str;
    long long acc;
    int c;
    long long cutoff;
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

    cutoff = neg ? -(unsigned long)LLONG_MIN : LLONG_MAX;
    cutlim = cutoff % (unsigned long)base;
    cutoff /= (unsigned long)base;

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
        acc = neg ? LLONG_MIN : LLONG_MAX;
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
