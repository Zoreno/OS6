/**
 * @file mpint_add_abs.c
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

int mpint_add_abs(mpint_t *r, const mpint_t *a, const mpint_t *b)
{
    int error;
    size_t b_len;
    uint32_t carry;

    if (r == b)
    {
        const mpint_t *t = a;
        a = b;
        b = t;
    }
    else if (r != a)
    {
        MPINT_GOTO_ON_ERROR(mpint_copy(r, a), end);
    }

    b_len = mpint_get_length(b);

    MPINT_GOTO_ON_ERROR(mpint_grow(r, b_len), end);

    r->sign = 1;
    carry = 0;

    for (size_t i = 0; i < b_len; ++i)
    {
        uint32_t temp_data = r->data[i] + carry;

        if (temp_data != 0)
        {
            carry = 0;
        }

        temp_data += b->data[i];

        if (temp_data < b->data[i])
        {
            carry = 1;
        }

        r->data[i] = temp_data;
    }

    for (size_t i = b_len; carry && i < r->size; ++i)
    {
        r->data[i] += carry;

        if (r->data[i] != 0)
        {
            carry = 0;
        }
    }

    if (carry && b_len >= r->size)
    {
        MPINT_GOTO_ON_ERROR(mpint_grow(r, b_len + 1), end);

        r->data[b_len] = 1;
    }

end:
    return error;
}

//=============================================================================
// End of file
//=============================================================================
