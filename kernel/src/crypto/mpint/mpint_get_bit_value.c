/**
 * @file mpint_get_bit_value.c
 * @author Joakim Bertils
 * @version 0.1
 * @date 2021-05-08
 *
 * @brief
 *
 * @copyright Copyright (C) 2021,
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

#include <crypto/mpint/mpint.h>

uint32_t mpint_get_bit_value(const mpint_t *a, size_t index)
{
    size_t word_index;
    size_t bit_index;

    word_index = index / (MPINT_INT_SIZE * 8);
    bit_index = index % (MPINT_INT_SIZE * 8);

    if (word_index >= a->size)
    {
        return 0;
    }

    return (a->data[word_index] >> bit_index) & 0x01;
}

//=============================================================================
// End of file
//=============================================================================
