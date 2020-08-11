/**
 * @file uitoa.c
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

static char bchars[] = {'0', '1', '2', '3',
                        '4', '5', '6', '7',
                        '8', '9', 'A', 'B',
                        'C', 'D', 'E', 'F'};

void uitoa(unsigned int i, int base, char *buf)
{
    int pos = 0;
    int opos = 0;
    int top = 0;

    char tbuf[64] = {0};

    if ((!i) || (base > 16) || (base == 0))
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

    for (opos = 0; opos < top; pos--, opos++)
    {
        buf[opos] = tbuf[pos];
    }

    buf[opos] = 0;
}
