/**
 * @file phys_mem.h
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

#ifndef _PHYS_MEM_H
#define _PHYS_MEM_H

#include <stdint.h>
#include <mm/memory_info.h>

typedef uint64_t phys_addr;

void phys_mem_init(memory_info_t *memory_info);
void phys_mem_init_region(phys_addr base, size_t size);
void phys_mem_deinit_region(phys_addr base, size_t size);

void *phys_mem_alloc_block();
void *phys_mem_alloc_block_z();
void *phys_mem_alloc_blocks(size_t blocks);
void *phys_mem_alloc_blocks_z(size_t blocks);

void phys_mem_free_block(void *base);
void phys_mem_free_blocks(void *base, size_t size);

size_t phys_mem_size();
size_t phys_mem_get_block_count();
size_t phys_mem_get_used_block_count();
size_t phys_mem_get_free_block_count();
size_t phys_mem_get_block_size();

void phys_mem_dump_statistics();

#endif

//=============================================================================
// End of file
//=============================================================================