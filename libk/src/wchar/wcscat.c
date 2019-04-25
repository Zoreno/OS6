/**
 * @file wcscat.c
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-04-26
 * 
 * @brief Wide char concaternation
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

wchar_t *wcscat(wchar_t *dest, const wchar_t *src)
{
    wchar_t *end = dest;

    // read past the first string
    while (*end != 0)
    {
        ++end;
    }

    // Copy source to dest + strlen(dest)
    while (*src)
    {
        *end = *src;
        ++end;
        ++src;
    }

    // Null terminate
    *end = 0;

    return dest;
}
