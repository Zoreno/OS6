#ifndef _MEMORY_INFO_H
#define _MEMORY_INFO_H

typedef struct
{
    uint64_t addr;
    uint64_t len;
} mem_region_t;

#define MEM_INFO_MAX_REGIONS 16

typedef struct
{
    uint64_t memory_size;

    mem_region_t regions[MEM_INFO_MAX_REGIONS];

    uint64_t kernel_load_addr;
    uint64_t kernel_size;
    uint64_t kernel_end;

} memory_info_t;

#endif