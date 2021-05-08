/**
 * @file mpint_montgomery_red.c
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

int mpint_montgomery_red(
    mpint_t *r, const mpint_t *a, uint32_t k, const mpint_t *p, mpint_t *t)
{
    uint32_t value;
    mpint_t b;

    value = 1;
    b.sign = 1;
    b.size = 1;
    b.data = &value;

    return mpint_montgomery_mul(r, a, &b, k, p, t);
}

//=============================================================================
// End of file
//=============================================================================
