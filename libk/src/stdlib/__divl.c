/**
 * @file __divl.c
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

uint32_t __divl(uint64_t num, uint64_t den)
{
    uint32_t num_high = num >> 32;
    uint32_t num_low = num;
    uint32_t quot;
    uint32_t rem;

    __asm__("divq %4"
            : "=d"(rem), "=a"(quot)
            : "0"(num_high), "1"(num_low), "rm"(den));

    return quot;
}
