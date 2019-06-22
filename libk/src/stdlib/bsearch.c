/**
 * @file bsearch.c
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

void *bsearch(
    const void *key,
    const void *base,
    size_t nitems,
    size_t size,
    int (*compar)(const void *, const void *))
{
    void *guess;
    int dir;

    while (nitems > 0)
    {
        guess = (char *)base + size * (nitems / 2);

        dir = compar(key, guess);

        if (!dir)
        {
            return guess;
        }

        else if (nitems == 1)
        {
            break;
        }
        else if (dir < 0)
        {
            nitems /= 2;
        }
        else
        {
            base = guess;
            nitems -= nitems / 2;
        }
    }

    return NULL;
}