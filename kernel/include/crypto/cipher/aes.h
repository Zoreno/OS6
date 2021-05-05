/**
 * @file aes.h
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

#ifndef _CRYPTO_AES_H
#define _CRYPTO_AES_H

#include <crypto/crypto.h>

#define AES_BLOCK_SIZE 16

typedef struct
{
    uint32_t nr;
    uint32_t ek[60];
    uint32_t dk[60];
} aes_context_t;

const cipher_algo_t *aes_get_algo();
error_t aes_init(aes_context_t *context, const uint8_t *key, size_t key_len);
void aes_encrypt_block(aes_context_t *context,
                       const uint8_t *input,
                       uint8_t *output);
void aes_decrypt_block(aes_context_t *context,
                       const uint8_t *input,
                       uint8_t *output);

#endif

//=============================================================================
// End of file
//=============================================================================
