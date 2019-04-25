/**
 * @file wcstok.c
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-04-26
 * 
 * @brief Wide character tokenization
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

#include <wchar.h>

wchar_t *wcstok(wchar_t *str, const wchar_t *delim, wchar_t **saveptr)
{
    wchar_t *token;

    if (str == NULL)
    {
        str = *saveptr;
    }

    str += wcsspn(str, delim);

    if (*str == '\0')
    {
        *saveptr = str;
        return NULL;
    }

    token = str;
    str = wcspbrk(token, delim);

    if (str == NULL)
    {
        *saveptr = (wchar_t *)wcschr(token, '\0');
    }
    else
    {
        *str = '\0';
        *saveptr = str + 1;
    }

    return token;
}