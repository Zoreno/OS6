/**
 * @file random.c
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
#include <stdint.h>

static uint64_t _seed;

int rand(void)
{
    _seed = UINT64_C(6364136223846793005) * _seed + 1;

    return _seed >> 33;
}

int rand_range(int min, int max)
{
    int range = max - min;

    return (rand() % range) + min;
}

void srand(unsigned int seed)
{
    _seed = seed - 1;
}
