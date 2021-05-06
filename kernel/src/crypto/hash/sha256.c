/**
 * @file sha256.c
 * @author Joakim Bertils
 * @version 0.1
 * @date 2021-05-06
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

#include <crypto/hash/sha256.h>

#include <stdlib.h>
#include <string.h>

//=============================================================================
// Local definitions
//=============================================================================

static const uint8_t padding[64] = {
    0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

// SHA-256 constants
static const uint32_t k[64] = {
    0x428A2F98, 0x71374491, 0xB5C0FBCF, 0xE9B5DBA5, 0x3956C25B, 0x59F111F1,
    0x923F82A4, 0xAB1C5ED5, 0xD807AA98, 0x12835B01, 0x243185BE, 0x550C7DC3,
    0x72BE5D74, 0x80DEB1FE, 0x9BDC06A7, 0xC19BF174, 0xE49B69C1, 0xEFBE4786,
    0x0FC19DC6, 0x240CA1CC, 0x2DE92C6F, 0x4A7484AA, 0x5CB0A9DC, 0x76F988DA,
    0x983E5152, 0xA831C66D, 0xB00327C8, 0xBF597FC7, 0xC6E00BF3, 0xD5A79147,
    0x06CA6351, 0x14292967, 0x27B70A85, 0x2E1B2138, 0x4D2C6DFC, 0x53380D13,
    0x650A7354, 0x766A0ABB, 0x81C2C92E, 0x92722C85, 0xA2BFE8A1, 0xA81A664B,
    0xC24B8B70, 0xC76C51A3, 0xD192E819, 0xD6990624, 0xF40E3585, 0x106AA070,
    0x19A4C116, 0x1E376C08, 0x2748774C, 0x34B0BCB5, 0x391C0CB3, 0x4ED8AA4A,
    0x5B9CCA4F, 0x682E6FF3, 0x748F82EE, 0x78A5636F, 0x84C87814, 0x8CC70208,
    0x90BEFFFA, 0xA4506CEB, 0xBEF9A3F7, 0xC67178F2};

#define W(x) (w[(x)&0x0F])

const crypto_hash_algo_t sha256_hash_algo = {
    "SHA-256",
    NULL,
    0,
    sizeof(sha256_context_t),
    SHA256_BLOCK_SIZE,
    SHA256_DIGEST_SIZE,
    SHA256_MIN_PAD_SIZE,
    1,
    (hash_algo_compute_func_t)sha256_compute,
    (hash_algo_init_func_t)sha256_init,
    (hash_algo_update_func_t)sha256_update,
    (hash_algo_finalize_func_t)sha256_finalize,
    (hash_algo_finalize_raw_func_t)sha256_finalize_raw

};

//=============================================================================
// Private function forward declarations
//=============================================================================

static inline uint32_t ch(uint32_t x, uint32_t y, uint32_t z);
static inline uint32_t maj(uint32_t x, uint32_t y, uint32_t z);
static inline uint32_t sigma1(uint32_t x);
static inline uint32_t sigma2(uint32_t x);
static inline uint32_t sigma3(uint32_t x);
static inline uint32_t sigma4(uint32_t x);

//=============================================================================
// Private functions
//=============================================================================

static inline uint32_t ch(uint32_t x, uint32_t y, uint32_t z)
{
    return (x & y) | (~x & z);
}

static inline uint32_t maj(uint32_t x, uint32_t y, uint32_t z)
{
    return (x & y) | (x & z) | (y & z);
}

static inline uint32_t sigma1(uint32_t x)
{
    return ror32(x, 2) ^ ror32(x, 13) ^ ror32(x, 22);
}

static inline uint32_t sigma2(uint32_t x)
{
    return ror32(x, 6) ^ ror32(x, 11) ^ ror32(x, 25);
}

static inline uint32_t sigma3(uint32_t x)
{
    return ror32(x, 7) ^ ror32(x, 18) ^ shr32(x, 3);
}

static inline uint32_t sigma4(uint32_t x)
{
    return ror32(x, 17) ^ ror32(x, 19) ^ shr32(x, 10);
}

//=============================================================================
// Interface functions
//=============================================================================

error_t sha256_compute(const void *data, size_t length, uint8_t *digest)
{
    sha256_context_t *context;

    // TODO: This should use a specialized malloc/free pair that always zeroizes
    // the memory and ideally allocates from a separate pool of memory.
    context = malloc(sizeof(sha256_context_t));

    if (context == NULL)
    {
        return -1;
    }

    sha256_init(context);
    sha256_update(context, data, length);
    sha256_finalize(context, digest);

    free(context);

    return 0;
}

void sha256_init(sha256_context_t *context)
{
    context->h[0] = 0x6A09E667;
    context->h[1] = 0xBB67AE85;
    context->h[2] = 0x3C6EF372;
    context->h[3] = 0xA54FF53A;
    context->h[4] = 0x510E527F;
    context->h[5] = 0x9B05688C;
    context->h[6] = 0x1F83D9AB;
    context->h[7] = 0x5BE0CD19;

    context->size = 0;
    context->total_size = 0;
}

#define MIN(x, y) (x > y ? y : x)

void sha256_update(sha256_context_t *context, const void *data, size_t length)
{
    size_t n;

    while (length > 0)
    {
        n = MIN(length, 64 - context->size);

        memcpy(context->buffer + context->size, data, n);

        context->size += n;
        context->total_size += n;
        data = (uint8_t *)data + n;
        length -= n;

        if (context->size == 64)
        {
            sha256_process_block(context);

            context->size = 0;
        }
    }
}

void sha256_finalize(sha256_context_t *context, uint8_t *digest)
{
    uint32_t i;
    size_t padding_size;
    uint64_t total_size;

    total_size = context->total_size;

    if (context->size < 56)
    {
        padding_size = 56 - context->size;
    }
    else
    {
        padding_size = 64 + 56 - context->size;
    }

    sha256_update(context, padding, padding_size);

    context->w[14] = htobe32((uint32_t)(total_size >> 32));
    context->w[15] = htobe32((uint32_t)total_size);

    sha256_process_block(context);

    for (i = 0; i < 8; ++i)
    {
        context->h[i] = htobe32(context->h[i]);
    }

    // It would be weird if the caller did not provide a digest buffer, but we
    // can make sure that it is not our fault that we cause a segmentation
    // fault.
    if (digest != NULL)
    {
        memcpy(digest, context->digest, SHA256_DIGEST_SIZE);
    }
}

void sha256_finalize_raw(sha256_context_t *context, uint8_t *digest)
{
    uint32_t i;

    if (digest == NULL)
    {
        return;
    }

    for (i = 0; i < 8; ++i)
    {
        context->h[i] = htobe32(context->h[i]);
    }

    memcpy(digest, context->digest, SHA256_DIGEST_SIZE);

    for (i = 0; i < 8; ++i)
    {
        context->h[i] = betoh32(context->h[i]);
    }
}

void sha256_process_block(sha256_context_t *context)
{
    uint32_t t;
    uint32_t temp1;
    uint32_t temp2;

    // Initialize the 8 working registers
    uint32_t a = context->h[0];
    uint32_t b = context->h[1];
    uint32_t c = context->h[2];
    uint32_t d = context->h[3];
    uint32_t e = context->h[4];
    uint32_t f = context->h[5];
    uint32_t g = context->h[6];
    uint32_t h = context->h[7];

    // Process message in 16-word blocks
    uint32_t *w = context->w;

    // Convert from big-endian byte order to host byte order
    for (t = 0; t < 16; t++)
    {
        w[t] = betoh32(w[t]);
    }

    // SHA-256 hash computation (alternate method)
    for (t = 0; t < 64; t++)
    {
        // Prepare the message schedule
        if (t >= 16)
            W(t) += sigma4(W(t + 14)) + W(t + 9) + sigma3(W(t + 1));

        // Calculate T1 and T2
        temp1 = h + sigma2(e) + ch(e, f, g) + k[t] + W(t);
        temp2 = sigma1(a) + maj(a, b, c);

        // Update the working registers
        h = g;
        g = f;
        f = e;
        e = d + temp1;
        d = c;
        c = b;
        b = a;
        a = temp1 + temp2;
    }

    // Update the hash value
    context->h[0] += a;
    context->h[1] += b;
    context->h[2] += c;
    context->h[3] += d;
    context->h[4] += e;
    context->h[5] += f;
    context->h[6] += g;
    context->h[7] += h;
}

//=============================================================================
// End of file
//=============================================================================
