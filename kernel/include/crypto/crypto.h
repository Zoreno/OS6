/**
 * @file crypto.h
 * @author Joakim Bertils
 * @version 0.1
 * @date 2021-05-05
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

#ifndef _CRYPTO_H
#define _CRYPTO_H

#include <crypto/crypto_util.h>
#include <util/endian.h>

#include <stddef.h>
#include <stdint.h>

typedef enum
{
    CIPHER_TYPE_STREAM,
    CIPHER_TYPE_BLOCK
} cipher_algo_type_t;

typedef int error_t;

typedef error_t (*cipher_algo_init_func_t)(void *context,
                                           const uint8_t *key,
                                           size_t key_len);

typedef void (*cipher_algo_encrypt_stream_func_t)(void *context,
                                                  const uint8_t *input,
                                                  uint8_t *output,
                                                  size_t length);
typedef void (*cipher_algo_decrypt_stream_func_t)(void *context,
                                                  const uint8_t *input,
                                                  uint8_t *output,
                                                  size_t length);
typedef void (*cipher_algo_encrypt_block_func_t)(void *context,
                                                 const uint8_t *input,
                                                 uint8_t *output);
typedef void (*cipher_algo_decrypt_block_func_t)(void *context,
                                                 const uint8_t *input,
                                                 uint8_t *output);

typedef struct
{
    const char *name;
    size_t context_size;
    cipher_algo_type_t type;
    size_t block_size;
    cipher_algo_init_func_t init_func;
    cipher_algo_encrypt_stream_func_t encrypt_stream_func;
    cipher_algo_decrypt_stream_func_t decrypt_stream_func;
    cipher_algo_encrypt_block_func_t encrypt_block_func;
    cipher_algo_decrypt_block_func_t decrypt_block_func;
} cipher_algo_t;

#endif

//=============================================================================
// End of file
//=============================================================================
