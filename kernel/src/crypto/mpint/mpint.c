/**
 * @file mpint.c
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

#include <crypto/crypto.h>
#include <crypto/mpint/mpint.h>

#include <string.h>

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) > (b)) ? (b) : (a))

//=============================================================================
// Private function forward declarations
//=============================================================================

static int mpint_export_le(const mpint_t *a, uint8_t *data, size_t length);
static int mpint_export_be(const mpint_t *a, uint8_t *data, size_t length);
static int mpint_import_le(mpint_t *r, const uint8_t *data, size_t length);
static int mpint_import_be(mpint_t *r, const uint8_t *data, size_t length);

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

int mpint_add_abs(mpint_t *r, const mpint_t *a, const mpint_t *b)
{
    int error;
    size_t b_len;
    uint32_t carry;

    if (r == b)
    {
        const mpint_t *t = a;
        a = b;
        b = t;
    }
    else if (r != a)
    {
        MPINT_GOTO_ON_ERROR(mpint_copy(r, a), end);
    }

    b_len = mpint_get_length(b);

    MPINT_GOTO_ON_ERROR(mpint_grow(r, b_len), end);

    r->sign = 1;
    carry = 0;

    for (size_t i = 0; i < b_len; ++i)
    {
        uint32_t temp_data = r->data[i] + carry;

        if (temp_data != 0)
        {
            carry = 0;
        }

        temp_data += b->data[i];

        if (temp_data < b->data[i])
        {
            carry = 1;
        }

        r->data[i] = temp_data;
    }

    for (size_t i = b_len; carry && i < r->size; ++i)
    {
        r->data[i] += carry;

        if (r->data[i] != 0)
        {
            carry = 0;
        }
    }

    if (carry && b_len >= r->size)
    {
        MPINT_GOTO_ON_ERROR(mpint_grow(r, b_len + 1), end);

        r->data[b_len] = 1;
    }

end:
    return error;
}

int mpint_add_int(mpint_t *r, const mpint_t *a, int b)
{
    uint32_t value;
    mpint_t t;

    value = _abs(b);
    t.sign = _sign(b);
    t.size = 1;
    t.data = &value;

    return mpint_add(r, a, &t);
}

int mpint_add_mod(mpint_t *r,
                  const mpint_t *a,
                  const mpint_t *b,
                  const mpint_t *p)
{
    int error;

    MPINT_GOTO_ON_ERROR(mpint_add(r, a, b), end);
    MPINT_GOTO_ON_ERROR(mpint_mod(r, r, p), end);

end:
    return error;
}

int mpint_add(mpint_t *r, const mpint_t *a, const mpint_t *b)
{
    int error;
    int sign;

    sign = a->sign;

    if (a->sign == b->sign)
    {
        error = mpint_add_abs(r, a, b);
        r->sign = sign;

        return error;
    }

    if (mpint_comp_abs(a, b) >= 0)
    {
        error = mpint_sub_abs(r, a, b);
        r->sign = sign;
        return error;
    }

    error = mpint_sub_abs(r, b, a);
    r->sign = -sign;

    return error;
}

int mpint_comp_abs(const mpint_t *a, const mpint_t *b)
{
    size_t m;
    size_t n;

    m = mpint_get_length(a);
    n = mpint_get_length(b);

    if (m == 0 && n == 0)
    {
        return 0;
    }
    else if (m > n)
    {
        return 1;
    }
    else if (m < n)
    {
        return -1;
    }

    while (n--)
    {
        if (a->data[n] > b->data[n])
        {
            return 1;
        }
        else if (a->data[n] < b->data[n])
        {
            return -1;
        }
    }

    return 0;
}

int mpint_comp_int(const mpint_t *a, int b)
{
    uint32_t value;
    mpint_t t;

    value = _abs(b);
    t.sign = _sign(b);
    t.size = 1;
    t.data = &value;

    return mpint_comp(a, &t);
}

int mpint_comp(const mpint_t *a, const mpint_t *b)
{
    uint32_t a_len;
    uint32_t b_len;

    a_len = mpint_get_length(a);
    b_len = mpint_get_length(b);

    if (a_len == 0 && b_len == 0)
    {
        return 0;
    }
    else if (a_len > b_len)
    {
        return a->sign;
    }
    else if (a_len < b_len)
    {
        return -b->sign;
    }

    if (a->sign > 0 && b->sign < 0)
    {
        return 1;
    }
    else if (a->sign < 0 && b->sign > 0)
    {
        return -1;
    }

    while (b_len--)
    {
        if (a->data[b_len] > b->data[b_len])
        {
            return a->sign;
        }
        else if (a->data[b_len] < b->data[b_len])
        {
            return -a->sign;
        }
    }

    return 0;
}

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

int mpint_div_int(mpint_t *q, mpint_t *r, const mpint_t *a, int b)
{
    uint32_t value;
    mpint_t t;

    value = _abs(b);
    t.sign = _sign(b);
    t.size = 1;
    t.data = &value;

    return mpint_div(q, r, a, &t);
}

int mpint_div(mpint_t *q, mpint_t *r, const mpint_t *a, const mpint_t *b)
{
    int error;

    mpint_t c;
    mpint_t d;
    mpint_t e;

    size_t m;
    size_t n;

    if (mpint_comp_int(b, 0) == 0)
    {
        // Divide by zero.
        return -1;
    }

    mpint_init(&c);
    mpint_init(&d);
    mpint_init(&e);

    MPINT_GOTO_ON_ERROR(mpint_copy(&c, a), end);
    MPINT_GOTO_ON_ERROR(mpint_copy(&d, b), end);
    MPINT_GOTO_ON_ERROR(mpint_set_value(&e, 0), end);

    m = mpint_get_bit_length(&c);
    n = mpint_get_bit_length(&d);

    if (m > n)
    {
        MPINT_GOTO_ON_ERROR(mpint_shift_left(&d, (m - n)), end);
    }

    while (n++ <= m)
    {
        MPINT_GOTO_ON_ERROR(mpint_shift_left(&e, 1), end);

        if (mpint_comp(&c, &d) >= 0)
        {
            MPINT_GOTO_ON_ERROR(mpint_set_bit_value(&e, 0, 1), end);
            MPINT_GOTO_ON_ERROR(mpint_sub(&c, &c, &d), end);
        }

        MPINT_GOTO_ON_ERROR(mpint_shift_right(&d, 1), end);
    }

    if (q != NULL)
    {
        MPINT_GOTO_ON_ERROR(mpint_copy(q, &e), end);
    }

    if (r != NULL)
    {
        MPINT_GOTO_ON_ERROR(mpint_copy(r, &c), end);
    }

end:
    mpint_free(&c);
    mpint_free(&d);
    mpint_free(&e);

    return error;
}

int mpint_exp_mod(mpint_t *r,
                  const mpint_t *a,
                  const mpint_t *e,
                  const mpint_t *p)
{
    int error;

    int i;
    int j;
    int n;

    uint32_t d;
    uint32_t k;
    uint32_t u;

    mpint_t b;
    mpint_t c2;
    mpint_t t;
    mpint_t s[8];

    mpint_init(&b);
    mpint_init(&c2);
    mpint_init(&t);

    for (size_t i = 0; i < 8; ++i)
    {
        mpint_init(&s[i]);
    }

    d = (mpint_get_bit_length(e) <= 32) ? 1 : 4;

    if (!mpint_get_bit_value(p, 0))  // Even modulus
    {
        MPINT_GOTO_ON_ERROR(mpint_mul_mod(&b, a, a, p), end);  // B = A^2 mod P
        MPINT_GOTO_ON_ERROR(mpint_copy(&s[0], a), end);        // S[0] = A

        for (i = 1; i < (1 << (d - 1)); ++i)
        {
            // S[i] = A^(2 * i + 1) mod P
            MPINT_GOTO_ON_ERROR(mpint_mul_mod(&s[i], &s[i - 1], &b, p), end);
        }

        MPINT_GOTO_ON_ERROR(mpint_set_value(r, 1), end);
        i = mpint_get_bit_length(e) - 1;

        while (i >= 0)
        {
            if (!mpint_get_bit_value(e, i))
            {
                // R = R^2 mod P
                MPINT_GOTO_ON_ERROR(mpint_mul_mod(r, r, r, p), end);
                --i;
            }
            else
            {
                n = MAX(i - d + 1, 0);

                while (!mpint_get_bit_value(e, n))
                {
                    ++n;
                }

                for (u = 0, j = i; j >= n; --j)
                {
                    // R = R^2 mod P
                    MPINT_GOTO_ON_ERROR(mpint_mul_mod(r, r, r, p), end);
                    u = (u << 1) | mpint_get_bit_value(e, j);
                }

                MPINT_GOTO_ON_ERROR(mpint_mul_mod(r, r, &s[u >> 1], p), end);

                i = n - 1;
            }
        }
    }
    else  // Odd modulus
    {
        k = mpint_get_length(p);

        MPINT_GOTO_ON_ERROR(mpint_set_value(&c2, 1), end);
        MPINT_GOTO_ON_ERROR(mpint_shift_left(&c2, 2 * k * (MPINT_INT_SIZE * 8)),
                            end);
        MPINT_GOTO_ON_ERROR(mpint_mod(&c2, &c2, p), end);

        // B = A * C mod P
        if (mpint_comp(a, p) >= 0)
        {
            MPINT_GOTO_ON_ERROR(mpint_mod(&b, a, p), end);
            MPINT_GOTO_ON_ERROR(mpint_montgomery_mul(&b, &b, &c2, k, p, &t),
                                end);
        }
        else
        {
            MPINT_GOTO_ON_ERROR(mpint_montgomery_mul(&b, a, &c2, k, p, &t),
                                end);
        }

        // R = B^2 * C^-1 mod P
        MPINT_GOTO_ON_ERROR(mpint_montgomery_mul(r, &b, &b, k, p, &t), end);
        MPINT_GOTO_ON_ERROR(mpint_copy(&s[0], &b), end);

        // S[i] = B^(2 * i + 1) * C^-1 mod P
        for (i = 1; i < (1 << (d - 1)); ++i)
        {
            MPINT_GOTO_ON_ERROR(
                mpint_montgomery_mul(&s[i], &s[i - 1], r, k, p, &t), end);
        }

        MPINT_GOTO_ON_ERROR(mpint_copy(r, &c2), end);
        MPINT_GOTO_ON_ERROR(mpint_montgomery_red(r, r, k, p, &t), end);

        i = mpint_get_bit_length(e) - 1;

        while (i >= 0)
        {
            if (!mpint_get_bit_value(e, i))
            {
                MPINT_GOTO_ON_ERROR(mpint_montgomery_mul(r, r, r, k, p, &t),
                                    end);
                --i;
            }
            else
            {
                n = MAX(i - d + 1, 0);

                while (!mpint_get_bit_value(e, n))
                {
                    ++n;
                }

                for (u = 0, j = i; j >= n; --j)
                {
                    MPINT_GOTO_ON_ERROR(mpint_montgomery_mul(r, r, r, k, p, &t),
                                        end);
                    u = (u << 1) | mpint_get_bit_value(e, j);
                }

                MPINT_GOTO_ON_ERROR(
                    mpint_montgomery_mul(r, r, &s[u >> 1], k, p, &t), end);

                i = n - 1;
            }
        }

        MPINT_GOTO_ON_ERROR(mpint_montgomery_red(r, r, k, p, &t), end);
    }

end:

    mpint_free(&b);
    mpint_free(&c2);
    mpint_free(&t);

    for (size_t i = 0; i < 8; ++i)
    {
        mpint_free(&s[i]);
    }

    return error;
}

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

void mpint_free(mpint_t *i)
{
    if (i->data != NULL)
    {
        crypto_zeroize(i->data, i->size * MPINT_INT_SIZE);
        crypto_free(i->data);
    }

    i->sign = 1;
    i->size = 0;
    i->data = NULL;
}

size_t mpint_get_bit_length(const mpint_t *a)
{
    size_t word_count;

    if (a->size == 0)
    {
        return 0;
    }

    for (word_count = a->size - 1; word_count > 0; --word_count)
    {
        if (a->data[word_count] != 0)
        {
            break;
        }
    }

    uint32_t top_word = a->data[word_count];

    size_t bits = word_count * MPINT_INT_SIZE * 8;

    for (; top_word != 0; top_word >>= 1)
    {
        ++bits;
    }

    return bits;
}

uint32_t mpint_get_bit_value(const mpint_t *a, size_t index)
{
    size_t word_index;
    size_t bit_index;

    word_index = index / (MPINT_INT_SIZE * 8);
    bit_index = index % (MPINT_INT_SIZE * 8);

    if (word_index >= a->size)
    {
        return 0;
    }

    return (a->data[word_index] >> bit_index) & 0x01;
}

size_t mpint_get_byte_length(const mpint_t *a)
{
    size_t word_count;

    if (a->size == 0)
    {
        return 0;
    }

    for (word_count = a->size - 1; word_count > 0; --word_count)
    {
        if (a->data[word_count] != 0)
        {
            break;
        }
    }

    uint32_t top_word = a->data[word_count];

    size_t bytes = word_count * MPINT_INT_SIZE;

    for (; top_word != 0; top_word >>= 8)
    {
        ++bytes;
    }

    return bytes;
}

size_t mpint_get_length(const mpint_t *a)
{
    ssize_t i = 0;

    if (a->size == 0 || a->data == NULL)
    {
        return 0;
    }

    for (i = a->size - 1; i >= 0; --i)
    {
        if (a->data[i] != 0)
        {
            break;
        }
    }

    return i + 1;
}

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

void mpint_init(mpint_t *i)
{
    i->sign = 1;
    i->size = 0;
    i->data = NULL;
}

int mpint_inv_mod(mpint_t *r, const mpint_t *a, const mpint_t *p)
{
    int error;

    mpint_t b;
    mpint_t c;
    mpint_t q0;
    mpint_t r0;
    mpint_t t;
    mpint_t u;
    mpint_t v;

    mpint_init(&b);
    mpint_init(&c);
    mpint_init(&q0);
    mpint_init(&r0);
    mpint_init(&t);
    mpint_init(&u);
    mpint_init(&v);

    MPINT_GOTO_ON_ERROR(mpint_copy(&b, p), end);
    MPINT_GOTO_ON_ERROR(mpint_copy(&c, a), end);
    MPINT_GOTO_ON_ERROR(mpint_set_value(&u, 0), end);
    MPINT_GOTO_ON_ERROR(mpint_set_value(&v, 1), end);

    while (mpint_comp_int(&c, 0) > 0)
    {
        MPINT_GOTO_ON_ERROR(mpint_div(&q0, &r0, &b, &c), end);

        MPINT_GOTO_ON_ERROR(mpint_copy(&b, &c), end);
        MPINT_GOTO_ON_ERROR(mpint_copy(&c, &r0), end);

        MPINT_GOTO_ON_ERROR(mpint_copy(&t, &v), end);
        MPINT_GOTO_ON_ERROR(mpint_mul(&q0, &q0, &q0), end);
        MPINT_GOTO_ON_ERROR(mpint_sub(&v, &u, &q0), end);
        MPINT_GOTO_ON_ERROR(mpint_copy(&u, &t), end);
    }

    if (mpint_comp_int(&b, 0))
    {
        // TODO: Better error handling.
        MPINT_GOTO_ON_ERROR(-1, end);
    }

    if (mpint_comp_int(&u, 0) > 0)
    {
        MPINT_GOTO_ON_ERROR(mpint_copy(r, &u), end);
    }
    else
    {
        MPINT_GOTO_ON_ERROR(mpint_add(r, &u, p), end);
    }

end:
    mpint_free(&b);
    mpint_free(&c);
    mpint_free(&q0);
    mpint_free(&r0);
    mpint_free(&t);
    mpint_free(&u);
    mpint_free(&v);

    return error;
}

int mpint_mod(mpint_t *r, const mpint_t *a, const mpint_t *p)
{
    int error;
    int sign;
    mpint_t c;

    if (mpint_comp_int(p, 0) <= 0)
    {
        // TODO: Better error message.
        return -1;
    }

    mpint_init(&c);

    sign = a->sign;

    size_t m = mpint_get_bit_length(a);
    size_t n = mpint_get_bit_length(p);

    MPINT_GOTO_ON_ERROR(mpint_copy(r, a), end);

    if (m >= n)
    {
        MPINT_GOTO_ON_ERROR(mpint_copy(&c, p), end);
        MPINT_GOTO_ON_ERROR(mpint_shift_left(&c, m - n), end);

        while (mpint_comp_abs(r, p) >= 0)
        {
            if (mpint_comp_abs(r, &c) >= 0)
            {
                MPINT_GOTO_ON_ERROR(mpint_sub_abs(r, r, &c), end);
            }

            MPINT_GOTO_ON_ERROR(mpint_shift_right(&c, 1), end);
        }
    }

    if (sign < 0)
    {
        MPINT_GOTO_ON_ERROR(mpint_sub_abs(r, p, r), end);
    }

end:
    mpint_free(&c);
    return error;
}

int mpint_montgomery_mul(mpint_t *r,
                         const mpint_t *a,
                         const mpint_t *b,
                         uint32_t k,
                         const mpint_t *p,
                         mpint_t *t)
{
    int error;

    uint32_t i;
    uint32_t m;
    uint32_t n;
    uint32_t q;

    for (m = 2 - p->data[0], i = 0; i < 4; ++i)
    {
        m = m * (2 - m * p->data[0]);
    }

    m = ~m + 1;

    n = MIN(b->size, k);

    MPINT_GOTO_ON_ERROR(mpint_grow(t, 2 * k + 1), end);
    MPINT_GOTO_ON_ERROR(mpint_set_value(t, 0), end);

    for (i = 0; i < k; ++i)
    {
        if (i < a->size)
        {
            q = (t->data[i] + a->data[i] * b->data[0]) * m;
            mpint_mul_acc_core(t->data + i, b->data, n, a->data[i]);
        }
        else
        {
            q = t->data[i] * m;
        }

        mpint_mul_acc_core(t->data + i, p->data, k, q);
    }

    MPINT_GOTO_ON_ERROR(mpint_shift_right(t, k * (MPINT_INT_SIZE * 8)), end);
    MPINT_GOTO_ON_ERROR(mpint_copy(r, t), end);

    if (mpint_comp(r, p) >= 0)
    {
        MPINT_GOTO_ON_ERROR(mpint_sub(r, r, p), end);
    }

end:
    return error;
}

int mpint_montgomery_red(
    mpint_t *r, const mpint_t *a, uint32_t k, const mpint_t *p, mpint_t *t)
{
    uint32_t value;
    mpint_t b;

    value = 1;
    b.sign = 1;
    b.size = 1;
    b.data = &value;

    return mpint_montgomery_mul(r, a, &b, k, p, t);
}

void mpint_mul_acc_core(uint32_t *r,
                        const uint32_t *a,
                        size_t m,
                        const uint32_t b)
{
    size_t i;
    uint32_t carry;
    uint32_t product_lower;
    uint32_t product_upper;
    uint64_t product;

    carry = 0;
    product_lower = 0;
    product_upper = 0;

    for (i = 0; i < m; ++i)
    {
        product = (uint64_t)a[i] * b;
        product_lower = (uint32_t)product;
        product_upper = (uint32_t)(product >> 32);

        product_lower += carry;
        if (product_lower < carry)
        {
            ++product_upper;
        }

        product_lower += r[i];
        if (product_lower < r[i])
        {
            ++product_upper;
        }

        r[i] = product_lower;
        carry = product_upper;
    }

    for (; carry != 0; ++i)
    {
        r[i] += carry;
        carry = (r[i] < carry);
    }
}

int mpint_mul_int(mpint_t *r, const mpint_t *a, int b)
{
    uint32_t value;
    mpint_t t;

    value = _abs(b);
    t.sign = _sign(b);
    t.size = 1;
    t.data = &value;

    return mpint_mul(r, a, &t);
}

int mpint_mul_mod(mpint_t *r,
                  const mpint_t *a,
                  const mpint_t *b,
                  const mpint_t *p)
{
    int error;

    MPINT_GOTO_ON_ERROR(mpint_mul(r, a, b), end);
    MPINT_GOTO_ON_ERROR(mpint_mod(r, r, p), end);

end:
    return error;
}

int mpint_mul(mpint_t *r, const mpint_t *a, const mpint_t *b)
{
    int error;

    mpint_t temp_a;
    mpint_t temp_b;

    size_t a_len;
    size_t b_len;

    mpint_init(&temp_a);
    mpint_init(&temp_b);

    if (r == a)
    {
        MPINT_GOTO_ON_ERROR(mpint_copy(&temp_a, a), end);

        a = &temp_a;
    }

    if (r == b)
    {
        MPINT_GOTO_ON_ERROR(mpint_copy(&temp_b, b), end);

        b = &temp_b;
    }

    a_len = mpint_get_length(a);
    b_len = mpint_get_length(b);

    MPINT_GOTO_ON_ERROR(mpint_grow(r, a_len + b_len), end);

    r->sign = (a->sign == b->sign) ? 1 : -1;

    memset(r->data, 0, r->size * MPINT_INT_SIZE);

    if (a_len < b_len)
    {
        for (size_t i = 0; i < a_len; ++i)
        {
            mpint_mul_acc_core(&r->data[i], b->data, b_len, a->data[i]);
        }
    }
    else
    {
        for (size_t i = 0; i < b_len; ++i)
        {
            mpint_mul_acc_core(&r->data[i], a->data, a_len, b->data[i]);
        }
    }

end:
    mpint_free(&temp_a);
    mpint_free(&temp_b);

    return error;
}

int mpint_rand(mpint_t *r,
               size_t length,
               const crypto_prng_algo_t *prng_algo,
               void *prng_context)
{
    int error;

    size_t m;
    size_t n;

    n = (length + (MPINT_INT_SIZE * 8) - 1) / (MPINT_INT_SIZE * 8);
    m = length % (MPINT_INT_SIZE * 8);

    error = mpint_grow(r, n);

    if (error)
    {
        return error;
    }

    memset(r->data, 0, r->size * MPINT_INT_SIZE);
    r->sign = 1;

    error = prng_algo->read_func(
        prng_context, (uint8_t *)r->data, n * MPINT_INT_SIZE);

    if (error)
    {
        return error;
    }

    if (n > 0 && m > 0)
    {
        r->data[n - 1] &= (1 << m) - 1;
    }

    return 0;
}

int mpint_set_bit_value(mpint_t *r, size_t index, uint32_t value)
{
    int error;

    size_t word_index;
    size_t bit_index;

    word_index = index / (MPINT_INT_SIZE * 8);
    bit_index = index % (MPINT_INT_SIZE * 8);

    error = mpint_grow(r, word_index + 1);

    if (error)
    {
        return error;
    }

    if (value)
    {
        r->data[word_index] |= (1 << bit_index);
    }
    else
    {
        r->data[word_index] &= ~(1 << bit_index);
    }

    // TODO: Better error codes.
    return 0;
}

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

int mpint_shift_left(mpint_t *r, size_t n)
{
    int error;

    size_t shift_word_count = n / (MPINT_INT_SIZE * 8);
    size_t shift_bit_count = n % (MPINT_INT_SIZE * 8);

    if (r->size == 0 || n == 0)
    {
        return 0;
    }

    MPINT_GOTO_ON_ERROR(mpint_grow(r, r->size + (n + 31) / 32), end);

    if (shift_word_count > 0)
    {
        for (size_t i = r->size - 1; i >= shift_word_count; --i)
        {
            r->data[i] = r->data[i - shift_word_count];
        }

        for (size_t i = 0; i < shift_word_count; ++i)
        {
            r->data[i] = 0;
        }
    }

    if (shift_bit_count > 0)
    {
        for (size_t i = r->size - 1; i >= 1; --i)
        {
            r->data[i] = (r->data[i] << shift_bit_count) |
                         (r->data[i - 1] >> (32 - shift_bit_count));
        }

        r->data[0] <<= shift_bit_count;
    }

end:
    return error;
}

int mpint_shift_right(mpint_t *r, size_t n)
{
    int error;
    size_t i;
    size_t m;

    size_t shift_word_count = n / (MPINT_INT_SIZE * 8);
    size_t shift_bit_count = n % (MPINT_INT_SIZE * 8);

    if (shift_word_count >= r->size)
    {
        memset(r->data, 0, r->size * MPINT_INT_SIZE);
        return 0;
    }

    if (shift_word_count > 0)
    {
        for (m = r->size - shift_word_count, i = 0; i < m; ++i)
        {
            r->data[i] = r->data[i + shift_word_count];
        }

        for (i = m; i < r->size; ++i)
        {
            r->data[i] = 0;
        }
    }

    if (shift_bit_count > 0)
    {
        for (m = r->size - shift_word_count - 1, i = 0; i < m; ++i)
        {
            r->data[i] = (r->data[i] >> shift_bit_count) |
                         (r->data[i + 1] << (32 - shift_bit_count));
        }

        r->data[m] >>= shift_bit_count;
    }

    return 0;
}

int mpint_sub_abs(mpint_t *r, const mpint_t *a, const mpint_t *b)
{
    int error;

    if (mpint_comp_abs(a, b) < 0)
    {
        const mpint_t *t = a;
        a = b;
        b = t;
    }

    size_t a_len = mpint_get_length(a);
    size_t b_len = mpint_get_length(b);
    size_t i;
    uint32_t carry;
    uint32_t temp_data;

    MPINT_GOTO_ON_ERROR(mpint_grow(r, a_len), end);

    r->sign = 1;
    carry = 0;

    for (i = 0; i < b_len; ++i)
    {
        temp_data = a->data[i];

        if (carry != 0)
        {
            if (temp_data != 0)
            {
                carry = 0;
            }

            temp_data -= 1;
        }

        if (temp_data < b->data[i])
        {
            carry = 1;
        }

        r->data[i] = temp_data - b->data[i];
    }

    for (i = b_len; carry && i < a_len; ++i)
    {
        if (a->data[i] != 0)
        {
            carry = 0;
        }

        r->data[i] = a->data[i] - 1;
    }

    if (r != a)
    {
        for (; i < a_len; ++i)
        {
            r->data[i] = a->data[i];
        }

        for (; i < r->size; ++i)
        {
            r->data[i] = 0;
        }
    }

end:
    return error;
}

int mpint_sub_int(mpint_t *r, const mpint_t *a, int b)
{
    uint32_t value;
    mpint_t t;

    value = _abs(b);
    t.sign = _sign(b);
    t.size = 1;
    t.data = &value;

    return mpint_sub(r, a, &t);
}

int mpint_sub_mod(mpint_t *r,
                  const mpint_t *a,
                  const mpint_t *b,
                  const mpint_t *p)
{
    int error;

    MPINT_GOTO_ON_ERROR(mpint_sub(r, a, b), end);
    MPINT_GOTO_ON_ERROR(mpint_mod(r, r, p), end);

end:
    return error;
}

int mpint_sub(mpint_t *r, const mpint_t *a, const mpint_t *b)
{
    int error;
    int sign;

    sign = a->sign;

    if (a->sign == b->sign)
    {
        if (mpint_comp_abs(a, b) >= 0)
        {
            error = mpint_sub_abs(r, a, b);
            r->sign = sign;
            return error;
        }

        error = mpint_sub_abs(r, b, a);
        r->sign = -sign;
        return error;
    }

    error = mpint_add_abs(r, a, b);
    r->sign = sign;
    return error;
}

void mpint_swap(const mpint_t *a, const mpint_t *b)
{
    const mpint_t *t = a;
    a = b;
    b = t;
}

//=============================================================================
// End of file
//=============================================================================
