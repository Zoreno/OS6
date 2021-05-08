/**
 * @file mpint.h
 * @author Joakim Bertils
 * @version 0.1
 * @date 2021-05-08
 *
 * @brief Representation of arbitrarily large integers.
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

#ifndef _CRYPTO_MPINT_H
#define _CRYPTO_MPINT_H

#include <crypto/crypto.h>

#include <stdint.h>

typedef struct
{
    int sign;
    size_t size;
    uint32_t *data;

} mpint_t;

/**
 * @brief The size of the underlying type
 *
 *
 */
#define MPINT_INT_SIZE sizeof(*((mpint_t *)0)->data)

/**
 * @brief Initialize the integer data type.
 *
 * Must be called before using the integer in any way.
 *
 * @param i Pointer to where the integer is stored.
 *
 */
void mpint_init(mpint_t *i);

/**
 * @brief Frees and clears the memory associated with the integer.
 *
 * @param i Pointer to where the integer is stored.
 *
 */
void mpint_free(mpint_t *i);

/**
 * @brief Grows the integer, making room for @a new_size number of words.
 *
 * @param i Pointer to where the integer is stored.
 * @param new_size New requested size of the integer.
 *
 * @return Non-zero on allocation error.
 */
int mpint_grow(mpint_t *i, size_t new_size);

#endif

//=============================================================================
// End of file
//=============================================================================
