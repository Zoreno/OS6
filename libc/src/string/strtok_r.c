/**
 * @file strtok_r.c
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

static size_t lfind(const char *str, const char accept)
{
    size_t i = 0;

    while (str[i] != accept)
    {
        ++i;
    }

    return (size_t)(str) + i;
}

char *strtok_r(char *str, const char *delim, char **saveptr)
{
    char *token;

    if (str == NULL)
    {
        str = *saveptr;
    }

    str += strspn(str, delim);

    if (*str == '\0')
    {
        *saveptr = str;
        return NULL;
    }

    token = str;

    str = strpbrk(token, delim);

    if (str == NULL)
    {
        *saveptr = (char *)lfind(token, '\0');
    }
    else
    {
        *str = '\0';
        *saveptr = str + 1;
    }

    return token;
}