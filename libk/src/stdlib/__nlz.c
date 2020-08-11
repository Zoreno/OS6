/**
 * @file __nlz.c
 * @author Joakim Bertils
 * @version 0.1
 * @date 2020-08-10
 * 
 * @brief 
 * 
 * @copyright Copyright (C) 2020,
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

int __nlz(uint32_t x)
{
    int n = 0;

    if (x <= 0x0000FFFF)
    {
        n += 16;
        x <<= 16;
    }

    if (x <= 0x00FFFFFF)
    {
        n += 8;
        x <<= 8;
    }

    if (x <= 0x0FFFFFFF)
    {
        n += 4;
        x <<= 4;
    }

    if (x <= 0x7FFFFFFF)
    {
        n++;
    }

    return n;
}
