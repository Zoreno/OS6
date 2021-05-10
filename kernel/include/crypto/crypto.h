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

#include <crypto/crypto_cipher.h>
#include <crypto/crypto_hash.h>
#include <crypto/crypto_prng.h>
#include <crypto/crypto_util.h>
#include <util/endian.h>

void *crypto_malloc(size_t size);
void crypto_free(void *p);

/**
 * @brief Zeroizes a chunk of memory.
 *
 * The function is written in such a way that there is no risk of this function
 * being optimized away. It is important to use this function instead of memset
 * when clearing memory in the crypto module.
 *
 * @param p Pointer to start of memory to clear.
 * @param size Size of the memory chunk.
 *
 * @return Pointer to the zeroized memory chunk.
 */
void *crypto_zeroize(void *p, size_t size);

#endif

//=============================================================================
// End of file
//=============================================================================
