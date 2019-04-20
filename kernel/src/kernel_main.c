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
#include <drivers/vbe.h>
#include <drivers/keyboard_ps2.h>
#include <drivers/mouse_ps2.h>

#include <vfs/vfs.h>
#include <vfs/ext2.h>

#include <cmos/cmos_rtc.h>

#include <util/hexdump.h>

#include <exec/elf64.h>

#include <debug/backtrace.h>

#include <pci/pci.h>

#include <ctype.h>

#include <gui/gui.h>

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
        case MULTIBOOT_TAG_TYPE_ELF_SECTIONS:
        {
            struct multiboot_tag_elf_sections *elf_sections = (struct multiboot_tag_elf_sections *)tag;

            init_kernel_symbol_context(elf_sections, mem_info);
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
    //mem_info->kernel_end = (uint64_t)&__kernel_end;
    mem_info->kernel_size = mem_info->kernel_end - mem_info->kernel_load_addr;
}

void mouse_moved_handler(mouse_moved_event_t *event)
{
    static int i = 0;

    printf("Mouse moved [%i]: %i, %i\n", i++, (int64_t)event->x, (int64_t)event->y);
}

int kernel_main(unsigned long long rbx, unsigned long long rax)
{
    (void)rax;

    serial_init_full();

    debug_terminal_initialize();

    set_stdout(scom1_fd);

    printf("Initializing kernel...\n");

    run_unit_tests();

    arch_initialize();

    RTC_init();

    memory_info_t mem_info;

    memset(&mem_info, 0, sizeof(mem_info));

    parse_multiboot((unsigned char *)rbx, &mem_info);

    printf("Kernel end: %#016x\n", mem_info.kernel_end);

    phys_mem_init(&mem_info);

    virt_mem_initialize();

    sti();

    keyboard_install();
    mouse_install();

    kheap_init();

    vfs_install();

    init_ide_devices();

    ext2_initialize();

    pciInit();

    vbe_bochs_set_gfx(1024, 768, 4);

    printf("Kernel initailization done!\n");

    printf("================================================================================\n");
    printf("|| Launching GUI...                                                           ||\n");
    printf("================================================================================\n");

    gui_init();

#if 0

    int ret = mkdir_fs("newdir", 0);

    printf("mkdir_fs returned %i\n", ret);

    ret = create_file_fs("/newdir/testfile3", 0);

    printf("create_file_fs returned %i\n", ret);

    const char *pathw = "/newdir/testfile3";

    fs_node_t *nodew = kopen(pathw, 0);

    if (!nodew)
    {
        printf("Could not open %s", pathw);

        for (;;)
            ;
    }

    if (!(nodew->flags & FS_FILE))
    {
        printf("Not a file\n");

        for (;;)
            ;
    }

    const char *string = "This the new string that should be stored in the file";

    int bytes_written;

    // Discard current content of file
    //truncate_fs(nodew);

    // Write new content.
    bytes_written = write_fs(nodew, 0, strlen(string), string);

    if (bytes_written != strlen(string))
    {
        printf("Error writing to file\n");
    }

#endif

#if 0

    const char *path = "/newdir/testfile3";

    fs_node_t *node = kopen(path, 0);

    if (!node)
    {
        printf("Could not open %s", path);

        for (;;)
            ;
    }

    if (!(node->flags & FS_FILE))
    {
        printf("Not a file\n");

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

    hexdump(file, size);

#endif

#if 0

    const char *pathd = "/";

    fs_node_t *noded = kopen(pathd, 0);

    if (!noded)
    {
        printf("Could not open %s", pathd);

        for (;;)
            ;
    }

    printf("Contents of %s\n", pathd);

    if (!noded->flags & FS_DIRECTORY)
    {
        printf("Not a directory!\n");

        for (;;)
            ;
    }

    int index = 0;
    struct dirent *d;

    while ((d = readdir_fs(noded, index++)) != NULL)
    {
        printf("%s\n", d->name);
    }

#endif

    for (;;)
        __asm__ volatile("hlt");
}