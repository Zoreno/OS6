/**
 * @file sha256.h
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

#ifndef _CRYPTO_HASH_SHA256_H
#define _CRYPTO_HASH_SHA256_H

#include <crypto/crypto.h>

#define SHA256_BLOCK_SIZE 64
#define SHA256_DIGEST_SIZE 32
#define SHA256_MIN_PAD_SIZE 9

typedef struct
{
    uint32_t h[8];
    uint32_t w[16];
    uint32_t digest[32];
    uint32_t buffer[64];

    size_t size;
    uint64_t total_size;
} sha256_context_t;

const crypto_hash_algo_t *sha256_get_algo();

error_t sha256_compute(const void *data, size_t length, uint8_t *digest);
void sha256_init(sha256_context_t *context);
void sha256_update(sha256_context_t *context, const void *data, size_t length);
void sha256_finalize(sha256_context_t *context, uint8_t *digest);
void sha256_finalize_raw(sha256_context_t *context, uint8_t *digest);
void sha256_process_block(sha256_context_t *context);

#endif

//=============================================================================
// End of file
//=============================================================================
