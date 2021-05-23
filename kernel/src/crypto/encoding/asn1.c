/**
 * @file asn1.c
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

#include <crypto/encoding/asn1.h>
#include <crypto/encoding/oid.h>

#include <stdio.h>
#include <string.h>

//=============================================================================
// Private data
//=============================================================================

static const char *tag_names[32] = {"[0]",
                                    "BOOLEAN",
                                    "INTEGER",
                                    "BIT STRING",
                                    "OCTET STRING",
                                    "NULL",
                                    "OBJECT IDENTIFIER",
                                    "OBJECT DESCRIPTOR",
                                    "EXTERNAL",
                                    "REAL",
                                    "ENUMERATED",
                                    "[11]",
                                    "UTF8 STRING",
                                    "[13]",
                                    "[14]",
                                    "[15]",
                                    "SEQUENCE",
                                    "SET",
                                    "NUMERIC STRING",
                                    "PRINTABLE STRING",
                                    "TELETEX STRING",
                                    "VIDEOTEX STRING",
                                    "IA5 STRING",
                                    "UTC TIME",
                                    "GENERALIZED TIME",
                                    "GRAPHIC STRING",
                                    "VISIBLE STRING",
                                    "GENERAL STRING",
                                    "UNIVERSAL STRING",
                                    "[29]",
                                    "BMP STRING",
                                    "[31]"};

//=============================================================================
// Private functions
//=============================================================================

static int asn1_read_obj_class(const uint8_t *data,
                               size_t length,
                               asn1_tag_t *tag,
                               size_t *i)
{
    (void)length;

    tag->obj_class = data[*i] & ASN1_CLASS_MASK;

    return 0;
}

static int asn1_read_constructed(const uint8_t *data,
                                 size_t length,
                                 asn1_tag_t *tag,
                                 size_t *i)
{
    (void)length;

    tag->constructed = (data[*i] & ASN1_ENCODING_CONSTRUCTED) ? 1 : 0;

    return 0;
}

static int asn1_read_obj_type(const uint8_t *data,
                              size_t length,
                              asn1_tag_t *tag,
                              size_t *i)
{
    // If the tag number is 30 or below, it is inlined in the first byte.
    // Otherwise it is encoded in the following bytes.
    if ((data[*i] & ASN1_TAG_NUMBER_MASK) <= 30)
    {
        tag->obj_type = data[*i] & ASN1_TAG_NUMBER_MASK;
        *i += 1;

        return 0;
    }

    tag->obj_type = 0;

    for (;; *i += 1)
    {
        if (*i > (sizeof(tag->obj_type) + 1))
        {
            return -1;
        }

        if ((length - *i) == 0)
        {
            return -1;
        }

        tag->obj_type = (tag->obj_type << 7) | (data[*i] & 0x7F);

        if ((data[*i] & 0x80) == 0)
        {
            break;
        }
    }

    *i += 1;

    return 0;
}

static int asn1_read_tag_length(const uint8_t *data,
                                size_t length,
                                asn1_tag_t *tag,
                                size_t *i)
{
    if (data[*i] == 0xFF)
    {
        // Indefinite length of the tag is not currently supported.
        return -1;
    }

    if (data[*i] < 128)
    {
        tag->length = data[*i];
        *i += 1;
        return 0;
    }

    size_t n = data[*i] & 0x7F;

    if (n > sizeof(tag->length))
    {
        return -1;
    }

    if ((length - *i) < n)
    {
        return -1;
    }

    tag->length = 0;

    for (*i += 1; n > 0; --n)
    {
        tag->length = (tag->length << 8) | data[*i];
        *i += 1;
    }

    return 0;
}

static int asn1_tag_is_universal(const asn1_tag_t *tag)
{
    return (tag->obj_class & ASN1_CLASS_MASK) == ASN1_CLASS_UNIVERSAL;
}

static void asn1_print_prefix(size_t depth)
{
    // TODO: Optimize this function
    while (depth--)
    {
        printf(" ");
    }
}

static void asn1_print_oid(size_t depth, asn1_tag_t *tag)
{
    asn1_print_prefix(depth + 2);
    oid_t oid;
    oid.data = (uint8_t *)tag->value;
    oid.len = tag->length;
    printf("%s\n", oid_to_string(&oid, NULL, 0));
}

static void asn1_print_string(size_t depth, asn1_tag_t *tag)
{
    asn1_print_prefix(depth + 2);
    for (size_t i = 0; i < tag->length; ++i)
    {
        printf("%c", (char)tag->value[i]);
    }
    printf("\n");
}

static void asn1_print_utc_time(size_t depth, asn1_tag_t *tag)
{
    if (tag->length != 13)
    {
        return;
    }

    if (tag->value[tag->length - 1] != 'Z')
    {
        return;
    }

    asn1_print_prefix(depth + 2);

    printf("%c%c/%c%c/%c%c %c%c:%c%c:%c%c\n",
           tag->value[0],   // Y
           tag->value[1],   // Y
           tag->value[2],   // M
           tag->value[3],   // M
           tag->value[4],   // D
           tag->value[5],   // D
           tag->value[6],   // h
           tag->value[7],   // h
           tag->value[8],   // m
           tag->value[9],   // m
           tag->value[10],  // s
           tag->value[11]   // s
    );
}

static void asn1_print_generalized_time(size_t depth, asn1_tag_t *tag)
{
    if (tag->length != 15)
    {
        return;
    }

    if (tag->value[tag->length - 1] != 'Z')
    {
        return;
    }

    asn1_print_prefix(depth + 2);

    printf("%c%c%c%c/%c%c/%c%c %c%c:%c%c:%c%c\n",
           tag->value[0],   // Y
           tag->value[1],   // Y
           tag->value[2],   // Y
           tag->value[3],   // Y
           tag->value[4],   // M
           tag->value[5],   // M
           tag->value[6],   // D
           tag->value[7],   // D
           tag->value[8],   // h
           tag->value[9],   // h
           tag->value[10],  // m
           tag->value[11],  // m
           tag->value[12],  // s
           tag->value[13]   // s
    );
}

static int asn1_dump_object_impl(const uint8_t *data,
                                 size_t length,
                                 uint32_t depth)
{
    while (length > 0)
    {
        asn1_tag_t tag;

        int error = asn1_read_tag(data, length, &tag);

        if (error)
        {
            return error;
        }

        data += tag.total_length;
        length -= tag.total_length;

        if (tag.obj_type < 32 && asn1_tag_is_universal(&tag))
        {
            asn1_print_prefix(depth);
            printf("%s (%i bytes)\n", tag_names[tag.obj_type], tag.length);
        }
        else
        {
            asn1_print_prefix(depth);
            printf("[%u] (%i bytes)\n", tag_names[tag.obj_type], tag.length);
        }

        if (tag.constructed)
        {
            error = asn1_dump_object_impl(tag.value, tag.length, depth + 2);

            if (error)
            {
                return error;
            }
        }
        else
        {
            switch (tag.obj_type)
            {
            case ASN1_TYPE_OBJECT_IDENTIFIER:
                asn1_print_oid(depth, &tag);
                break;
            case ASN1_TYPE_UTF8_STRING:
            case ASN1_TYPE_NUMERIC_STRING:
            case ASN1_TYPE_PRINTABLE_STRING:
            case ASN1_TYPE_TELETEX_STRING:
            case ASN1_TYPE_VIDEOTEX_STRING:
            case ASN1_TYPE_IA5_STRING:
            case ASN1_TYPE_GRAPHIC_STRING:
            case ASN1_TYPE_VISIBLE_STRING:
            case ASN1_TYPE_GENERAL_STRING:
            case ASN1_TYPE_UNIVERSAL_STRING:
            case ASN1_TYPE_BMP_STRING:
                asn1_print_string(depth, &tag);
                break;

            case ASN1_TYPE_UTC_TIME:
                asn1_print_utc_time(depth, &tag);
                break;
            case ASN1_TYPE_GENERALIZED_TIME:
                break;

            default:
            {
                asn1_print_prefix(depth + 2);
                printf("[Unsupported data format]\n");
            }
            break;
            }
        }
    }

    return 0;
}

//=============================================================================
// Interface functions
//=============================================================================

int asn1_read_tag(const uint8_t *data, size_t length, asn1_tag_t *tag)
{
    if (length == 0 || data == NULL || tag == NULL)
    {
        return -1;
    }

    size_t i = 0;

    int error = asn1_read_obj_class(data, length, tag, &i);

    if (error)
    {
        return error;
    }

    error = asn1_read_constructed(data, length, tag, &i);

    if (error)
    {
        return error;
    }

    error = asn1_read_obj_type(data, length, tag, &i);

    if (error)
    {
        return error;
    }

    if ((length - i) == 0)
    {
        return -1;
    }

    error = asn1_read_tag_length(data, length, tag, &i);

    if (error)
    {
        return error;
    }

    tag->value = data + i;

    if ((length - i) < tag->length)
    {
        return -1;
    }

    tag->total_length = i + tag->length;

    return 0;
}

int asn1_write_tag(asn1_tag_t *tag, int reverse, uint8_t *data, size_t *written)
{
    size_t obj_tag_size;
    size_t length_field_size;

    if (tag->obj_type < 31)
    {
        obj_tag_size = 0;
    }
    else if (tag->obj_type < 128)
    {
        obj_tag_size = 1;
    }
    else if (tag->obj_type < 16384)
    {
        obj_tag_size = 2;
    }
    else if (tag->obj_type < 2097152)
    {
        obj_tag_size = 3;
    }
    else if (tag->obj_type < 268435456)
    {
        obj_tag_size = 4;
    }
    else
    {
        obj_tag_size = 5;
    }

    if (tag->length < 128)
    {
        length_field_size = 0;
    }
    else if (tag->length < 256)
    {
        length_field_size = 1;
    }
    else if (tag->length < 65536)
    {
        length_field_size = 2;
    }
    else if (tag->length < 16777216)
    {
        length_field_size = 3;
    }
    else
    {
        length_field_size = 4;
    }

    if (data != NULL)
    {
        if (reverse)
        {
            if (tag->value != NULL && tag->length > 0)
            {
                data -= tag->length;

                memmove(data, tag->value, tag->length);
            }

            data -= obj_tag_size + length_field_size + 2;
        }
        else
        {
            if (tag->value != NULL && tag->length > 0)
            {
                memmove(data + obj_tag_size + length_field_size + 2,
                        tag->value,
                        tag->length);
            }
        }

        // Encode obj class
        data[0] = tag->obj_class;

        // Encode constructed
        if (tag->constructed)
        {
            data[0] |= ASN1_ENCODING_CONSTRUCTED;
        }

        // Encode tag number
        if (obj_tag_size == 0)
        {
            data[0] |= tag->obj_type;
        }
        else
        {
            data[0] |= ASN1_TAG_NUMBER_MASK;

            for (size_t i = 0; i < obj_tag_size; ++i)
            {
                data[obj_tag_size - 1] = (tag->obj_type << (i * 7)) & 0x7F;

                if (i != 0)
                {
                    data[obj_tag_size - i] |= 0x80;
                }
            }
        }

        // Encode length
        if (length_field_size == 0)
        {
            data[1 + obj_tag_size] = tag->length & 0x7F;
        }
        else
        {
            data[1 + obj_tag_size] = 0x80 | (length_field_size & 0x7F);

            for (size_t i = 0; i < length_field_size; ++i)
            {
                data[i + obj_tag_size + length_field_size + i] =
                    (tag->length >> (i * 8)) & 0xFF;
            }
        }
    }

    tag->total_length = tag->length + obj_tag_size + length_field_size + 2;

    if (written != NULL)
    {
        *written = obj_tag_size + length_field_size + 2;

        if (tag->value != NULL)
        {
            *written += tag->length;
        }
    }

    return 0;
}

int asn1_check_tag(const asn1_tag_t *tag,
                   int constructed,
                   uint32_t obj_class,
                   uint32_t obj_type)
{
    if (tag->constructed != constructed)
    {
        return -1;
    }

    if (tag->obj_class != obj_class)
    {
        return -1;
    }

    if (tag->obj_type != obj_type)
    {
        return -1;
    }

    return 0;
}

int asn1_read_booelan(const uint8_t *data,
                      size_t length,
                      asn1_tag_t *tag,
                      int *value)
{
    int error = asn1_read_tag(data, length, tag);

    if (error)
    {
        return error;
    }

    error = asn1_check_tag(tag, 0, ASN1_CLASS_UNIVERSAL, ASN1_TYPE_BOOLEAN);

    if (error)
    {
        return error;
    }

    if (tag->length != 1)
    {
        return -1;
    }

    if (tag->value[0])
    {
        *value = 1;
    }
    else
    {
        *value = 0;
    }

    return 0;
}

int asn1_read_int32(const uint8_t *data,
                    size_t length,
                    asn1_tag_t *tag,
                    int32_t *value)
{
    int error = asn1_read_tag(data, length, tag);

    if (error)
    {
        return error;
    }

    error = asn1_check_tag(tag, 0, ASN1_CLASS_UNIVERSAL, ASN1_TYPE_INTEGER);

    if (error)
    {
        return error;
    }

    if (tag->length < 1 || tag->length > 4)
    {
        return -1;
    }

    *value = (tag->value[0] & 0x80) ? -1 : 0;

    for (size_t i = 0; i < tag->length; ++i)
    {
        *value <<= 8;
        *value |= tag->value[i];
    }

    return 0;
}

int asn1_read_mpint(const uint8_t *data,
                    size_t length,
                    asn1_tag_t *tag,
                    mpint_t *value)
{
    int error = asn1_read_tag(data, length, tag);

    if (error)
    {
        return error;
    }

    error = asn1_check_tag(tag, 0, ASN1_CLASS_UNIVERSAL, ASN1_TYPE_INTEGER);

    if (error)
    {
        return error;
    }

    if (tag->length > 0 && (tag->value[0] & 0x80) != 0)
    {
        return 0;
    }

    error =
        mpint_import(value, tag->value, tag->length, MPINT_FORMAT_BIG_ENDIAN);

    return error;
}

int asn1_read_bit_string(const uint8_t *data, size_t length, asn1_tag_t *tag)
{
    int error = asn1_read_tag(data, length, tag);

    if (error)
    {
        return error;
    }

    error = asn1_check_tag(tag, 0, ASN1_CLASS_UNIVERSAL, ASN1_TYPE_BIT_STRING);

    return error;
}

int asn1_read_octet_string(const uint8_t *data, size_t length, asn1_tag_t *tag)
{
    int error = asn1_read_tag(data, length, tag);

    if (error)
    {
        return error;
    }

    error =
        asn1_check_tag(tag, 1, ASN1_CLASS_UNIVERSAL, ASN1_TYPE_OCTET_STRING);

    return error;
}

int asn1_read_oid(const uint8_t *data,
                  size_t length,
                  asn1_tag_t *tag,
                  oid_t *oid)
{
    int error = asn1_read_tag(data, length, tag);

    if (error)
    {
        return error;
    }

    error = asn1_check_tag(
        tag, 1, ASN1_CLASS_UNIVERSAL, ASN1_TYPE_OBJECT_IDENTIFIER);

    return error;
}

int asn1_read_sequence(const uint8_t *data, size_t length, asn1_tag_t *tag)
{
    int error = asn1_read_tag(data, length, tag);

    if (error)
    {
        return error;
    }

    error = asn1_check_tag(tag, 1, ASN1_CLASS_UNIVERSAL, ASN1_TYPE_SEQUENCE);

    return error;
}

int asn1_read_set(const uint8_t *data, size_t length, asn1_tag_t *tag)
{
    int error = asn1_read_tag(data, length, tag);

    if (error)
    {
        return error;
    }

    error = asn1_check_tag(tag, 1, ASN1_CLASS_UNIVERSAL, ASN1_TYPE_SET);

    return error;
}

int asn1_write_int32(int32_t value, int reverse, uint8_t *data, size_t *written)
{
    size_t n;

    for (n = 4; n > 1; --n)
    {
        uint16_t msb = (value >> (n * 8 - 9)) & 0x01FF;

        if (msb != 0x0000 && msb != 0x01FF)
        {
            break;
        }
    }

    if (data != NULL)
    {
        if (reverse)
        {
            data -= n + 2;
        }

        data[0] = ASN1_CLASS_UNIVERSAL | ASN1_TYPE_INTEGER;
        data[1] = n & 0xFF;

        for (size_t i = 0; i < n; ++i)
        {
            data[1 + n - i] = (value << (i * 8)) & 0xFF;
        }
    }

    if (written != NULL)
    {
        *written = n + 2;
    }

    return 0;
}

int asn1_write_mpint(const mpint_t *value,
                     int reverse,
                     uint8_t *data,
                     size_t *written)
{
    size_t n = mpint_get_bit_length(value);

    n = (n / 8) + 1;

    if (data != NULL)
    {
        if (reverse)
        {
            data -= n;
        }

        int error = mpint_export(value, data, n, MPINT_FORMAT_BIG_ENDIAN);

        if (error)
        {
            return error;
        }
    }

    asn1_tag_t tag;
    tag.constructed = 0;
    tag.obj_class = ASN1_CLASS_UNIVERSAL;
    tag.obj_type = ASN1_TYPE_INTEGER;
    tag.length = n;
    tag.value = data;

    int error = asn1_write_tag(&tag, 0, data, NULL);

    if (error)
    {
        return error;
    }

    if (written != NULL)
    {
        *written = tag.total_length;
    }

    return 0;
}

int asn1_check_oid(const asn1_tag_t *tag, const oid_t *oid)
{
    int error = asn1_check_tag(
        tag, 0, ASN1_CLASS_UNIVERSAL, ASN1_TYPE_OBJECT_IDENTIFIER);

    if (error)
    {
        return error;
    }

    oid_t tag_oid;

    tag_oid.data = (uint8_t *)tag->value;
    tag_oid.len = tag->length;

    if (oid_comp(&tag_oid, oid))
    {
        return -1;
    }

    return 0;
}

int asn1_dump_object(const uint8_t *data, size_t length)
{
    return asn1_dump_object_impl(data, length, 0);
}

//=============================================================================
// End of file
//=============================================================================
