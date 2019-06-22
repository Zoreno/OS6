/**
 * @file strcspn.c
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

#include <string.h>

size_t strcspn(const char *str1, const char *reject)
{
    if (reject[0] == '\0' || reject[1] == '\0')
    {
        return strchr(str1, reject[0]) - str1;
    }

    unsigned char table[256];
    unsigned char *p = memset(table, 0, 256);

    unsigned char *s = (unsigned char *)reject;
    unsigned char tmp;

    do
    {
        p[tmp = *s++] = 1;
    } while (tmp);

    unsigned char *str = (unsigned char *)str1;
    s = (unsigned char *)str1;

    while (*s)
    {
        if (p[*s])
        {
            break;
        }

        ++s;
    }

    return s - str;
}