/**
 * @file hmac.c
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

#include <crypto/mac/hmac.h>

#include <string.h>

int hmac_init(hmac_context_t *context,
              const crypto_hash_algo_t *hash,
              const void *key,
              size_t key_len)
{
    size_t i;

    if (context == NULL || hash == NULL)
    {
        return -1;
    }

    if (key == NULL && key_len != 0)
    {
        return -1;
    }

    context->hash = hash;

    if (key_len > hash->block_size)
    {
        hash->init_func(context->hash_context);
        hash->update_func(context->hash_context, key, key_len);
        hash->finalize_func(context->hash_context, context->key);

        crypto_zeroize(context->key + hash->digest_size,
                       hash->block_size - hash->digest_size);
    }
    else
    {
        memcpy(context->key, key, key_len);
        crypto_zeroize(context->key + key_len, hash->block_size - key_len);
    }

    for (i = 0; i < hash->block_size; ++i)
    {
        context->key[i] ^= HMAC_INNER_PAD;
    }

    hash->init_func(context->hash_context);
    hash->update_func(context->hash_context, context->key, hash->block_size);

    return 0;
}

int hmac_compute(const crypto_hash_algo_t *hash,
                 const void *key,
                 size_t key_len,
                 const void *data,
                 size_t data_len,
                 uint8_t *digest)
{
    int error;
    hmac_context_t *context;

    context = crypto_malloc(sizeof(hmac_context_t));

    if (context != NULL)
    {
        error = hmac_init(context, hash, key, key_len);

        if (!error)
        {
            hmac_update(context, data, data_len);
            hmac_finalize(context, digest);
        }

        crypto_free(context);
    }
    else
    {
        error = -1;
    }

    return error;
}

void hmac_update(hmac_context_t *context, const void *data, size_t data_len)
{
    const crypto_hash_algo_t *hash;

    hash = context->hash;

    hash->update_func(context->hash_context, data, data_len);
}

void hmac_finalize(hmac_context_t *context, uint8_t *digest)
{
    size_t i;
    const crypto_hash_algo_t *hash;

    hash = context->hash;
    hash->finalize_func(context->hash_context, context->digest);

    for (i = 0; i < hash->block_size; ++i)
    {
        context->key[i] ^= HMAC_INNER_PAD ^ HMAC_OUTER_PAD;
    }

    hash->init_func(context->hash_context);
    hash->update_func(context->hash_context, context->key, hash->block_size);
    hash->update_func(
        context->hash_context, context->digest, hash->digest_size);
    hash->finalize_func(context->hash_context, context->digest);

    if (digest != NULL)
    {
        memcpy(digest, context->digest, hash->digest_size);
    }
}

void hmac_finalize_raw(hmac_context_t *context, uint8_t *digest)
{
    size_t i;
    const crypto_hash_algo_t *hash;

    hash = context->hash;

    for (i = 0; i < hash->block_size; ++i)
    {
        context->key[i] ^= HMAC_INNER_PAD ^ HMAC_OUTER_PAD;
    }

    hash->init_func(context->hash_context);
    hash->update_func(context->hash_context, context->key, hash->block_size);
    hash->update_func(
        context->hash_context, context->digest, hash->digest_size);
    hash->finalize_func(context->hash_context, context->digest);

    if (digest != NULL)
    {
        memcpy(digest, context->digest, hash->digest_size);
    }
}

//=============================================================================
// End of file
//=============================================================================
