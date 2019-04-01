#include <mm/kheap.h>
#include <mm/phys_mem.h>
#include <mm/virt_mem.h>

#include <string.h>
#include <stdio.h>

//==============================================================================
// Definitions
//==============================================================================

#define PLACEMENT_BEGIN 0xFD0000000
#define PLACEMENT_END 0xFD0200000
#define KHEAP_BEGIN 0xFD0200000
#define KHEAP_END 0xFE0000000

#define PAGE_SIZE 4096

//==============================================================================
// Structures
//==============================================================================

typedef struct
{
    uint64_t res : 1;
    uint64_t size : 63;

    uint64_t number;
} kheap_region_t;

//==============================================================================
// Private variables
//==============================================================================

static kheap_region_t *regions = 0;
static uint64_t region_count = 0;
static uint64_t region_max_count = 0;
static uint64_t first_free_region = 0;

static void *first_free_addr = (void *)KHEAP_BEGIN;
static const uint8_t *HEAP_START = (const uint8_t *)KHEAP_BEGIN;
static uint64_t heap_size = 0;
static const uint64_t HEAP_MIN_GROWTH = 0x10000;

//==============================================================================
// Private function forwards
//==============================================================================

static uint64_t align_up(uint64_t val, uint64_t align);
static uint64_t max(uint64_t v1, uint64_t v2);
static void *pmalloc(size_t size, uint64_t alignment);
static int heap_grow(size_t size, uint8_t *heap_end, int cont);
static void *kmalloc_imp(size_t size, uint64_t alignment);
static void kfree_imp(void *addr);

//==============================================================================
// private Functions
//==============================================================================

static uint64_t align_up(uint64_t val, uint64_t align)
{
    if (!align)
    {
        return val;
    }

    --align;

    return (val + align) & ~align;
}

static uint64_t max(uint64_t v1, uint64_t v2)
{
    return v1 > v2 ? v1 : v2;
}

static void *pmalloc(size_t size, uint64_t alignment)
{
    static uint8_t *nextPlacement = (uint8_t *)PLACEMENT_BEGIN;

    size = align_up(size, 8);

    uint8_t *currPlacement = (uint8_t *)align_up((uint64_t)nextPlacement, alignment);

    if (((uint64_t)currPlacement + size) > PLACEMENT_END)
    {
        return 0;
    }

    return (void *)currPlacement;
}

static int heap_grow(size_t size, uint8_t *heap_end, int cont)
{
    (void)cont;

    if ((region_count > 0) && regions[region_count - 1].res && (region_count >= region_max_count))
    {
        return 0;
    }

    uint64_t offset = 0;

    while (offset < size)
    {
        void *addr = phys_mem_alloc_block();

        virt_mem_map_page(addr, heap_end + offset, 0);

        offset += PAGE_SIZE;
    }

    if ((region_count > 0) && !regions[region_count - 1].res)
    {
        regions[region_count - 1].size += size;
    }
    else
    {
        regions[region_count].res = 0;
        regions[region_count].size = size;
        regions[region_count].number = 0;

        ++region_count;
    }

    heap_size += size;

    return 1;
}

