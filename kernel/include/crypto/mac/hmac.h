/**
 * @file hmac.h
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

#ifndef _CRYPTO_MAC_HMAC_H
#define _CRYPTO_MAC_HMAC_H

#include <crypto/crypto.h>
#include <crypto/hash/sha256.h>

#define HMAC_INNER_PAD 0x36
#define HMAC_OUTER_PAD 0x5c

typedef struct
{
    const crypto_hash_algo_t *hash;
    uint8_t hash_context[sizeof(sha256_context_t)];
    uint8_t key[SHA256_BLOCK_SIZE];
    uint8_t digest[SHA256_DIGEST_SIZE];
} hmac_context_t;

int hmac_init(hmac_context_t *context,
              const crypto_hash_algo_t *hash,
              const void *key,
              size_t key_len);
int hmac_compute(const crypto_hash_algo_t *hash,
                 const void *key,
                 size_t key_len,
                 const void *data,
                 size_t data_len,
                 uint8_t *digest);
void hmac_update(hmac_context_t *context, const void *data, size_t data_len);
void hmac_finalize(hmac_context_t *context, uint8_t *digest);
void hmac_finalize_raw(hmac_context_t *context, uint8_t *digest);

#endif

//=============================================================================
// End of file
//=============================================================================
