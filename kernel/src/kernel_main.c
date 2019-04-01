#include <debug/debug_terminal.h>

#include <boot/multiboot2.h>

#include <arch/arch.h>

#include <mm/phys_mem.h>
#include <mm/memory_info.h>
#include <mm/virt_mem.h>
#include <mm/kheap.h>

#include <string.h>
#include <stdio.h>

extern void run_unit_tests();

// https://www.gnu.org/software/grub/manual/multiboot2/html_node/kernel_002ec.html

extern void *__kernel_start;
extern void *__kernel_end;

// TODO: Move to separate file
void parse_multiboot(unsigned char *mb_ptr, memory_info_t *mem_info)
{

    struct multiboot_start_tag *start_tag = (struct multiboot_start_tag *)mb_ptr;

    uint32_t mb_size = start_tag->total_size;

    printf("Multiboot total size: %i\n", mb_size);

    int end_found = 0;

    mb_ptr += sizeof(struct multiboot_start_tag);

    while (!end_found)
    {
        struct multiboot_tag *tag = (struct multiboot_tag *)mb_ptr;

        switch (tag->type)
        {
        case MULTIBOOT_TAG_TYPE_CMDLINE:
        {
            struct multiboot_tag_string *cmdline = (struct multiboot_tag_string *)tag;
            printf("[Command line] \"%s\"\n", cmdline->string);
        }
        break;

        case MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME:
        {
            struct multiboot_tag_string *boot_loader = (struct multiboot_tag_string *)tag;
            printf("[Boot loader name] \"%s\"\n", boot_loader->string);
        }
        break;
        case MULTIBOOT_TAG_TYPE_BASIC_MEMINFO:
        {
            struct multiboot_tag_basic_meminfo *basic_mem =
                (struct multiboot_tag_basic_meminfo *)tag;
            printf("[Basic memory] lower: %i upper: %i\n", basic_mem->mem_lower, basic_mem->mem_upper);
        }
        break;
        case MULTIBOOT_TAG_TYPE_BOOTDEV:
        {
            struct multiboot_tag_bootdev *bootdev = (struct multiboot_tag_bootdev *)tag;

            printf("[Bootdev] biosdev: %i, partition: %i, sub_partition: %i\n",
                   bootdev->biosdev, bootdev->slice, bootdev->part);
        }
        break;
        case MULTIBOOT_TAG_TYPE_MMAP:
        {
            struct multiboot_tag_mmap *mmap = (struct multiboot_tag_mmap *)tag;

            int count = mmap->size / mmap->entry_size;

            printf("[mmap] entry_size: %i, count: %i\n", mmap->entry_size, count);

            int j = 0;

            for (int i = 0; i < count; ++i)
            {
                struct multiboot_mmap_entry mmap_entry = mmap->entries[i];

                printf("[mmap_entry] addr: %#016x len: %#016x type: %i\n",
                       mmap_entry.addr, mmap_entry.len, mmap_entry.type);

                if (mmap_entry.type == MULTIBOOT_MEMORY_AVAILABLE)
                {
                    mem_info->regions[j].addr = mmap_entry.addr;
                    mem_info->regions[j].len = mmap_entry.len;

                    ++j;
                }
            }
        }
        break;
        case MULTIBOOT_TAG_TYPE_LOAD_BASE_ADDR:
        {
            struct multiboot_tag_load_base_addr *lba = (struct multiboot_tag_load_base_addr *)tag;

            printf("[Load Base Address] addr: %#08x\n", lba->load_base_addr);
        }
        break;
        case MULTIBOOT_TAG_TYPE_END:
        {
            end_found = 1;
        }
        break;
        default:
        {
            printf("[Tag] type: %i size: %i\n", tag->type, tag->size);
        }
        break;
        }

        if (tag->type == MULTIBOOT_TAG_TYPE_END)
        {
            end_found = 1;
        }

        mb_ptr += (tag->size + 7) & ~7;
    }

    uint64_t largest_mem = 0;

    for (int i = 0; i < MEM_INFO_MAX_REGIONS; ++i)
    {
        mem_region_t region = mem_info->regions[i];

        uint64_t region_end = region.addr + region.len;

        if (region_end > largest_mem)
        {
            largest_mem = region_end;
        }
    }

    mem_info->memory_size = largest_mem;

    mem_info->kernel_load_addr = (uint64_t)&__kernel_start;
    mem_info->kernel_end = (uint64_t)&__kernel_end;
    mem_info->kernel_size = mem_info->kernel_end - mem_info->kernel_load_addr;
}

int kernel_main(unsigned long long rbx, unsigned long long rax)
{
    (void)rax;

    debug_terminal_initialize();

    printf("================================================================================");
    printf("|| Welcome to the OS6 operating system.                                       ||");
    printf("|| The kernel is now running in 64 bit mode.                                  ||");
    printf("================================================================================");

    run_unit_tests();

    arch_initialize();

    memory_info_t mem_info;

    memset(&mem_info, 0, sizeof(mem_info));

    parse_multiboot((unsigned char *)rbx, &mem_info);

    //printf("[MEMINFO] Memory size: %i\n", mem_info.memory_size);

    phys_mem_init(&mem_info);

    virt_mem_initialize();

    kheap_init();

    for (;;)
        __asm__ volatile("hlt");
}

/*

Buddy Allocation

Order 4 upper limit

2^0 = 1
2^1 = 2
2^2 = 4
2^3 = 8
2^4 = 16
2^5 = 32
2^6 = 64
2^7 = 128
2^8 = 256
2^9 = 512

const int highest_order = 9;

typedef struct
{
    uint64_t start;
    uint64_t bitmap[2 * highest_order / 64]; // 988777766666666
} block_t;

typedef struct
{

} page_frame_allocator_t;

int is_set(uint64_t *bitmap, int bit)
{
    ...
}

int set(uint64_t *bitmap, int bit);
int clear(uint64_t *bitmap, int bit);

int _find_page(uint64_t *bitmap, int i, int order, int curr_order)
{
    if(is_set(bitmap, i))
    {
        return -1;
    }

    if(curr_order == order)
    {
        return i;
    }

    int res;

    if((res = _find_page(bitmap, 2 * i + 1, order, curr_order - 1)) != -1)
    {
        return res;
    }

    return _find_page(bitmap, 2 * i  + 2, order, curr_order - 1);
}

void *_alloc_page(block_t *block, size_t size)
{
    int order = _get_order(size);

    int page = _find_page(block->bitmap, 0, order, highest_order);

    if(page != -1)
    {
        void *addr = _get_address(block, i);

        return addr;
    }

    return NULL;
}

Request A for 1 block (Order 0)

Memory before:
4
Step 1: Split smallest until we have a order 0
3-3
2-2-3
1-1-2-3
0-0-1-2-3
Step 2: Allocate order 0 block
0(A)-0-1-2-3

Request B for 1 block

0(A)-0(B)-1-2-3

Request C for 2 blocks (order 1)

0(A)-0(B)-1(C)-2-3

Free A

0-0(B)-1(C)-2-3

Free B

0-0-1(C)-2-3
1-1(C)-2-3

Free C

1-1-2-3
2-2-3
3-3
4


*/