static void *kmalloc_imp(size_t size, uint64_t alignment)
{
    static uint64_t consecutive_number = 0;

    size_t within = 0xFFFFFFFFF;

    if (alignment & HEAP_WITHIN_PAGE)
    {
        within = PAGE_SIZE;
    }

    if (alignment & HEAP_WITHIN_64K)
    {
        within = 0x10000;
    }

    int cont = (alignment & HEAP_CONTINUOUS) ? 1 : 0;

    alignment &= KHEAP_ALIGNMENT_MASK;

    if (regions == 0)
    {
        return pmalloc(size, alignment);
    }

    size = align_up(size, alignment);

    int found_free = 0;

    uint8_t *region_address = (uint8_t *)first_free_addr;

    for (uint64_t i = first_free_region; i < region_count; ++i)
    {
        if (regions[i].res)
        {
            found_free = 1;
        }
        else if (!found_free)
        {
            first_free_region = i;
            first_free_addr = region_address;
        }

        uint8_t *aligned_address = (uint8_t *)align_up((uintptr_t)region_address, alignment);
        uintptr_t additional_size = (uintptr_t)aligned_address - (uintptr_t)region_address;

        if (!regions[i].res && (regions[i].size >= size + additional_size) && (within - (uintptr_t)region_address % within >= additional_size))
        {
            if (aligned_address != region_address)
            {
                if (region_count >= region_max_count)
                {
                    return 0;
                }

                memmove(regions + i + 1, regions + i, (region_count - i) * sizeof(kheap_region_t));

                ++region_count;
                regions[i].size = aligned_address - region_address;
                regions[i].res = 0;
                regions[i + 1].size -= regions[i].size;
                region_address += regions[i].size;

                ++i;
            }

            if (regions[i].size > size + additional_size)
            {
                if (region_count + 1 > region_max_count)
                {
                    return 0;
                }

                memmove(regions + i + 2, regions + i + 1, (region_count - i - 1) * sizeof(kheap_region_t));

                ++region_count;

                regions[i + 1].size = regions[i].size - size;
                regions[i + 1].res = 0;
                regions[i + 1].number = 0;

                regions[i].size = size;
            }

            regions[i].res = 1;
            regions[i].number = consecutive_number++;

            return region_address;
        }

        region_address += regions[i].size;
    }

    uint64_t size_to_grow = max(HEAP_MIN_GROWTH, align_up(size * 3 / 2, PAGE_SIZE));

    if (!heap_grow(size_to_grow, (uint8_t *)((uintptr_t)HEAP_START + heap_size), cont))
    {
        return 0;
    }

    return kmalloc_imp(size, alignment);
}

static void kfree_imp(void *addr)
{
    if (!addr)
    {
        return;
    }

    uint8_t *region_address = (uint8_t *)HEAP_START;

    for (uint64_t i = 0; i < region_count; ++i)
    {
        if (region_address == addr && regions[i].res)
        {
            regions[i].number = 0;
            regions[i].res = 0;

            if (((i + 1) < region_count) && !regions[i + 1].res)
            {
                regions[i].size += regions[i + 1].size;

                memmove(regions + i + 1, regions + i + 2, (region_count - 2) * sizeof(kheap_region_t));

                --region_count;
            }

            if (i > 0 && !regions[i - 1].res)
            {
                regions[i - 1].size += regions[i].size;

                memmove(regions + i, regions + i + 1, (region_count - 1 - i) * sizeof(kheap_region_t));

                --region_count;

                --i;
            }

            if (i < first_free_region)
            {
                first_free_region = i;
                first_free_addr = region_address;
            }

            return;
        }

        region_address += regions[i].size;
    }
}

//==============================================================================
// Interface functions
//==============================================================================

void *kheap_get_current_end()
{
    return (void *)(HEAP_START + heap_size);
}

void kheap_init()
{
    printf("[KHEAP] Initializing heap...\n");

    uint64_t i = PLACEMENT_BEGIN;

    int count = 0;

    for (i; i < PLACEMENT_END; i += PAGE_SIZE)
    {
        void *paddr = phys_mem_alloc_block();

        virt_mem_map_page(paddr, (void *)i, 0);

        printf("Mapping %#016x to %#016x\n", paddr, i);
    }

    regions = (kheap_region_t *)pmalloc(0, 0);

    region_count = 0;
    region_max_count = (PLACEMENT_END - (uint64_t)regions) / sizeof(kheap_region_t);

    printf("[KHEAP] Done!\n");
}

void *kmalloc(size_t size)
{
    return kmalloc_imp(size, 0);
}

void *kmalloc_a(size_t size, uint64_t alignment)
{
    return kmalloc_imp(size, alignment);
}

void kfree(void *addr)
{
    kfree_imp(addr);
}

//==============================================================================
// End of file
//==============================================================================