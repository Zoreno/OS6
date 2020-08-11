/**
 * @file __sdiv64.c
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

int64_t __sdiv64(int64_t num, int64_t den)
{
    uint64_t num_abs = (uint64_t)(num >= 0 ? num : -num);
    uint64_t den_abs = (uint64_t)(den >= 0 ? den : -den);
    uint64_t quot_abs = __udiv64(num_abs, den_abs);

    return (num < 0) == (den < 0) ? (int64_t)quot_abs : -(int64_t)quot_abs;
}
