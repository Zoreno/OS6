/**
 * @file mpint_montgomery_mul.c
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

#define MIN(a, b) ((a > b) ? (b) : (a))

int mpint_montgomery_mul(mpint_t *r,
                         const mpint_t *a,
                         const mpint_t *b,
                         uint32_t k,
                         const mpint_t *p,
                         mpint_t *t)
{
    int error;

    uint32_t i;
    uint32_t m;
    uint32_t n;
    uint32_t q;

    for (m = 2 - p->data[0], i = 0; i < 4; ++i)
    {
        m = m * (2 - m * p->data[0]);
    }

    m = ~m + 1;

    n = MIN(b->size, k);

    MPINT_GOTO_ON_ERROR(mpint_grow(t, 2 * k + 1), end);
    MPINT_GOTO_ON_ERROR(mpint_set_value(t, 0), end);

    for (i = 0; i < k; ++i)
    {
        if (i < a->size)
        {
            q = (t->data[i] + a->data[i] * b->data[0]) * m;
            mpint_mul_acc_core(t->data + i, b->data, n, a->data[i]);
        }
        else
        {
            q = t->data[i] * m;
        }

        mpint_mul_acc_core(t->data + i, p->data, k, q);
    }

    MPINT_GOTO_ON_ERROR(mpint_shift_right(t, k * (MPINT_INT_SIZE * 8)), end);
    MPINT_GOTO_ON_ERROR(mpint_copy(r, t), end);

    if (mpint_comp(r, p) >= 0)
    {
        MPINT_GOTO_ON_ERROR(mpint_sub(r, r, p), end);
    }

end:
    return error;
}

//=============================================================================
// End of file
//=============================================================================
