/**
 * @file crypto_hash.h
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

#ifndef _CRYPTO_HASH_H
#define _CRYPTO_HASH_H

#include <stddef.h>
#include <stdint.h>

typedef error_t (*hash_algo_compute_func_t)(const void *data,
                                            size_t length,
                                            uint8_t *digest);
typedef void (*hash_algo_init_func_t)(void *context);
typedef void (*hash_algo_update_func_t)(void *context,
                                        const void *data,
                                        size_t length);
typedef void (*hash_algo_finalize_func_t)(void *context, uint8_t *digest);
typedef void (*hash_algo_finalize_raw_func_t)(void *context, uint8_t *digest);

typedef struct
{
    const char *name;
    const uint8_t *uid;
    size_t oid_size;
    size_t context_size;
    size_t block_size;
    size_t digest_size;
    size_t min_pad_size;
    int big_endian;

    hash_algo_compute_func_t compute_func;
    hash_algo_init_func_t init_func;
    hash_algo_update_func_t update_func;
    hash_algo_finalize_func_t finalize_func;
    hash_algo_finalize_raw_func_t finalize_raw_func;
} crypto_hash_algo_t;

#endif

//=============================================================================
// End of file
//=============================================================================
