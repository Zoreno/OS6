/**
 * @file mpint_export.c
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

static int mpint_export_le(const mpint_t *a, uint8_t *data, size_t length)
{
    size_t n = mpint_get_byte_length(a);

    if (n > length)
    {
        return -1;
    }

    memset(data, 0, length);

    for (size_t i = 0; i < n; ++i, ++data)
    {
        *data = a->data[i / MPINT_INT_SIZE] >> ((i % MPINT_INT_SIZE) * 8);
    }

    return 0;
}

static int mpint_export_be(const mpint_t *a, uint8_t *data, size_t length)
{
    size_t n = mpint_get_byte_length(a);

    if (n > length)
    {
        return -1;
    }

    memset(data, 0, length);

    data += length - 1;

    for (size_t i = 0; i < n; ++i, --data)
    {
        *data = a->data[i / MPINT_INT_SIZE] >> ((i % MPINT_INT_SIZE) * 8);
    }

    return 0;
}

//=============================================================================
// Interface functions
//=============================================================================

int mpint_export(const mpint_t *a,
                 uint8_t *data,
                 size_t length,
                 mpint_format_t format)
{
    switch (format)
    {
    case MPINT_FORMAT_BIG_ENDIAN:
        return mpint_export_be(a, data, length);

    case MPINT_FORMAT_LITTLE_ENDIAN:
        return mpint_export_le(a, data, length);

    default:
        break;
    }

    // Unsupported format.
    return -1;
}

//=============================================================================
// End of file
//=============================================================================
