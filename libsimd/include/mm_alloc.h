/**
 * @file mm_alloc.h
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-06-30
 * 
 * @brief MMX intrinsics memory allocation
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

#ifndef _LIBSIMD_MM_ALLOC_H
#define _LIBSIMD_MM_ALLOC_H

#include <stdint.h>

void *_mm_malloc(size_t __size, size_t __align);
void _mm_free(void *__ptr);

#endif

//=============================================================================
// End of file
//=============================================================================