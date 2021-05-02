/**
 * @file mm_bitmap.c
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-06-22
 *
 * @brief
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

#include <mm/mm_bitmap.h>

#include <logging/logging.h>

#define BITS_PER_ENTRY 64

void mm_bitmap_set(mm_bitmap_t bitmap, uint64_t bit)
{
    if (!bitmap)
    {
        log_error("[BITMAP] Bitmap was NULL");
        return;
    }

    bitmap[bit / BITS_PER_ENTRY] |= (1 << (bit % BITS_PER_ENTRY));
}

void mm_bitmap_clear(mm_bitmap_t bitmap, uint64_t bit)
{
    if (!bitmap)
    {
        log_error("[BITMAP] Bitmap was NULL");
        return;
    }

    bitmap[bit / BITS_PER_ENTRY] &= ~(1 << (bit % BITS_PER_ENTRY));
}

uint64_t mm_bitmap_test(mm_bitmap_t bitmap, uint64_t bit)
{
    if (!bitmap)
    {
        log_error("[BITMAP] Bitmap was NULL");
        return 0;
    }

    return bitmap[bit / BITS_PER_ENTRY] & (1 << (bit % BITS_PER_ENTRY));
}

uint64_t mm_bitmap_first_free(mm_bitmap_t bitmap, size_t bitmap_size)
{
    if (!bitmap)
    {
        log_error("[BITMAP] Bitmap was NULL");
        return -1;
    }

    for (uint64_t entry = 0; entry < bitmap_size / BITS_PER_ENTRY; ++entry)
    {
        if (bitmap[entry] == 0xFFFFFFFFFFFFFFFF)
        {
            continue;
        }

        for (size_t bit = 0; bit < BITS_PER_ENTRY; ++bit)
        {
            if (!(bitmap[entry] & (1 << bit)))
            {
                return entry * BITS_PER_ENTRY + bit;
            }
        }
    }

    return -1;
}

uint64_t mm_bitmap_first_free_s(mm_bitmap_t bitmap,
                                size_t bitmap_size,
                                size_t n)
{
    if (!bitmap)
    {
        log_error("[BITMAP] Bitmap was NULL");
        return -1;
    }

    if (n == 0)
    {
        return -1;
    }

    uint64_t first_free = mm_bitmap_first_free(bitmap, bitmap_size);

    if ((int)first_free == -1)
    {
        return -1;
    }

    if (n == 1)
    {
        return first_free;
    }

    uint64_t contiguous_free = 0;

    uint64_t first_bit_in_chain = first_free;

    for (uint64_t bit = first_free; bit < bitmap_size; ++bit)
    {
        if (mm_bitmap_test(bitmap, bit))
        {
            contiguous_free = 0;
            first_bit_in_chain = bit + 1;
        }
        else
        {
            ++contiguous_free;
        }

        if (contiguous_free == n)
        {
            return first_bit_in_chain;
        }
    }

    return -1;
}

//=============================================================================
// End of file
//=============================================================================
