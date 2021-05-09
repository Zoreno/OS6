/**
 * @file mpint_mod.c
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

int mpint_mod(mpint_t *r, const mpint_t *a, const mpint_t *p)
{
    int error;
    int sign;
    mpint_t c;

    if (mpint_comp_int(p, 0) <= 0)
    {
        // TODO: Better error message.
        return -1;
    }

    mpint_init(&c);

    sign = a->sign;

    size_t m = mpint_get_bit_length(a);
    size_t n = mpint_get_bit_length(p);

    MPINT_GOTO_ON_ERROR(mpint_copy(r, a), end);

    if (m >= n)
    {
        MPINT_GOTO_ON_ERROR(mpint_copy(&c, p), end);
        MPINT_GOTO_ON_ERROR(mpint_shift_left(&c, m - n), end);

        while (mpint_comp_abs(r, p) >= 0)
        {
            if (mpint_comp_abs(r, &c) >= 0)
            {
                MPINT_GOTO_ON_ERROR(mpint_sub_abs(r, r, &c), end);
            }

            MPINT_GOTO_ON_ERROR(mpint_shift_right(&c, 1), end);
        }
    }

    if (sign < 0)
    {
        MPINT_GOTO_ON_ERROR(mpint_sub_abs(r, p, r), end);
    }

end:
    mpint_free(&c);
    return error;
}

//=============================================================================
// End of file
//=============================================================================
