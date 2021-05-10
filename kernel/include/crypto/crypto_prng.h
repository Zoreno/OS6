/**
 * @file crypto_prng.h
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

#ifndef _CRYPTO_PRNG_H
#define _CRYPTO_PRNG_H

#include <stddef.h>
#include <stdint.h>

typedef int (*prng_algo_init_func_t)(void *context);
typedef void (*prng_algo_release_func_t)(void *context);
typedef int (*prng_algo_seed_func_t)(void *context,
                                     const uint8_t *input,
                                     size_t length);
typedef int (*prng_algo_add_entropy_func_t)(void *context,
                                            size_t source,
                                            const uint8_t *input,
                                            size_t length,
                                            size_t entropy);
typedef int (*prng_algo_read_func_t)(void *context,
                                     uint8_t *output,
                                     size_t length);

typedef struct
{
    const char *name;
    size_t context_size;
    prng_algo_init_func_t init_func;
    prng_algo_release_func_t release_func;
    prng_algo_seed_func_t seed_func;
    prng_algo_add_entropy_func_t add_entropy_func;
    prng_algo_read_func_t read_func;
} crypto_prng_algo_t;

#endif

//=============================================================================
// End of file
//=============================================================================
