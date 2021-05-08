/**
 * @file mpint_shift_right.c
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

#include <string.h>

int mpint_shift_right(mpint_t *r, size_t n)
{
    int error;
    size_t i;
    size_t m;

    size_t shift_word_count = n / (MPINT_INT_SIZE * 8);
    size_t shift_bit_count = n % (MPINT_INT_SIZE * 8);

    if (shift_word_count >= r->size)
    {
        memset(r->data, 0, r->size * MPINT_INT_SIZE);
        return 0;
    }

    if (shift_word_count > 0)
    {
        for (m = r->size - shift_word_count, i = 0; i < m; ++i)
        {
            r->data[i] = r->data[i + shift_word_count];
        }

        for (i = m; i < r->size; ++i)
        {
            r->data[i] = 0;
        }
    }

    if (shift_bit_count > 0)
    {
        for (m = r->size - shift_word_count - 1, i = 0; i < m; ++i)
        {
            r->data[i] = (r->data[i] >> shift_bit_count) |
                         (r->data[i + 1] << (32 - shift_bit_count));
        }

        r->data[m] >>= shift_bit_count;
    }

    return 0;
}

//=============================================================================
// End of file
//=============================================================================
