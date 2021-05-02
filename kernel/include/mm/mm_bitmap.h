/**
 * @file mm_bitmap.h
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

#ifndef _MM_BITMAP_H
#define _MM_BITMAP_H

#include <stdint.h>

typedef uint64_t *mm_bitmap_t;

void mm_bitmap_set(mm_bitmap_t bitmap, uint64_t bit);
void mm_bitmap_clear(mm_bitmap_t bitmap, uint64_t bit);
uint64_t mm_bitmap_test(mm_bitmap_t bitmap, uint64_t bit);
uint64_t mm_bitmap_first_free(mm_bitmap_t bitmap, size_t bitmap_size);
uint64_t mm_bitmap_first_free_s(mm_bitmap_t bitmap,
                                size_t bitmap_size,
                                size_t n);

#endif

//=============================================================================
// End of file
//=============================================================================
