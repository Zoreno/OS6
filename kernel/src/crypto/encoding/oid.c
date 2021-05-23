/**
 * @file oid.c
 * @author Joakim Bertils
 * @version 0.1
 * @date 2021-05-20
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
#include <crypto/encoding/oid.h>

#include <stdio.h>
#include <string.h>

//=============================================================================
// Private functions
//=============================================================================

static void emit_buffer_to_string(char *buffer,
                                  size_t buffer_len,
                                  char **str,
                                  size_t *str_len)
{
    if (buffer_len <= *str_len)
    {
        strcpy(*str, buffer);
        *str += buffer_len;
        *str_len -= buffer_len;
    }
}

//=============================================================================
// Interface functions
//=============================================================================

int oid_check(oid_t *oid)
{
    if (oid->data == NULL)
    {
        return -1;
    }

    if (oid->len == 0)
    {
        return -1;
    }

    size_t i;
    size_t n;

    for (i = 1, n = 2; i < oid->len; ++i)
    {
        if ((oid->data[i] & OID_MORE_FLAG) == 0)
        {
            ++n;
        }

        if (n > 128)
        {
            return -1;
        }
    }

    if (oid->data[oid->len - 1] & OID_MORE_FLAG)
    {
        return -1;
    }

    return 0;
}

int oid_comp(const oid_t *oid1, const oid_t *oid2)
{
    for (size_t i = 0; i < oid1->len && i < oid2->len; ++i)
    {
        if (oid1->data[i] < oid2->data[i])
        {
            return -1;
        }

        if (oid1->data[i] > oid2->data[i])
        {
            return 1;
        }
    }

    if (oid1->len < oid2->len)
    {
        return -1;
    }

    if (oid1->len > oid2->len)
    {
        return 1;
    }

    return 0;
}

int oid_match(const oid_t *oid, const oid_t *subtree, const oid_t *mask);

size_t oid_count_subidentifiers(const oid_t *oid)
{
    if (oid->len == 0)
    {
        return 0;
    }

    size_t n = 2;

    for (size_t i = 1; i < oid->len; ++i)
    {
        if ((oid->data[i] & OID_MORE_FLAG) == 0)
        {
            ++n;
        }
    }

    return n;
}

int oid_encode_subidentifier(oid_t *oid, size_t *pos, uint32_t value)
{
    uint8_t tmp[5];

    tmp[0] = value & OID_VALUE_MASK;

    value >>= 7;

    size_t n = 1;

    while (value != 0)
    {
        tmp[n] = OID_MORE_FLAG | (value & OID_VALUE_MASK);
        value >>= 7;
        ++n;
    }

    if ((*pos + n) > oid->len)
    {
        return -1;
    }

    for (size_t i = 0; i < n; ++i)
    {
        oid->data[*pos + i] = tmp[n - i - 1];
    }

    *pos += n;

    return 0;
}

int oid_decode_subidentifier(const oid_t *oid, size_t *pos, uint32_t *value)
{
    *value = 0;

    if (*pos >= oid->len)
    {
        return -1;
    }

    for (size_t i = *pos; i < oid->len; ++i)
    {
        *value <<= 7;
        *value |= oid->data[i] & OID_VALUE_MASK;

        if ((oid->data[i] & OID_MORE_FLAG) == 0)
        {
            *pos = i + 1;

            return 0;
        }
    }

    return -1;
}

char *oid_to_string(const oid_t *oid, char *str, size_t max_str_len)
{
    static char buffer[128];
    char temp[12];

    if (str == NULL)
    {
        str = buffer;
        max_str_len = sizeof(buffer) - 1;
    }

    char *p = str;
    *p = '\0';

    if (oid->len <= 0)
    {
        return str;
    }

    size_t n = sprintf(temp, "%i.%i", oid->data[0] / 40, oid->data[0] % 40);

    emit_buffer_to_string(temp, n, &p, &max_str_len);

    uint32_t value;

    for (size_t i = 0; i < oid->len; ++i)
    {
        value <<= 7;
        value |= oid->data[i] & OID_VALUE_MASK;

        if ((oid->data[i] & OID_MORE_FLAG) == 0)
        {
            n = sprintf(temp, ".%i", value);

            emit_buffer_to_string(temp, n, &p, &max_str_len);
        }

        value = 0;
    }

    return str;
}

//=============================================================================
// End of file
//=============================================================================
