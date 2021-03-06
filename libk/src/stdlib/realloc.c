/**
 * @file realloc.c
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-06-17
 * 
 * @brief Realloc memory
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

#include <stdlib.h>

// This works because libk is statically linked to the kernel
extern void *krealloc(void *ptr, size_t size);
extern void *kmalloc(size_t size);

__attribute__((malloc)) void *realloc(void *ptr, size_t size)
{
    if (!ptr)
    {
        return kmalloc(size);
    }

    return krealloc(ptr, size);
}
