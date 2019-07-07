/**
 * @file strtod.c
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-07-07
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

#include <ctype.h>
#include <math.h>

#include "../../include/__libk_symbols.h"

static const char *skipwhite(const char *q)
{
    const char *p = q;
    while (isspace(*p))
    {
        ++p;
    }

    return p;
}

// Format: [+-]?\d*(\.\d+)?((e|E)[+-]?\d+)?

double libk_strtod(const char *str, char **endptr)
{
    double d = 0.0;
    int sign;
    int n = 0;
    const char *p;
    const char *a;

    a = p = str;
    p = skipwhite(p);

    sign = 1;

    // Consume sign if present
    if (*p == '-')
    {
        sign = -1;
        ++p;
    }
    else if (*p == '+')
    {
        ++p;
    }

    // Consume integer part if present
    if (isdigit(*p))
    {
        d = (double)(*p++ - '0');

        while (*p && isdigit(*p))
        {
            d = d * 10.0 + (double)(*p - '0');
            ++p;
            ++n;
        }
        a = p;
    }
    else if (*p != '.')
    {
        goto done;
    }

    d *= sign;

    // Consume fraction part if present
    if (*p == '.')
    {
        double f = 0.0;
        double base = 0.1;
        ++p;

        while (*p && isdigit(*p))
        {
            f += base * (*p - '0');
            base /= 10.0;
            ++p;
            ++n;
        }

        d += f * sign;
        a = p;
    }

    // Consume exponential part if present
    if ((*p == 'E') || (*p == 'e'))
    {
        int e = 0;
        ++p;

        // Get sign if any
        sign = 1;

        if (*p == '-')
        {
            sign = -1;
            ++p;
        }
        else if (*p == '+')
        {
            ++p;
        }

        if (isdigit(*p))
        {
            // Get leading zeroes
            while (*p == '0')
            {
                ++p;
            }

            e = (int)(*p++ - '0');

            while (*p && isdigit(*p))
            {
                e = e * 10 + (int)(*p - '0');
                ++p;
            }

            e *= sign;
        }
        else if (!isdigit(*(a - 1)))
        {
            a = str;
            goto done;
        }
        else if (*p == 0)
        {
            goto done;
        }

        if (d == 2.2250738585072011 && e == -308)
        {
            d = 0.0;
            a = p;
            goto done;
        }

        if (d == 2.2250738585072012 && e <= -308)
        {
            d *= 1.0e-308;
            a = p;
            goto done;
        }

        // TODO: Change to pow when implemented

#ifdef _LIBK
#define pow powf
#endif

        d *= pow(10.0, (double)e);

        a = p;
    }
    else if (p > str && isdigit(*(p - 1)))
    {
        a = str;
    }

done:

    if (endptr)
    {
        *endptr = (char *)a;
    }

    return d;
}

weak_alias(libk_strtod, strtod);

//==============================================================================
// End of file
//==============================================================================
