/**
 * @file __udiv64.c
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

uint64_t __udiv64(uint64_t num, uint64_t den)
{
    if ((den >> 32) == 0)
    {
        uint64_t b = 1ULL << 32;
        uint32_t num_high = num >> 32;
        uint32_t num_low = num;

        return __divl(b * (num_high % den) + num_low, den) + b * num_high / den;
    }

    if (num < den)
    {
        return 0;
    }

    uint32_t den_high = den >> 32;

    int s = __nlz(den_high);

    uint64_t quot = __divl(num >> 1, (den << s) >> 32) >> (31 - s);

    return num - (quot - 1) * den < den ? quot - 1 : quot;
}
