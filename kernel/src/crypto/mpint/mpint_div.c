/**
 * @file mpint_div.c
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

int mpint_div(mpint_t *q, mpint_t *r, const mpint_t *a, const mpint_t *b)
{
    int error;

    mpint_t c;
    mpint_t d;
    mpint_t e;

    if (mpint_comp_int(b, 0) == 0)
    {
        // Divide by zero.
        return -1;
    }

    mpint_init(&c);
    mpint_init(&d);
    mpint_init(&e);

    MPINT_GOTO_ON_ERROR(mpint_copy(&c, a), end);
    MPINT_GOTO_ON_ERROR(mpint_copy(&d, b), end);
    MPINT_GOTO_ON_ERROR(mpint_set_value(&e, 0), end);

    size_t m = mpint_get_bit_length(&c);
    size_t n = mpint_get_bit_length(&d);

    if (m > n)
    {
        MPINT_GOTO_ON_ERROR(mpint_shift_left(&d, (m - n)), end);
    }

    while (n++ <= m)
    {
        MPINT_GOTO_ON_ERROR(mpint_shift_left(&e, 1), end);

        if (mpint_comp(&c, &d) >= 0)
        {
            MPINT_GOTO_ON_ERROR(mpint_set_bit_value(&e, 0, 1), end);
            MPINT_GOTO_ON_ERROR(mpint_sub(&c, &c, &d), end);
        }

        MPINT_GOTO_ON_ERROR(mpint_shift_right(&d, 1), end);
    }

    if (q != NULL)
    {
        MPINT_GOTO_ON_ERROR(mpint_copy(q, &e), end);
    }

    if (r != NULL)
    {
        MPINT_GOTO_ON_ERROR(mpint_copy(r, &c), end);
    }

end:
    mpint_free(&c);
    mpint_free(&d);
    mpint_free(&e);

    return error;
}

//=============================================================================
// End of file
//=============================================================================
