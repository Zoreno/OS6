/**
 * @file mpint_div_int.c
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

static int _abs(int val)
{
    return val >= 0 ? val : -val;
}

static int _sign(int val)
{
    return val >= 0 ? 1 : -1;
}

int mpint_div_int(mpint_t *q, mpint_t *r, const mpint_t *a, int b)
{
    uint32_t value;
    mpint_t t;

    value = _abs(b);
    t.sign = _sign(b);
    t.size = 1;
    t.data = &value;

    return mpint_div(q, r, a, &t);
}

//=============================================================================
// End of file
//=============================================================================
