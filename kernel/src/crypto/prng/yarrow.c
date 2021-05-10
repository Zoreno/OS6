/**
 * @file yarrow.c
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

#include <crypto/prng/yarrow.h>

#include <string.h>

//=============================================================================
// Private data
//=============================================================================

const crypto_prng_algo_t yarrow_prng_algo = {
    "Yarrow",
    sizeof(yarrow_context_t),
    (prng_algo_init_func_t)yarrow_init,
    (prng_algo_release_func_t)yarrow_release,
    (prng_algo_seed_func_t)yarrow_seed,
    (prng_algo_add_entropy_func_t)yarrow_add_entropy,
    (prng_algo_read_func_t)yarrow_read,
};

//=============================================================================
// Private function forward declarations
//=============================================================================

static void generate_block(yarrow_context_t *context, uint8_t *output);
static void fast_reseed(yarrow_context_t *context);
static void slow_reseed(yarrow_context_t *context);

//=============================================================================
// Private functions
//=============================================================================

static void generate_block(yarrow_context_t *context, uint8_t *output)
{
    int i;

    aes_encrypt_block(&context->cipher_context, context->counter, output);

    for (i = AES_BLOCK_SIZE - 1; i >= 0; --i)
    {
        if (++(context->counter[i]) != 0)
        {
            break;
        }
    }
}

static void fast_reseed(yarrow_context_t *context)
{
    int i;

    sha256_update(
        &context->fast_pool.context, context->key, sizeof(context->key));
    sha256_finalize(&context->fast_pool.context, context->key);

    aes_init(&context->cipher_context, context->key, sizeof(context->key));

    crypto_zeroize(context->counter, sizeof(context->counter));
    aes_encrypt_block(
        &context->cipher_context, context->counter, context->counter);

    sha256_init(&context->fast_pool.context);

    for (i = 0; i < YARROW_N; ++i)
    {
        context->fast_pool.entropy[i] = 0;
    }

    context->ready = 1;
}

static void slow_reseed(yarrow_context_t *context)
{
    size_t i;

    sha256_finalize(&context->fast_pool.context, NULL);

    sha256_update(
        &context->slow_pool.context, context->key, sizeof(context->key));
    sha256_update(&context->slow_pool.context,
                  context->fast_pool.context.digest,
                  SHA256_DIGEST_SIZE);
    sha256_finalize(&context->slow_pool.context, context->key);

    aes_init(&context->cipher_context, context->key, sizeof(context->key));

    crypto_zeroize(context->counter, sizeof(context->counter));
    aes_encrypt_block(
        &context->cipher_context, context->counter, context->counter);

    sha256_init(&context->fast_pool.context);
    sha256_init(&context->slow_pool.context);

    for (i = 0; i < YARROW_N; ++i)
    {
        context->fast_pool.entropy[i] = 0;
        context->fast_pool.entropy[i] = 0;
    }

    context->ready = 1;
}

//=============================================================================
// Interface functions
//=============================================================================

const crypto_prng_algo_t *yarrow_get_algo()
{
    return &yarrow_prng_algo;
}

int yarrow_init(yarrow_context_t *context)
{
    crypto_zeroize(context, sizeof(yarrow_context_t));

    // TODO: Create mutex.

    sha256_init(&context->fast_pool.context);
    sha256_init(&context->slow_pool.context);

    context->ready = 0;

    return 0;
}

void yarrow_release(yarrow_context_t *context)
{
    crypto_zeroize(context, sizeof(yarrow_context_t));
}

int yarrow_seed(yarrow_context_t *context, const uint8_t *input, size_t length)
{
    if (length < sizeof(context->key))
    {
        // The seed has to be longer than the size of the key.
        return -1;
    }

    sha256_update(&context->fast_pool.context, input, length);
    fast_reseed(context);

    return 0;
}

// TODO: Move this to a utility file.
#define MIN(a, b) (((a) > (b)) ? (b) : (a))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

int yarrow_add_entropy(yarrow_context_t *context,
                       size_t source,
                       const uint8_t *input,
                       size_t length,
                       size_t entropy)
{
    size_t i;
    size_t k;

    if (source >= YARROW_N)
    {
        return -1;
    }

    if (context->current_pool[source] == YARROW_POOL_FAST)
    {
        sha256_update(&context->fast_pool.context, input, length);
        context->fast_pool.entropy[source] += entropy;

        if (context->fast_pool.entropy[source] >= YARROW_FAST_THRESHOLD)
        {
            fast_reseed(context);
        }

        context->current_pool[source] = YARROW_POOL_SLOW;
    }
    else
    {
        sha256_update(&context->slow_pool.context, input, length);
        context->slow_pool.entropy[source] += entropy;

        context->slow_pool.entropy[source] =
            MIN(context->slow_pool.entropy[source], YARROW_SLOW_THRESHOLD);

        for (k = 0, i = 0; i < YARROW_N; ++i)
        {
            if (context->slow_pool.entropy[i] >= YARROW_SLOW_THRESHOLD)
            {
                ++k;
            }
        }

        if (k >= YARROW_K)
        {
            slow_reseed(context);
        }

        context->current_pool[source] = YARROW_POOL_FAST;
    }

    return 0;
}

int yarrow_read(yarrow_context_t *context, uint8_t *output, size_t length)
{
    size_t n;
    uint8_t buffer[AES_BLOCK_SIZE];

    if (!context->ready)
    {
        return -1;
    }

    while (length > 0)
    {
        n = MIN(length, AES_BLOCK_SIZE);

        generate_block(context, buffer);

        memcpy(output, buffer, n);

        context->block_count++;

        output += n;
        length -= n;
    }

    if (context->block_count >= YARROW_PG)
    {
        generate_block(context, context->key);

        aes_init(&context->cipher_context, context->key, sizeof(context->key));

        context->block_count = 0;
    }

    return 0;
}

//=============================================================================
// End of file
//=============================================================================
