/**
 * @file memrchr.c
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

// TODO: Test this function to make sure that the limits are correct
void *memchr(const void *str, int c, size_t n)
{
    unsigned char *ptr = ((unsigned char *)str) + n - 1;
    unsigned char val = (unsigned char)c;
    size_t i;

    for (i = 0; i < n; ++i, --ptr)
    {
        if (*ptr == val)
        {
            return ptr;
        }
    }

    return NULL;
}