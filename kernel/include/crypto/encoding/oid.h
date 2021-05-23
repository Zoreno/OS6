/**
 * @file oid.h
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

#ifndef _CRYPTO_ENCODING_OID_H
#define _CRYPTO_ENCODING_OID_H

#include <stddef.h>
#include <stdint.h>

#define OID_MORE_FLAG 0x80
#define OID_VALUE_MASK 0x7F

typedef struct
{
    uint8_t *data;
    size_t len;
} oid_t;

int oid_check(oid_t *oid);
int oid_comp(const oid_t *oid1, const oid_t *oid2);
int oid_match(const oid_t *oid, const oid_t *subtree, const oid_t *mask);
size_t oid_count_subidentifiers(const oid_t *oid);
int oid_encode_subidentifier(oid_t *oid, size_t *pos, uint32_t value);
int oid_decode_subidentifier(const oid_t *oid, size_t *pos, uint32_t *value);
char *oid_to_string(const oid_t *oid, char *str, size_t max_str_len);

#endif

//=============================================================================
// End of file
//=============================================================================
