/**
 * @file mpint_shift_left.c
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

int mpint_shift_left(mpint_t *r, size_t n)
{
    int error;

    size_t shift_word_count = n / (MPINT_INT_SIZE * 8);
    size_t shift_bit_count = n % (MPINT_INT_SIZE * 8);

    if (r->size == 0 || n == 0)
    {
        return 0;
    }

    MPINT_GOTO_ON_ERROR(mpint_grow(r, r->size + (n + 31) / 32), end);

    if (shift_word_count > 0)
    {
        for (size_t i = r->size - 1; i >= shift_word_count; --i)
        {
            r->data[i] = r->data[i - shift_word_count];
        }

        for (size_t i = 0; i < shift_word_count; ++i)
        {
            r->data[i] = 0;
        }
    }

    if (shift_bit_count > 0)
    {
        for (size_t i = r->size - 1; i >= 1; --i)
        {
            r->data[i] = (r->data[i] << shift_bit_count) |
                         (r->data[i - 1] >> (32 - shift_bit_count));
        }

        r->data[0] <<= shift_bit_count;
    }

end:
    return error;
}

//=============================================================================
// End of file
//=============================================================================
