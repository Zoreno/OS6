/**
 * @file mm_alloc.c
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

#include <mm_alloc.h>

extern void *malloc(size_t);
extern void free(void *);

void *_mm_malloc(size_t __size, size_t __align)
{
    void *__malloc_ptr;
    void *__aligned_ptr;

    if (__align & (__align - 1))
    {
        return ((void *)0);
    }

    if (__size == 0)
    {
        return ((void *)0);
    }

    if (__align < 2 * sizeof(void *))
    {
        __align = 2 * sizeof(void *);
    }

    __malloc_ptr = malloc(__size + __align);

    if (!__malloc_ptr)
    {
        return ((void *)0);
    }

    __aligned_ptr = (void *)(((size_t)__malloc_ptr + __align) & ~((size_t)(__align)-1));

    ((void **)__aligned_ptr)[-1] = __malloc_ptr;

    return __aligned_ptr;
}

void _mm_free(void *__ptr)
{
    if (__ptr)
    {
        free(((void **)__ptr)[-1]);
    }
}