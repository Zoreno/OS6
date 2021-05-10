/**
 * @file yarrow.h
 * @author Joakim Bertils
 * @version 0.1
 * @date 2021-05-10
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

#ifndef _CRYPTO_PRNG_YARROW_H
#define _CRYPTO_PRGN_YARROW_H

#include <crypto/cipher/aes.h>
#include <crypto/crypto.h>
#include <crypto/hash/sha256.h>

#define YARROW_N 3
#define YARROW_K 2
#define YARROW_PG 10
#define YARROW_FAST_THRESHOLD 100
#define YARROW_SLOW_THRESHOLD 160

typedef enum
{
    YARROW_POOL_FAST,
    YARROW_POOL_SLOW
} yarrow_pool_id_t;

typedef struct
{
    sha256_context_t context;
    size_t entropy[YARROW_N];
} yarrow_pool_t;

typedef struct
{
    int ready;

    yarrow_pool_id_t current_pool[YARROW_N];
    yarrow_pool_t fast_pool;
    yarrow_pool_t slow_pool;

    aes_context_t cipher_context;
    uint8_t key[32];
    uint8_t counter[16];

    size_t block_count;
} yarrow_context_t;

const crypto_prng_algo_t *yarrow_get_algo();

int yarrow_init(yarrow_context_t *context);
void yarrow_release(yarrow_context_t *context);
int yarrow_seed(yarrow_context_t *context, const uint8_t *input, size_t length);
int yarrow_add_entropy(yarrow_context_t *context,
                       size_t source,
                       const uint8_t *input,
                       size_t length,
                       size_t entropy);
int yarrow_read(yarrow_context_t *context, uint8_t *output, size_t length);

#endif
