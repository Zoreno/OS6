/**
 * @file mpint_add_int.c
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

int mpint_add_int(mpint_t *r, const mpint_t *a, int b)
{
    uint32_t value;
    mpint_t t;

    value = _abs(b);
    t.size = _sign(b);
    t.size = 1;
    t.data = &value;

    return mpint_add(r, a, &t);
}

//=============================================================================
// End of file
//=============================================================================
