/**
 * @file malloc.c
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-06-17
 * 
 * @brief Memory allocation
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
#include <stdint.h>
#include <string.h>
#include <stdio.h>

// Note that this introduces dependency from libk to the kernel. It would be
// nice to handle this in a more noticable way.
extern void *kmalloc(size_t size);

__attribute__((malloc)) void *malloc(size_t size)
{
    return kmalloc(size);
}
