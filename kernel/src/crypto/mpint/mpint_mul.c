/**
 * @file mpint_mul.c
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

int mpint_mul(mpint_t *r, const mpint_t *a, const mpint_t *b)
{
    int error;

    mpint_t temp_a;
    mpint_t temp_b;

    size_t a_len;
    size_t b_len;

    mpint_init(&temp_a);
    mpint_init(&temp_b);

    if (r == a)
    {
        MPINT_GOTO_ON_ERROR(mpint_copy(&temp_a, a), end);

        a = &temp_a;
    }

    if (r == b)
    {
        MPINT_GOTO_ON_ERROR(mpint_copy(&temp_b, b), end);

        b = &temp_b;
    }

    a_len = mpint_get_length(a);
    b_len = mpint_get_length(b);

    MPINT_GOTO_ON_ERROR(mpint_grow(r, a_len + b_len), end);

    r->sign = (a->sign == b->sign) ? 1 : -1;

    memset(r->data, 0, r->size * MPINT_INT_SIZE);

    if (a_len < b_len)
    {
        for (size_t i = 0; i < a_len; ++i)
        {
            mpint_mul_acc_core(&r->data[i], b->data, b_len, a->data[i]);
        }
    }
    else
    {
        for (size_t i = 0; i < b_len; ++i)
        {
            mpint_mul_acc_core(&r->data[i], a->data, a_len, b->data[i]);
        }
    }

end:
    mpint_free(&temp_a);
    mpint_free(&temp_b);

    return error;
}

//=============================================================================
// End of file
//=============================================================================
