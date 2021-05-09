/**
 * @file mpint_copy.c
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

int mpint_copy(mpint_t *dest, const mpint_t *src)
{
    int error;

    uint32_t src_len;

    if (dest == src)
    {
        return 0;
    }

    src_len = mpint_get_length(src);

    error = mpint_grow(dest, src_len);

    if (error)
    {
        return error;
    }

    memset(dest->data, 0, dest->size * MPINT_INT_SIZE);
    memcpy(dest->data, src->data, dest->size * MPINT_INT_SIZE);
    dest->sign = src->sign;

    return 0;
}

//=============================================================================
// End of file
//=============================================================================
