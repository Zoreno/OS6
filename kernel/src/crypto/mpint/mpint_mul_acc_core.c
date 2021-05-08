/**
 * @file mpint_mul_acc_core.c
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

void mpint_mul_acc_core(uint32_t *r,
                        const uint32_t *a,
                        size_t m,
                        const uint32_t b)
{
    size_t i;
    uint32_t carry;
    uint32_t product_lower;
    uint32_t product_upper;
    uint64_t product;

    carry = 0;
    product_lower = 0;
    product_upper = 0;

    for (i = 0; i < m; ++i)
    {
        product = (uint64_t)a[i] * b;
        product_lower = (uint32_t)product;
        product_upper = (uint32_t)(product >> 32);

        product_lower += carry;
        if (product_lower < carry)
        {
            ++product_upper;
        }

        product_lower += r[i];
        if (product_lower < r[i])
        {
            ++product_upper;
        }

        r[i] = product_lower;
        carry = product_upper;
    }

    for (; carry != 0; ++i)
    {
        r[i] += carry;
        carry = (r[i] < carry);
    }
}

//=============================================================================
// End of file
//=============================================================================
