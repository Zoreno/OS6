/**
 * @file mpint_set_bit_value.c
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

int mpint_set_bit_value(mpint_t *r, size_t index, uint32_t value)
{
    int error;

    size_t word_index;
    size_t bit_index;

    word_index = index / (MPINT_INT_SIZE * 8);
    bit_index = index % (MPINT_INT_SIZE * 8);

    error = mpint_grow(r, word_index + 1);

    if (error)
    {
        return error;
    }

    if (value)
    {
        r->data[word_index] |= (1 << bit_index);
    }
    else
    {
        r->data[word_index] &= ~(1 << bit_index);
    }

    // TODO: Better error codes.
    return 0;
}

//=============================================================================
// End of file
//=============================================================================
