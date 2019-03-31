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

#endif