/**
 * @file kheap.h
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

#ifndef _KHEAP_H
#define _KHEAP_H

#include <stdint.h>

#define KHEAP_ALIGNMENT_MASK 0x00FFFFFFFFFFFFFF

#define BIT(i) (1 << (i))

#define HEAP_WITHIN_PAGE BIT(56)
#define HEAP_WITHIN_64K BIT(57)
#define HEAP_CONTINUOUS BIT(63)

void *kheap_get_current_end();
void kheap_init();
void *kmalloc(size_t size);
void *kmalloc_a(size_t size, uint64_t alignment);
void kfree(void *addr);

#endif