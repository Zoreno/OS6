/**
 * @file phys_mem.c
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

#include <logging/logging.h>
#include <mm/mm_bitmap.h>
#include <mm/phys_mem.h>

#include <stdio.h>
#include <string.h>

#define PHYS_MEM_BLOCKS_PER_BYTE 8
#define PHYS_MEM_BLOCK_SIZE 4096
#define PHYS_MEM_BLOCK_ALIGN PHYS_MEM_BLOCK_SIZE

typedef struct
{
    uint64_t memory_size;

    uint64_t used_blocks;
    uint64_t max_blocks;

    mm_bitmap_t bitmap;
} phys_mem_t;

static phys_mem_t _phys_mem = {0, 0, 0, 0};

static uint64_t align_up(uint64_t val, uint64_t align);
static uint64_t align_down(uint64_t val, uint64_t align);

static uint64_t get_frame(uint64_t addr);
static uint64_t get_addr(uint64_t frame);

static uint64_t align_up(uint64_t val, uint64_t align)
{
    if (!align)
    {
        return val;
    }

    --align;

    return (val + align) & ~align;
}

static uint64_t align_down(uint64_t val, uint64_t align)
{
    if (!align)
    {
        return val;
    }

    return val & ~(align + 1);
}

static uint64_t get_frame(uint64_t addr)
{
    return align_down(addr, PHYS_MEM_BLOCK_SIZE) / PHYS_MEM_BLOCK_SIZE;
}

static uint64_t get_addr(uint64_t frame)
{
    return frame * PHYS_MEM_BLOCK_SIZE;
}

void phys_mem_init(memory_info_t *mem_info)
{
    _phys_mem.memory_size = mem_info->memory_size;
    _phys_mem.bitmap =
        (mm_bitmap_t)(align_up(mem_info->kernel_end, PHYS_MEM_BLOCK_SIZE));
    _phys_mem.max_blocks =
        align_up(mem_info->memory_size, PHYS_MEM_BLOCK_SIZE) /
        PHYS_MEM_BLOCK_SIZE;
    _phys_mem.used_blocks = _phys_mem.max_blocks;

    log_debug("[PMM] Bitmap size: %i",
              phys_mem_get_block_count() / PHYS_MEM_BLOCKS_PER_BYTE);

    int entries = (phys_mem_get_block_count() + 63) / 64;

    for (int i = 0; i < entries; ++i)
    {
        _phys_mem.bitmap[i] = 0xFFFFFFFFFFFFFFFF;
    }

    for (int i = 0; i < MEM_INFO_MAX_REGIONS; ++i)
    {
        if (mem_info->regions[i].addr != 0)
        {
            phys_mem_init_region(mem_info->regions[i].addr,
                                 mem_info->regions[i].len);
        }
    }

    // Deinit the memory occupied by the kernel
    phys_mem_deinit_region(mem_info->kernel_load_addr, mem_info->kernel_size);

    // Deinit memory used for the bitmap
    phys_mem_deinit_region(
        (phys_addr)_phys_mem.bitmap,
        phys_mem_get_block_count() / PHYS_MEM_BLOCKS_PER_BYTE);

    log_info("[PMM] Initialized! Bitmap address: %#016x, (Entries: %i)",
             _phys_mem.bitmap,
             entries);
}

void phys_mem_init_region(phys_addr base, size_t size)
{
    uint64_t align = base / PHYS_MEM_BLOCK_ALIGN;
    uint64_t blocks = align_up(size, PHYS_MEM_BLOCK_SIZE) / PHYS_MEM_BLOCK_SIZE;

    log_debug("[PMM] Initing region: %#016x, (%i bytes)(Frame: %i count: %i)",
              base,
              size,
              align,
              blocks);

    for (uint64_t i = 0; i < blocks; ++i)
    {
        mm_bitmap_clear(_phys_mem.bitmap, align + i);
        --_phys_mem.used_blocks;
    }

    mm_bitmap_set(_phys_mem.bitmap, 0);
}

void phys_mem_deinit_region(phys_addr base, size_t size)
{
    uint64_t align = base / PHYS_MEM_BLOCK_ALIGN;
    uint64_t blocks = align_up(size, PHYS_MEM_BLOCK_SIZE) / PHYS_MEM_BLOCK_SIZE;

    log_debug("[PMM] Deiniting region: %#016x, (%i bytes)(Frame: %i count: %i)",
              base,
              size,
              align,
              blocks);

    for (uint64_t i = 0; i < blocks; ++i)
    {
        mm_bitmap_set(_phys_mem.bitmap, align + i);
        --_phys_mem.used_blocks;
    }
}

void *phys_mem_alloc_block()
{
    if (phys_mem_get_block_count() == 0)
    {
        log_error("[PMM] Out of memory");
        return 0;
    }

    uint64_t frame =
        mm_bitmap_first_free(_phys_mem.bitmap, _phys_mem.max_blocks);

    if ((int)frame == -1)
    {
        log_error("[PMM] Out of memory");
        return 0;
    }

    if ((int)frame == 0)
    {
        log_error("[PMM] Frame zero returned");
        return 0;
    }

    mm_bitmap_set(_phys_mem.bitmap, frame);

    phys_addr addr = frame * PHYS_MEM_BLOCK_SIZE;
    ++_phys_mem.used_blocks;

    return (void *)addr;
}

void *phys_mem_alloc_block_z()
{
    void *addr = phys_mem_alloc_block();

    if (addr)
    {
        memset(addr, 0, PHYS_MEM_BLOCK_SIZE);
    }

    return addr;
}

void *phys_mem_alloc_blocks(size_t blocks)
{
    if (phys_mem_get_block_count() < blocks)
    {
        log_error("[PMM] Out of memory");
        return 0;
    }

    uint64_t frame =
        mm_bitmap_first_free_s(_phys_mem.bitmap, _phys_mem.max_blocks, blocks);

    if ((int)frame == -1)
    {
        log_error("[PMM] Out of memory");
        return 0;
    }

    for (uint64_t i = 0; i < blocks; ++i)
    {
        mm_bitmap_set(_phys_mem.bitmap, frame + i);
    }

    phys_addr addr = frame * PHYS_MEM_BLOCK_SIZE;
    _phys_mem.used_blocks += blocks;

    return (void *)addr;
}

void *phys_mem_alloc_blocks_z(size_t blocks)
{
    void *addr = phys_mem_alloc_blocks(blocks);

    if (addr)
    {
        memset(addr, 0, blocks * PHYS_MEM_BLOCK_SIZE);
    }

    return addr;
}

void phys_mem_free_block(void *base)
{
    phys_addr addr = (phys_addr)base;
    uint64_t frame = addr / PHYS_MEM_BLOCK_SIZE;

    mm_bitmap_clear(_phys_mem.bitmap, frame);

    --_phys_mem.used_blocks;
}

void phys_mem_free_blocks(void *base, size_t size)
{
    phys_addr addr = (phys_addr)base;
    uint64_t frame = addr / PHYS_MEM_BLOCK_SIZE;

    for (uint64_t i = 0; i < size; ++i)
    {
        mm_bitmap_clear(_phys_mem.bitmap, frame + i);
    }

    _phys_mem.used_blocks -= size;
}

size_t phys_mem_size()
{
    return _phys_mem.memory_size;
}

size_t phys_mem_get_block_count()
{
    return _phys_mem.max_blocks;
}

size_t phys_mem_get_used_block_count()
{
    return _phys_mem.used_blocks;
}

size_t phys_mem_get_free_block_count()
{
    return _phys_mem.max_blocks - _phys_mem.used_blocks;
}

size_t phys_mem_get_block_size()
{
    return PHYS_MEM_BLOCK_SIZE;
}

void phys_mem_dump_statistics()
{
    log_debug("[PHYSMEM] Used Memory Blocks: %i/%i (%i/%i Bytes)",
              phys_mem_get_used_block_count(),
              phys_mem_get_block_count(),
              phys_mem_get_used_block_count() * phys_mem_get_block_size(),
              phys_mem_get_block_count() * phys_mem_get_block_size());
}

//=============================================================================
// End of file
//=============================================================================
