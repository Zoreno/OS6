/**
 * @file mpint_exp_mod.c
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

#define MAX(a, b) (((a) > (b)) ? (a) : (b))

int mpint_exp_mod(mpint_t *r,
                  const mpint_t *a,
                  const mpint_t *e,
                  const mpint_t *p)
{
    int error;

    int i;
    int j;
    int n;

    uint32_t d;
    uint32_t k;
    uint32_t u;

    mpint_t b;
    mpint_t c2;
    mpint_t t;
    mpint_t s[8];

    mpint_init(&b);
    mpint_init(&c2);
    mpint_init(&t);

    for (size_t i = 0; i < 8; ++i)
    {
        mpint_init(&s[i]);
    }

    d = (mpint_get_bit_length(e) <= 32) ? 1 : 4;

    if (!mpint_get_bit_value(p, 0))  // Even modulus
    {
        MPINT_GOTO_ON_ERROR(mpint_mul_mod(&b, a, a, p), end);  // B = A^2 mod P
        MPINT_GOTO_ON_ERROR(mpint_copy(&s[0], a), end);        // S[0] = A

        for (i = 1; i < (1 << (d - 1)); ++i)
        {
            // S[i] = A^(2 * i + 1) mod P
            MPINT_GOTO_ON_ERROR(mpint_mul_mod(&s[i], &s[i - 1], &b, p), end);
        }

        MPINT_GOTO_ON_ERROR(mpint_set_value(r, 1), end);
        i = mpint_get_bit_length(e) - 1;

        while (i >= 0)
        {
            if (!mpint_get_bit_value(e, i))
            {
                // R = R^2 mod P
                MPINT_GOTO_ON_ERROR(mpint_mul_mod(r, r, r, p), end);
                --i;
            }
            else
            {
                n = MAX(i - d + 1, 0);

                while (!mpint_get_bit_value(e, n))
                {
                    ++n;
                }

                for (u = 0, j = i; j >= n; --j)
                {
                    // R = R^2 mod P
                    MPINT_GOTO_ON_ERROR(mpint_mul_mod(r, r, r, p), end);
                    u = (u << 1) | mpint_get_bit_value(e, j);
                }

                MPINT_GOTO_ON_ERROR(mpint_mul_mod(r, r, &s[u >> 1], p), end);

                i = n - 1;
            }
        }
    }
    else  // Odd modulus
    {
        k = mpint_get_length(p);

        MPINT_GOTO_ON_ERROR(mpint_set_value(&c2, 1), end);
        MPINT_GOTO_ON_ERROR(mpint_shift_left(&c2, 2 * k * (MPINT_INT_SIZE * 8)),
                            end);
        MPINT_GOTO_ON_ERROR(mpint_mod(&c2, &c2, p), end);

        // B = A * C mod P
        if (mpint_comp(a, p) >= 0)
        {
            MPINT_GOTO_ON_ERROR(mpint_mod(&b, a, p), end);
            MPINT_GOTO_ON_ERROR(mpint_montgomery_mul(&b, &b, &c2, k, p, &t),
                                end);
        }
        else
        {
            MPINT_GOTO_ON_ERROR(mpint_montgomery_mul(&b, a, &c2, k, p, &t),
                                end);
        }

        // R = B^2 * C^-1 mod P
        MPINT_GOTO_ON_ERROR(mpint_montgomery_mul(r, &b, &b, k, p, &t), end);
        MPINT_GOTO_ON_ERROR(mpint_copy(&s[0], &b), end);

        // S[i] = B^(2 * i + 1) * C^-1 mod P
        for (i = 1; i < (1 << (d - 1)); ++i)
        {
            MPINT_GOTO_ON_ERROR(
                mpint_montgomery_mul(&s[i], &s[i - 1], r, k, p, &t), end);
        }

        MPINT_GOTO_ON_ERROR(mpint_copy(r, &c2), end);
        MPINT_GOTO_ON_ERROR(mpint_montgomery_red(r, r, k, p, &t), end);

        i = mpint_get_bit_length(e) - 1;

        while (i >= 0)
        {
            if (!mpint_get_bit_value(e, i))
            {
                MPINT_GOTO_ON_ERROR(mpint_montgomery_mul(r, r, r, k, p, &t),
                                    end);
                --i;
            }
            else
            {
                n = MAX(i - d + 1, 0);

                while (!mpint_get_bit_value(e, n))
                {
                    ++n;
                }

                for (u = 0, j = i; j >= n; --j)
                {
                    MPINT_GOTO_ON_ERROR(mpint_montgomery_mul(r, r, r, k, p, &t),
                                        end);
                    u = (u << 1) | mpint_get_bit_value(e, j);
                }

                MPINT_GOTO_ON_ERROR(
                    mpint_montgomery_mul(r, r, &s[u >> 1], k, p, &t), end);

                i = n - 1;
            }
        }

        MPINT_GOTO_ON_ERROR(mpint_montgomery_red(r, r, k, p, &t), end);
    }

end:

    mpint_free(&b);
    mpint_free(&c2);
    mpint_free(&t);

    for (size_t i = 0; i < 8; ++i)
    {
        mpint_free(&s[i]);
    }

    return error;
}

//=============================================================================
// End of file
//=============================================================================
