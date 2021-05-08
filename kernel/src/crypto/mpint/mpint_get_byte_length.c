/**
 * @file mpint_get_byte_length.c
 * @author Joakim Bertils
 * @version 0.1
 * @date 2021-05-08
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

#include <crypto/mpint/mpint.h>

size_t mpint_get_byte_length(const mpint_t *a)
{
    size_t word_count;

    if (a->size == 0)
    {
        return 0;
    }

    for (word_count = a->size - 1; word_count > 0; --word_count)
    {
        if (a->data[word_count] != 0)
        {
            break;
        }
    }

    uint32_t top_word = a->data[word_count];

    size_t bytes = word_count * MPINT_INT_SIZE;

    for (; top_word != 0; top_word >>= 8)
    {
        ++bytes;
    }

    return bytes;
}

//=============================================================================
// End of file
//=============================================================================
