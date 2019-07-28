/**
 * @file memccpy.c
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
#include <stdint.h>

void *memccpy(void *dest, const void *src, int c, size_t n)
{
    uint8_t *destPtr = (uint8_t *)dest;
    uint8_t *srcPtr = (uint8_t *)src;
    uint8_t cVal = (uint8_t)c;
    size_t bytes = n;

    for (size_t i = 0; i < bytes; ++i)
    {
        destPtr[i] = srcPtr[i];

        if (srcPtr[i] == cVal)
        {
            return srcPtr + i + 1;
        }
    }

    return NULL;
}