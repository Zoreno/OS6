/**
 * @file memcpyw.c
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

void *memcpyw(void *str1, const void *str2, size_t n)
{
    uint16_t *destPtr = (uint16_t *)str1;
    uint16_t *srcPtr = (uint16_t *)str2;
    size_t words = n;

    for (size_t i = 0; i < words; ++i)
    {
        destPtr[i] = srcPtr[i];
    }

    return destPtr;
}
