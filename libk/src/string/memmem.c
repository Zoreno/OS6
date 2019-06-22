/**
 * @file memmem.c
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

void *memmem(const void *haystack, size_t haystack_len,
             const void *needle, size_t needle_len)
{
    const char *last_possible =
        (const char *)haystack + haystack_len - needle_len;

    if (needle_len == 0)
    {
        return (void *)haystack;
    }

    if (haystack_len < needle_len)
    {
        return NULL;
    }

    for (const char *start = (const char *)haystack;
         start <= last_possible;
         ++start)
    {
        if (start[0] == ((const char *)needle)[0] &&
            !memcmp((const void *)&start[1],
                    (const void *)((const char *)needle + 1),
                    needle_len - 1))
        {
            return (void *)start;
        }
    }

    return NULL;
}