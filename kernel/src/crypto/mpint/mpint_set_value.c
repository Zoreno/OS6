/**
 * @file mpint_set_value.c
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

#include <string.h>

int mpint_set_value(mpint_t *r, int a)
{
    int error;

    MPINT_GOTO_ON_ERROR(mpint_grow(r, 1), end);

    memset(r->data, 0, r->size * MPINT_INT_SIZE);

    r->data[0] = (a >= 0) ? a : -a;
    r->sign = (a >= 0) ? 1 : -1;

end:
    return error;
}

//=============================================================================
// End of file
//=============================================================================
