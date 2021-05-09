/**
 * @file mpint_grow.c
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

int mpint_grow(mpint_t *i, size_t new_size)
{
    uint32_t *data;

    if (new_size < 1)
    {
        new_size = 1;
    }

    if (i->size >= new_size)
    {
        return 0;
    }

    data = (uint32_t *)crypto_malloc(new_size * MPINT_INT_SIZE);

    if (data == NULL)
    {
        // TODO: Better error codes.
        return -1;
    }

    crypto_zeroize(data, new_size * MPINT_INT_SIZE);

    if (i->size > 0)
    {
        memcpy(data, i->data, i->size * MPINT_INT_SIZE);

        crypto_zeroize(i->data, i->size * MPINT_INT_SIZE);
        crypto_free(i->data);
    }

    i->size = new_size;
    i->data = data;

    return 0;
}

//=============================================================================
// End of file
//=============================================================================
