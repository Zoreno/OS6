/**
 * @file mpint_import.c
 * @author Joakim Bertils
 * @version 0.1
 * @date 2021-05-19
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

//=============================================================================
// Private functions
//=============================================================================

static int mpint_import_le(mpint_t *r, const uint8_t *data, size_t length)
{
    while (length > 0 && data[length - 1] == 0)
    {
        --length;
    }

    int error = mpint_grow(r, (length + MPINT_INT_SIZE - 1) / MPINT_INT_SIZE);

    if (error)
    {
        return error;
    }

    memset(r->data, 0, r->size * MPINT_INT_SIZE);
    r->sign = 1;

    for (size_t i = 0; i < length; ++i, ++data)
    {
        r->data[i / MPINT_INT_SIZE] |= *data << ((i % MPINT_INT_SIZE) * 8);
    }

    return 0;
}

static int mpint_import_be(mpint_t *r, const uint8_t *data, size_t length)
{
    while (length > 1 && *data == 0)
    {
        ++data;
        --length;
    }

    int error = mpint_grow(r, (length + MPINT_INT_SIZE - 1) / MPINT_INT_SIZE);

    if (error)
    {
        return error;
    }

    memset(r->data, 0, r->size * MPINT_INT_SIZE);
    r->sign = 1;

    data += length - 1;

    for (size_t i = 0; i < length; ++i, --data)
    {
        r->data[i / MPINT_INT_SIZE] |= *data << ((i % MPINT_INT_SIZE) * 8);
    }

    return 0;
}

//=============================================================================
// Interface functions
//=============================================================================

int mpint_import(mpint_t *r,
                 const uint8_t *data,
                 size_t length,
                 mpint_format_t format)
{
    switch (format)
    {
    case MPINT_FORMAT_BIG_ENDIAN:
        return mpint_import_be(r, data, length);

    case MPINT_FORMAT_LITTLE_ENDIAN:
        return mpint_import_le(r, data, length);

    default:
        break;
    }

    // Unsupported format.
    return -1;
}

//=============================================================================
// End of file
//=============================================================================
