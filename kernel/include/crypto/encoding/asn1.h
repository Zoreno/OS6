/**
 * @file asn1.h
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

#ifndef _CRYPTO_ENCODING_ASN1_H
#define _CRYPTO_ENCODING_ASN1_H

#include <crypto/crypto.h>
#include <crypto/encoding/oid.h>
#include <crypto/mpint/mpint.h>

#include <stddef.h>
#include <stdint.h>

#define ASN1_TAG_NUMBER_MASK 0x1F
#define ASN1_ENCODING_MASK 0x20
#define ASN1_ENCODING_PRIMITIVE 0x00
#define ASN1_ENCODING_CONSTRUCTED 0x20
#define ASN1_CLASS_MASK 0xC0
#define ASN1_CLASS_UNIVERSAL 0x00
#define ASN1_CLASS_APPLICATION 0x40
#define ASN1_CLASS_CONTEXT_SPECIFIC 0x80
#define ASN1_CLASS_PRIVATE 0xC0

typedef enum
{
    ASN1_TYPE_BOOLEAN = 1,
    ASN1_TYPE_INTEGER = 2,
    ASN1_TYPE_BIT_STRING = 3,
    ASN1_TYPE_OCTET_STRING = 4,
    ASN1_TYPE_NULL = 5,
    ASN1_TYPE_OBJECT_IDENTIFIER = 6,
    ASN1_TYPE_OBJECT_DESCRIPTOR = 7,
    ASN1_TYPE_EXTERNAL = 8,
    ASN1_TYPE_REAL = 9,
    ASN1_TYPE_ENUMERATED = 10,
    ASN1_TYPE_UTF8_STRING = 12,
    ASN1_TYPE_SEQUENCE = 16,
    ASN1_TYPE_SET = 17,
    ASN1_TYPE_NUMERIC_STRING = 18,
    ASN1_TYPE_PRINTABLE_STRING = 19,
    ASN1_TYPE_TELETEX_STRING = 20,
    ASN1_TYPE_VIDEOTEX_STRING = 21,
    ASN1_TYPE_IA5_STRING = 22,
    ASN1_TYPE_UTC_TIME = 23,
    ASN1_TYPE_GENERALIZED_TIME = 24,
    ASN1_TYPE_GRAPHIC_STRING = 25,
    ASN1_TYPE_VISIBLE_STRING = 26,
    ASN1_TYPE_GENERAL_STRING = 27,
    ASN1_TYPE_UNIVERSAL_STRING = 28,
    ASN1_TYPE_BMP_STRING = 30,
} asn1_type_t;

typedef struct
{
    int constructed;
    uint32_t obj_class;
    uint32_t obj_type;
    size_t length;

    const uint8_t *value;
    size_t total_length;
} asn1_tag_t;

// Generic tag interface

int asn1_read_tag(const uint8_t *data, size_t length, asn1_tag_t *tag);
int asn1_write_tag(asn1_tag_t *tag,
                   int reverse,
                   uint8_t *data,
                   size_t *written);
int asn1_check_tag(const asn1_tag_t *tag,
                   int constructed,
                   uint32_t obj_class,
                   uint32_t obj_type);

// Read types

int asn1_read_booelan(const uint8_t *data,
                      size_t length,
                      asn1_tag_t *tag,
                      int *value);
int asn1_read_int32(const uint8_t *data,
                    size_t length,
                    asn1_tag_t *tag,
                    int32_t *value);
int asn1_read_mpint(const uint8_t *data,
                    size_t length,
                    asn1_tag_t *tag,
                    mpint_t *value);
int asn1_read_bit_string(const uint8_t *data, size_t length, asn1_tag_t *tag);
int asn1_read_octet_string(const uint8_t *data, size_t length, asn1_tag_t *tag);
int asn1_read_oid(const uint8_t *data,
                  size_t length,
                  asn1_tag_t *tag,
                  oid_t *oid);
int asn1_read_sequence(const uint8_t *data, size_t length, asn1_tag_t *tag);
int asn1_read_set(const uint8_t *data, size_t length, asn1_tag_t *tag);

// Write types

int asn1_write_int32(int32_t value,
                     int reverse,
                     uint8_t *data,
                     size_t *written);
int asn1_write_mpint(const mpint_t *value,
                     int reverse,
                     uint8_t *data,
                     size_t *written);

// Utils

int asn1_check_oid(const asn1_tag_t *tag, const oid_t *oid);
int asn1_dump_object(const uint8_t *data, size_t length);

#endif

//=============================================================================
// End of file
//=============================================================================
