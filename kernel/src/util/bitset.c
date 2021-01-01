/**
 * @file bitset.c
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-04-19
 * 
 * @brief Implemententation of the bitset datastructure
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

#include <util/bitset.h>

#include <stdlib.h>
#include <string.h>

void bitset_init(bitset_t *set, size_t size)
{
    set->size = (size + 7) & ~7;
    set->data = malloc(set->size);

    memset(set->data, 0, set->size);
}

void bitset_free(bitset_t *set)
{
    free(set->data);
}

void bitset_set(bitset_t *set, size_t bit)
{
    set->data[bit / 8] |= (1 << (bit % 8));
}

void bitset_clear(bitset_t *set, size_t bit)
{
    set->data[bit / 8] &= ~(1 << (bit % 8));
}

int bitset_test(bitset_t *set, size_t bit)
{
    return set->data[bit / 8] & (1 << (bit % 8));
}

int bitset_ffub(bitset_t *set)
{
    for (size_t i = 0; i < set->size; ++i)
    {
        if (set->data[i] != 0xFF)
        {
            for (size_t j = 0; j < 8; ++j)
            {
                if (!(set->data[i] & (1 << (j))))
                {
                    set->data[i] |= (1 << (j));

                    return 8 * i + j;
                }
            }
        }
    }

    return -1;
}

//=============================================================================
// End of file
//=============================================================================