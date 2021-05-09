/**
 * @file mpint_sub_abs.c
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

//  R = | A - B |
int mpint_sub_abs(mpint_t *r, const mpint_t *a, const mpint_t *b)
{
    int error;

    if (mpint_comp_abs(a, b) < 0)
    {
        const mpint_t *t = a;
        a = b;
        b = t;
    }

    size_t a_len = mpint_get_length(a);
    size_t b_len = mpint_get_length(b);
    size_t i;
    uint32_t carry;
    uint32_t temp_data;

    MPINT_GOTO_ON_ERROR(mpint_grow(r, a_len), end);

    r->sign = 1;
    carry = 0;

    for (i = 0; i < b_len; ++i)
    {
        temp_data = a->data[i];

        if (carry != 0)
        {
            if (temp_data != 0)
            {
                carry = 0;
            }

            temp_data -= 1;
        }

        if (temp_data < b->data[i])
        {
            carry = 1;
        }

        r->data[i] = temp_data - b->data[i];
    }

    for (i = b_len; carry && i < a_len; ++i)
    {
        if (a->data[i] != 0)
        {
            carry = 0;
        }

        r->data[i] = a->data[i] - 1;
    }

    if (r != a)
    {
        for (; i < a_len; ++i)
        {
            r->data[i] = a->data[i];
        }

        for (; i < r->size; ++i)
        {
            r->data[i] = 0;
        }
    }

end:
    return error;
}

//=============================================================================
// End of file
//=============================================================================
