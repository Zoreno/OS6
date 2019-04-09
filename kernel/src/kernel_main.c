#include <debug/debug_terminal.h>

#include <boot/multiboot2.h>

#include <arch/arch.h>

#include <mm/phys_mem.h>
#include <mm/memory_info.h>
#include <mm/virt_mem.h>
#include <mm/kheap.h>

#include <serial/serial.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <util/list.h>
#include <util/tree.h>

#include <drivers/ide.h>
#include <drivers/blockdev.h>

#include <vfs/vfs.h>
#include <vfs/ext2.h>

#include <cmos/cmos_rtc.h>

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

    serial_init_full();

    debug_terminal_initialize();

    set_stdout(scom1_fd);

    printf("================================================================================");
    printf("|| Welcome to the OS6 operating system.                                       ||");
    printf("|| The kernel is now running in 64 bit mode.                                  ||");
    printf("================================================================================");

    run_unit_tests();

    arch_initialize();

    RTC_init();

    memory_info_t mem_info;

    memset(&mem_info, 0, sizeof(mem_info));

    parse_multiboot((unsigned char *)rbx, &mem_info);

    //printf("[MEMINFO] Memory size: %i\n", mem_info.memory_size);

    phys_mem_init(&mem_info);

    virt_mem_initialize();

    sti();

    kheap_init();

    vfs_install();

    init_ide_devices();

    ext2_initialize();

    const char *path = "/home/testfile";

    fs_node_t *node = kopen(path, 0);

    if (!node)
    {
        printf("Could not open %s", path);

        for (;;)
            ;
    }

    printf("%s opened!\n", path);
    printf("Size: %i\n", node->length);

    uint32_t size = node->length;

    uint8_t *file = malloc(size);
    int bytes_read;

    bytes_read = read_fs(node, 0, size, file);

    if (bytes_read != size)
    {
        printf("Error reading from file\n");
    }

    uint32_t bytes = size;
    uint32_t bytes_per_col = 16;

    int nrows = (bytes + bytes_per_col - 1) / bytes_per_col;

    int width = 0;

    while (bytes)
    {
        bytes /= 16;
        width++;
    }

    for (uint32_t row = 0; row < nrows; ++row)
    {
        printf("%#0*x | ", width, row * bytes_per_col);

        for (uint32_t col = 0; col < 16; ++col)
        {
            uint32_t offset = row * bytes_per_col + col;

            if (offset >= size)
            {
                printf("-- ");
            }
            else
            {
                printf("%02x ", file[offset]);
            }
        }

        printf("|");

        for (uint32_t col = 0; col < 16; ++col)
        {
            uint32_t offset = row * bytes_per_col + col;

            if (offset >= size)
            {
                printf(" ");
            }
            else
            {
                char c = file[offset] & 0x7F;

                if (c < 32 || c >= 127)
                {
                    c = '.';
                }

                char s[2] = {0};
                s[0] = c;

                printf("%s", s);
            }
        }

        printf("|");

        printf("\n");
    }

    for (;;)
        __asm__ volatile("hlt");
}