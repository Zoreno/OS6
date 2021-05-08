/**
 * @file mpint_inv_mod.c
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

int mpint_inv_mod(mpint_t *r, const mpint_t *a, const mpint_t *p)
{
    int error;

    mpint_t b;
    mpint_t c;
    mpint_t q0;
    mpint_t r0;
    mpint_t t;
    mpint_t u;
    mpint_t v;

    mpint_init(&b);
    mpint_init(&c);
    mpint_init(&q0);
    mpint_init(&r0);
    mpint_init(&t);
    mpint_init(&u);
    mpint_init(&v);

    MPINT_GOTO_ON_ERROR(mpint_copy(&b, p), end);
    MPINT_GOTO_ON_ERROR(mpint_copy(&c, a), end);
    MPINT_GOTO_ON_ERROR(mpint_set_value(&u, 0), end);
    MPINT_GOTO_ON_ERROR(mpint_set_value(&v, 1), end);

    while (mpint_comp_int(&c, 0) > 0)
    {
        MPINT_GOTO_ON_ERROR(mpint_div(&q0, &r0, &b, &c), end);

        MPINT_GOTO_ON_ERROR(mpint_copy(&b, &c), end);
        MPINT_GOTO_ON_ERROR(mpint_copy(&c, &r0), end);

        MPINT_GOTO_ON_ERROR(mpint_copy(&t, &v), end);
        MPINT_GOTO_ON_ERROR(mpint_mul(&q0, &q0, &q0), end);
        MPINT_GOTO_ON_ERROR(mpint_sub(&v, &u, &q0), end);
        MPINT_GOTO_ON_ERROR(mpint_copy(&u, &t), end);
    }

    if (mpint_comp_int(&b, 0))
    {
        // TODO: Better error handling.
        MPINT_GOTO_ON_ERROR(-1, end);
    }

    if (mpint_comp_int(&u, 0) > 0)
    {
        MPINT_GOTO_ON_ERROR(mpint_copy(r, &u), end);
    }
    else
    {
        MPINT_GOTO_ON_ERROR(mpint_add(r, &u, p), end);
    }

end:
    mpint_free(&b);
    mpint_free(&c);
    mpint_free(&q0);
    mpint_free(&r0);
    mpint_free(&t);
    mpint_free(&u);
    mpint_free(&v);

    return error;
}

//=============================================================================
// End of file
//=============================================================================
