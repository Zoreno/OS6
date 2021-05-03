/**
 * @file bitset.h
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-04-19
 *
 * @brief Type representing a set of bits that may be individually set or
 * cleared
 *
 * @copyright Copyright (C) 2019,
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

#ifndef _BITSET_H
#define _BITSET_H

#include <stdint.h>
#include <stddef.h>

/**
 * @brief Struct representing a set of bits
 *
 *
 */
typedef struct
{
    /**
     * @brief Pointer to the data
     *
     *
     */
    unsigned char *data;

    /**
     * @brief Size of the bitset (in bits)
     *
     *
     */
    size_t size;
} bitset_t;

/**
 * @brief Initializes the bitset, setting all bits to zero
 *
 * @param set Pointer to set struct
 * @param size Size to allocate in bits
 *
 */
void bitset_init(bitset_t *set, size_t size);

/**
 * @brief Frees the bitset
 *
 * @param set Pointer to set struct
 *
 */
void bitset_free(bitset_t *set);

/**
 * @brief Sets a bit in the bitset
 *
 * @param set Pointer to set
 * @param bit Bit to set
 *
 */
void bitset_set(bitset_t *set, size_t bit);

/**
 * @brief Clears a bit in the bitset
 *
 * @param set Pointer to set
 * @param bit Bit to clear
 *
 */
void bitset_clear(bitset_t *set, size_t bit);

/**
 * @brief Test if a bit is set
 *
 * @param set Pointer to bit struct
 * @param bit Bit to test
 *
 * @return int Non-zero if bit is set.
 */
int bitset_test(bitset_t *set, size_t bit);

/**
 * @brief Find the first unset bit and set it
 *
 * @param set Pointer to set
 *
 * @return int First unset bit
 */
int bitset_ffub(bitset_t *set);

#endif

//=============================================================================
// End of file
//=============================================================================
