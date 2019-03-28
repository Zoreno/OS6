#include <debug/debug_terminal.h>

#include <boot/multiboot2.h>

#include <arch/arch.h>

#include <string.h>
#include <stdio.h>

extern void run_unit_tests();

// https://www.gnu.org/software/grub/manual/multiboot2/html_node/kernel_002ec.html

void initialize(unsigned char *mb_ptr)
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

            for (int i = 0; i < count; ++i)
            {
                struct multiboot_mmap_entry mmap_entry = mmap->entries[i];

                uint32_t addr_upper = mmap_entry.addr >> 32;
                uint32_t addr_lower = mmap_entry.addr & 0xFFFFFFFF;

                uint32_t len_upper = mmap_entry.len >> 32;
                uint32_t len_lower = mmap_entry.len & 0xFFFFFFFF;

                printf("[mmap_entry] addr: %#08x%08x len: %#08x%08x type: %i\n",
                       addr_upper, addr_lower, len_upper, len_lower, mmap_entry.type);
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

    initialize((unsigned char *)rbx);

    for (;;)
        ;
}
