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

#include <util/hexdump.h>

extern void run_unit_tests();

// https://www.gnu.org/software/grub/manual/multiboot2/html_node/kernel_002ec.html

extern void *__kernel_start;
extern void *__kernel_end;

typedef uint64_t Elf64_Addr;
typedef uint64_t Elf64_Off;
typedef uint16_t Elf64_Half;
typedef uint32_t Elf64_Word;
typedef int32_t Elf64_Sword;
typedef uint64_t Elf64_Xword;
typedef int64_t Elf64_Sxword;

typedef struct
{
    Elf64_Word sh_name;
    Elf64_Word sh_type;
    Elf64_Xword sh_flags;
    Elf64_Addr sh_addr;
    Elf64_Off sh_offset;
    Elf64_Xword sh_size;
    Elf64_Word sh_link;
    Elf64_Word sh_info;
    Elf64_Xword sh_addralign;
    Elf64_Xword sh_entsize;
} __attribute__((packed)) Elf64_Shdr;

typedef struct
{
    Elf64_Word st_name;
    unsigned char st_info;
    unsigned char st_other;
    Elf64_Half st_shndx;
    Elf64_Addr st_value;
    Elf64_Xword st_size;
} __attribute__((packed)) Elf64_Sym;

#define STT_FUNC 2

static Elf64_Sym *_symbol_section = NULL;
static int _symbol_count = 0;
static const char *_symbol_string_table = NULL;

const char *lookup_symbol(void *addr, int64_t *offset)
{
    if (!_symbol_section || !_symbol_string_table)
    {
        return NULL;
    }

    Elf64_Sym *best_match = NULL;

    // These could be placed in a map for log(N) access. This will do for now.
    for (int i = 0; i < _symbol_count; ++i)
    {
        // Only handle function symbols
        if ((_symbol_section[i].st_info & 0x0F) == STT_FUNC)
        {
            if (_symbol_section[i].st_value <= addr && (!best_match || best_match->st_value < _symbol_section[i].st_value))
            {
                best_match = &_symbol_section[i];
            }
        }
    }

    if (!best_match)
    {
        return NULL;
    }

    if (offset)
    {
        *offset = (Elf64_Addr)addr - best_match->st_value;
    }

    return _symbol_string_table + best_match->st_name;
}

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

            printf("Section count: %i\n", elf_sections->num);
            printf("Section entry size: %i\n", elf_sections->entsize);
            printf("Section shndx: %i\n", elf_sections->shndx);

            Elf64_Shdr *sec_header = (Elf64_Shdr *)elf_sections->sections;

            Elf64_Shdr *string_header = ((Elf64_Shdr *)elf_sections->sections) + elf_sections->shndx;

            const char *string_table = string_header->sh_addr;

            for (int i = 0; i < elf_sections->num; ++i)
            {

                printf("Section %i:\n", i);
                printf("Name: %i (%s)\n", sec_header[i].sh_name, string_table + sec_header[i].sh_name);
                printf("Type: %i\n", sec_header[i].sh_type);
                printf("Addr: %#016x\n", sec_header[i].sh_addr);
                printf("Size: %i\n", sec_header[i].sh_size);

                if (strcmp(string_table + sec_header[i].sh_name, ".strtab") == 0)
                {
                    _symbol_string_table = sec_header[i].sh_addr;
                    printf("Found .strtab at: %#016x\n", _symbol_string_table);
                }

                if (strcmp(string_table + sec_header[i].sh_name, ".symtab") == 0)
                {
                    _symbol_section = sec_header[i].sh_addr;
                    _symbol_count = sec_header[i].sh_size / sizeof(Elf64_Sym);
                    printf("Found .symtab at: %#016x. (%i entries)\n", _symbol_section, _symbol_count);
                }
            }
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

void print_backtrace(unsigned long long int *reg)
{
    while (*reg)
    {
        int64_t offset;
        const char *name = lookup_symbol(*(reg + 1), &offset);

        printf("Called from <%s+%#x>\n", name, offset);

        reg = *reg;
    }
}

void backtrace()
{
    register unsigned long long int *_rbp __asm__("rbp");

    print_backtrace(_rbp);
}

void func1()
{
    backtrace();
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

    func1();

    //printf("[MEMINFO] Memory size: %i\n", mem_info.memory_size);

    phys_mem_init(&mem_info);

    virt_mem_initialize();

    sti();

    kheap_init();

    vfs_install();

    init_ide_devices();

    ext2_initialize();

    const char *path = "/testfile2";

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

    hexdump(file, size);

    for (;;)
        __asm__ volatile("hlt");
}