/**
 * @file mpint_rand.c
 * @author Joakim Bertils
 * @version 0.1
 * @date 2021-05-17
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

#include <crypto/crypto.h>
#include <crypto/mpint/mpint.h>

#include <string.h>

int mpint_rand(mpint_t *r,
               size_t length,
               const crypto_prng_algo_t *prng_algo,
               void *prng_context)
{
    int error;

    size_t m;
    size_t n;

    n = (length + (MPINT_INT_SIZE * 8) - 1) / (MPINT_INT_SIZE * 8);
    m = length % (MPINT_INT_SIZE * 8);

    error = mpint_grow(r, n);

    if (error)
    {
        return error;
    }

    memset(r->data, 0, r->size * MPINT_INT_SIZE);
    r->sign = 1;

    error = prng_algo->read_func(
        prng_context, (uint8_t *)r->data, n * MPINT_INT_SIZE);

    if (error)
    {
        return error;
    }

    if (n > 0 && m > 0)
    {
        r->data[n - 1] &= (1 << m) - 1;
    }

    return 0;
}

//=============================================================================
// End of file
//=============================================================================
