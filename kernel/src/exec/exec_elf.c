/**
 * @file exec_elf.c
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-07-19
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

#include <exec/elf64.h>
#include <exec/elf32.h>

#include <stdio.h>
#include <vfs/vfs.h>
#include <mm/virt_mem.h>
#include <string.h>
#include <process/process.h>
#include <stdlib.h>

/**
 * @brief Type describing an entry point "main" function.
 * 
 * 
 */
typedef int (*entry_func_t)(int argc, const char **argv);

/**
 * @brief Set this to one to enable output of debug information.
 * 
 * 
 */
#define ELF_DEBUG 1

#if ELF_DEBUG == 1
#define ERROR(...) printf(__VA_ARGS__)
#else
#define ERROR(...)
#endif

/**
 * @brief Make our own makeshift bool implementation.
 * 
 * 
 */
#ifndef __cplusplus
#ifndef bool
#define bool int
#define true (1)
#define false (0)
#endif
#endif

// TODO: Move implementation of elf64 to elf64.c and elf32 to elf32.c

/**
 * @brief Dumps ELF header information somewhat like "readelf -h"
 * 
 * @param header Pointer to header stored in memory.
 * 
 */
static void elf64_debug_dump_ehdr(Elf64_Ehdr_t *header)
{
#if ELF_DEBUG == 1

    // Print the heading

    printf("ELF Header:\n");

    // Print the magic field

    printf("  Magic:");

    for (int i = 0; i < EI_NIDENT; ++i)
    {
        printf(" %02x", header->e_ident[i]);
    }

    printf("\n");

    // Print the class

    printf("  Class: ");

    switch (header->e_ident[EI_CLASS])
    {
    case ELFCLASS64:
        printf("ELF64\n");
        break;
    case ELFCLASS32:
        printf("ELF32\n");
        break;
    default:
        printf("Invalid class\n");
        break;
    }

    // Print the data encoding

    printf("  Data: ");

    switch (header->e_ident[EI_DATA])
    {
    case ELFDATA2MSB:
        printf("ELFDATA2MSB\n");
        break;
    case ELFDATA2LSB:
        printf("ELFDATA2LSB\n");
        break;
    default:
        printf("Invalid data encoding\n");
        break;
    }

    // Print the version

    printf("  Version: %i%s\n",
           header->e_ident[EI_VERSION],
           header->e_ident[EI_VERSION] == EV_CURRENT ? " (current)" : "");

    // Print entry point

    printf("  Entry Point Address: %#0x\n",
           header->e_entry);

    // Print start of program headers

    printf("  Start of program headers: %i (bytes)\n",
           header->e_phoff);

    // Print start of section headers

    printf("  Start of section headers: %i (bytes)\n",
           header->e_shoff);

    // Print flags

    printf("  Flags: %#04x\n",
           header->e_flags);

    // Print size of this header

    printf("  Size of this header: %i (bytes)\n",
           header->e_ehsize);

    // Print size of program headers

    printf("  Size of program headers: %i (bytes)\n",
           header->e_phentsize);

    // Print number of program headers

    printf("  Number of program headers: %i\n",
           header->e_phnum);

    // Print size of section headers

    printf("  Size of section headers: %i (bytes)\n",
           header->e_shentsize);

    // Print number of section headers

    printf("  Number of section headers: %i\n",
           header->e_shnum);

    // Print section header string table index

    printf("  Section header string table index: %i\n",
           header->e_shstrndx);

#else
    (void)header;
#endif
}

/**
 * @brief Check if the elf magic is correct for 64 bit elf.
 * 
 * @param header Pointer to ELF header stored in memory
 * 
 * @return true if correct
 */
static bool elf64_check_file(Elf64_Ehdr_t *header)
{
    if (!header)
    {
        return false;
    }

    if (header->e_ident[EI_MAG0] != ELFMAG0)
    {
        ERROR("ELF Header EI_MAG0 incorrect.\n");
        return false;
    }

    if (header->e_ident[EI_MAG1] != ELFMAG1)
    {
        ERROR("ELF Header EI_MAG1 incorrect.\n");
        return false;
    }

    if (header->e_ident[EI_MAG2] != ELFMAG2)
    {
        ERROR("ELF Header EI_MAG2 incorrect.\n");
        return false;
    }

    if (header->e_ident[EI_MAG3] != ELFMAG3)
    {
        ERROR("ELF Header EI_MAG3 incorrect.\n");
        return false;
    }

    return true;
}

/**
 * @brief Checks if the executable is supported by the OS.
 * 
 * @param header Pointer to elf header stored in memory.
 * 
 * @return true if supported.
 */
static bool elf64_check_supported(Elf64_Ehdr_t *header)
{
    if (!elf64_check_file(header))
    {
        ERROR("Invalid ELF file.\n");
        return false;
    }

    if (header->e_ident[EI_CLASS] != ELFCLASS64)
    {
        ERROR("Unsupported ELF file class.\n");
        return false;
    }

    if (header->e_ident[EI_DATA] != ELFDATA2LSB)
    {
        ERROR("Unsupported ELF file byte order.\n");
        return false;
    }

    if (header->e_machine != EM_AMD64)
    {
        ERROR("Unsupported ELF file target: %i.\n",
              header->e_machine);
        return false;
    }

    if (header->e_ident[EI_VERSION] != EV_CURRENT)
    {
        ERROR("Unsupported ELF file version.\n");
        return false;
    }

    if (header->e_type != ET_REL && header->e_type != ET_EXEC)
    {
        ERROR("Unsupported ELF file type.\n");
        return false;
    }

    return true;
}

/**
 * @brief Get the address of the section header array.
 * 
 * @param header Pointer to ELF header.
 * 
 * @return Pointer to first section header.
 */
static Elf64_Shdr_t *elf64_sheader(Elf64_Ehdr_t *header)
{
    return (Elf64_Shdr_t *)((int)header + header->e_shoff);
}

/**
 * @brief Gets the address of a specific section header.
 * 
 * @param header Pointer to ELF header.
 * @param index Index of header to get.
 * 
 * @return Pointer to section header at index @index
 */
static Elf64_Shdr_t *elf64_section(Elf64_Ehdr_t *header, int index)
{
    Elf64_Shdr_t *sheader = elf64_sheader(header);

    return &sheader[index];
}

/**
 * @brief Get the address of the string table
 * 
 * @param header Pointer to ELF header.
 * 
 * @return Pointer to the beginning of the string table.
 */
static char *elf64_str_table(Elf64_Ehdr_t *header)
{
    if (header->e_shstrndx == SHN_UNDEF)
    {
        return NULL;
    }

    return (char *)header + elf64_section(header, header->e_shstrndx)->sh_offset;
}

/**
 * @brief Fetch a string from the string table.
 * 
 * @param header Pointer to ELF header.
 * @param offset Offset of the string in the table.
 * 
 * @return String from string table.
 */
static char *elf64_lookup_string(Elf64_Ehdr_t *header, int offset)
{
    char *strtab = elf64_str_table(header);

    if (!strtab)
    {
        return NULL;
    }

    return strtab + offset;
}

/**
 * @brief Value to return on relocation error.
 * 
 * 
 */
#define ELF_RELOC_ERR -1

/**
 * @brief Lookup a symbol in the executable
 * 
 * @param name Name of the symbol
 * 
 * @return Pointer to the symbol
 */
static void *elf64_lookup_symbol(const char *name)
{
    // TODO: Implement
    return NULL;
}

/**
 * @brief Get the absolute address of the value of a symbol.
 * 
 * @param header Pointer to an ELF header.
 * @param table Symbol table index.
 * @param index Symbol index.
 * 
 * @return Address of value of symbol or @ELF_RELOC_ERR on error
 */
static int elf64_get_symval(Elf64_Ehdr_t *header,
                            int table,
                            uint32_t index)
{
    if (table == SHN_UNDEF || index == SHN_UNDEF)
    {
        return 0;
    }

    Elf64_Shdr_t *symtab = elf64_section(header, table);

    uint32_t symtab_entries = symtab->sh_size / symtab->sh_entsize;

    if (index >= symtab_entries)
    {
        ERROR("Symbol index out of range (%d:%d).\n", table, index);
        return ELF_RELOC_ERR;
    }

    int symaddr = (int)header + symtab->sh_offset;
    Elf64_Sym_t *symbol = &((Elf64_Sym_t *)symaddr)[index];

    if (symbol->st_shndx == SHN_UNDEF)
    {
        Elf64_Shdr_t *strtab = elf64_section(header, symtab->sh_link);

        const char *name = (const char *)header + strtab->sh_offset + symbol->st_name;

        void *target = elf64_lookup_symbol(name);

        if (target == NULL)
        {
            if (ELF64_ST_BIND(symbol->st_info) & STB_WEAK)
            {
                return 0;
            }
            else
            {
                ERROR("Undefined external symbol: %s\n", name);
                return ELF_RELOC_ERR;
            }
        }
        else
        {
            return (int)target;
        }
    }
    else if (symbol->st_shndx == SHN_ABS)
    {
        return symbol->st_value;
    }
    else
    {
        Elf64_Shdr_t *target = elf64_section(header, symbol->st_shndx);

        return (int)header + symbol->st_value + target->sh_offset;
    }
}

static int elf64_load_stage1(Elf64_Ehdr_t *header)
{
    Elf64_Shdr_t *shdr = elf64_sheader(header);

    for (unsigned int i = 0; i < header->e_shnum; ++i)
    {
        Elf64_Shdr_t *section = &shdr[i];
    }
}

/**
 * @brief Check if the elf magic is correct for 32 bit elf.
 * 
 * @param header Pointer to ELF header stored in memory
 * 
 * @return zero if error
 */
static int elf32_check_file(Elf32_Ehdr_t *header)
{
    if (!header)
    {
        return 0;
    }

    if (header->e_ident[EI_MAG0] != ELFMAG0)
    {
        ERROR("ELF Header EI_MAG0 incorrect.\n");
        return 0;
    }

    if (header->e_ident[EI_MAG1] != ELFMAG1)
    {
        ERROR("ELF Header EI_MAG1 incorrect.\n");
        return 0;
    }

    if (header->e_ident[EI_MAG2] != ELFMAG2)
    {
        ERROR("ELF Header EI_MAG2 incorrect.\n");
        return 0;
    }

    if (header->e_ident[EI_MAG3] != ELFMAG3)
    {
        ERROR("ELF Header EI_MAG3 incorrect.\n");
        return 0;
    }

    return 1;
}

/**
 * @brief Checks if the executable is supported by the OS.
 * 
 * @param header Pointer to elf header stored in memory.
 * 
 * @return true if supported.
 */
static bool elf32_check_supported(Elf32_Ehdr_t *header)
{
    if (!elf32_check_file(header))
    {
        ERROR("Invalid ELF file.\n");
        return false;
    }

    if (header->e_ident[EI_CLASS] != ELFCLASS32)
    {
        ERROR("Unsupported ELF file class.\n");
        return false;
    }

    if (header->e_ident[EI_DATA] != ELFDATA2LSB)
    {
        ERROR("Unsupported ELF file byte order.\n");
        return false;
    }

    if (header->e_machine != EM_386)
    {
        ERROR("Unsupported ELF file target: %i.\n",
              header->e_machine);
        return false;
    }

    if (header->e_ident[EI_VERSION] != EV_CURRENT)
    {
        ERROR("Unsupported ELF file version.\n");
        return false;
    }

    if (header->e_type != ET_REL && header->e_type != ET_EXEC)
    {
        ERROR("Unsupported ELF file type.\n");
        return false;
    }

    return true;
}

// TODO: handle 32 bit ELF files too.
int exec_elf(char *path, int argc, char **argv, char **env, int depth)
{
    process_t *current_process = process_get_current();

    Elf64_Ehdr_t header;

    if (path != NULL)
    {
        printf("Opening %s\n", path);
    }

    fs_node_t *file = kopen(path, 0);

    if (!file)
    {
        printf("Could not open file: %s\n", path);
        return -1;
    }

    read_fs(file, 0, sizeof(Elf64_Ehdr_t), (uint8_t *)&header);

    elf64_debug_dump_ehdr(&header);

    if (!elf64_check_supported(&header))
    {
        printf("\"%s\" is not a valid ELF executable\n", path);
        close_fs(file);
        return -1;
    }

    for (uintptr_t x = 0;
         x < (uint64_t)header.e_shentsize * header.e_shnum;
         x += header.e_shentsize)
    {
        Elf64_Shdr_t shdr;

        read_fs(file,
                header.e_shoff + x,
                sizeof(Elf64_Shdr_t),
                (uint8_t *)&shdr);

        if (shdr.sh_type == SHT_NOBITS)
        {
            if (!shdr.sh_size)
            {
                continue;
            }

            if (shdr.sh_flags & SHF_ALLOC)
            {
                void *mem = malloc(shdr.sh_size);

                if (!mem)
                {
                    ERROR("Could not allocate memory for section. Requested %i bytes\n",
                          shdr.sh_size);
                }

                memset(mem, 0, shdr.sh_size);

                //shdr.sh_size = (int)mem - (int)&header;

                printf("Allocated memory for section: %i bytes\n", shdr.sh_size);
            }
        }
    }

    // TODO: Check permissions

    for (uintptr_t x = 0;
         x < (uint64_t)header.e_phentsize * header.e_phnum;
         x += header.e_phentsize)
    {
        Elf64_Phdr_t phdr;

        read_fs(file,
                header.e_phoff + x,
                sizeof(Elf64_Phdr_t),
                (uint8_t *)&phdr);

        if (phdr.p_type == PT_DYNAMIC)
        {
            close_fs(file);

            printf("Dynamic executable");

            // TODO: Implement with linker
            return -1;
        }
    }

    uintptr_t entry = (uintptr_t)header.e_entry;
    uintptr_t base_address = UINTPTR_MAX;
    uintptr_t end_address = 0;

    for (uintptr_t x = 0;
         x < (uint64_t)header.e_phentsize * header.e_phnum;
         x += header.e_phentsize)
    {
        Elf64_Phdr_t phdr;

        //printf("Found program header: %i\n", phdr.p_type);

        read_fs(file, header.e_phoff + x, sizeof(Elf64_Phdr_t), (uint8_t *)&phdr);

        if (phdr.p_type == PT_LOAD)
        {
            if (phdr.p_vaddr < base_address)
            {
                base_address = phdr.p_vaddr;
            }

            if (phdr.p_memsz + phdr.p_vaddr > end_address)
            {
                end_address = phdr.p_memsz + phdr.p_vaddr;
            }
        }
    }

    current_process->image.entry = base_address;
    current_process->image.size = end_address - base_address;

    for (uintptr_t x = 0;
         x < (uint64_t)header.e_phentsize * header.e_phnum;
         x += header.e_phentsize)
    {
        Elf64_Phdr_t phdr;

        //printf("Found program header: %i\n", phdr.p_type);

        read_fs(file, header.e_phoff + x, sizeof(Elf64_Phdr_t), (uint8_t *)&phdr);

        if (phdr.p_type == PT_LOAD)
        {
            if (phdr.p_vaddr < 0x400000 || phdr.p_vaddr >= 0x20000000)
            {
                printf("Invalid load address %#016x\n", phdr.p_vaddr);
                close_fs(file);
                return -1;
            }

            for (uintptr_t i = phdr.p_vaddr; i < phdr.p_vaddr + phdr.p_memsz; i += 0x1000)
            {
                // TODO: Check page alignment

                void *paddr = phys_mem_alloc_block();

                virt_mem_map_page(paddr, (void *)i, VIRT_MEM_WRITABLE | VIRT_MEM_USER);
            }

            read_fs(file, phdr.p_offset, phdr.p_filesz, (uint8_t *)phdr.p_vaddr);

            memset((void *)(phdr.p_vaddr + phdr.p_filesz), 0, (size_t)(phdr.p_memsz - phdr.p_filesz));
        }
    }

    close_fs(file);

    int envc = 0;

    if (env)
    {
        for (envc = 0; env[envc] != NULL; ++envc)
            ;
    }

    uintptr_t heap = current_process->image.entry + current_process->image.size;

    // TODO: Replace with align_up function
    while (heap & 0xFFF)
    {
        ++heap;
    }

    void *heap_phys = phys_mem_alloc_block();
    virt_mem_map_page(heap_phys, (void *)heap, VIRT_MEM_WRITABLE | VIRT_MEM_USER);

    // Allocate room on heap for argv
    char **argv_ = (char **)heap;
    heap += sizeof(char *) * (argc + 1);

    // Allocate room on heap for env
    char **env_ = (char **)heap;
    heap += sizeof(char *) * (envc + 1);

    for (int i = 0; i < argc; ++i)
    {
        size_t size = strlen(argv[i]) * sizeof(char) + 1;

        for (uintptr_t x = heap; x < heap + size + PAGE_SIZE; x += PAGE_SIZE)
        {
            void *mem = phys_mem_alloc_block();

            virt_mem_map_page(mem, (void *)x, VIRT_MEM_WRITABLE | VIRT_MEM_USER);
        }

        argv_[i] = (char *)heap;
        memcpy((void *)heap, argv[i], size);
        heap += size;
    }

    argv_[argc] = 0;

    for (int i = 0; i < envc; ++i)
    {
        size_t size = strlen(env[i]) * sizeof(char) + 1;

        for (uintptr_t x = heap; x < heap + size + PAGE_SIZE; x += PAGE_SIZE)
        {
            void *mem = phys_mem_alloc_block();

            virt_mem_map_page(mem, (void *)x, VIRT_MEM_WRITABLE | VIRT_MEM_USER);
        }

        env_[i] = (char *)heap;
        memcpy((void *)heap, env[i], size);
        heap += size;
    }

    env_[envc] = 0;

    current_process->image.heap = heap;
    current_process->image.heap_actual = heap + (PAGE_SIZE - heap % PAGE_SIZE);

    void *mem = phys_mem_alloc_block();

    virt_mem_map_page(mem, (void *)current_process->image.heap_actual, VIRT_MEM_WRITABLE | VIRT_MEM_USER);

    current_process->image.start = entry;

    for (unsigned int i = 3; i < current_process->file_descriptors->length; ++i)
    {
        if (current_process->file_descriptors->entries[i])
        {
            close_fs(current_process->file_descriptors->entries[i]);
            current_process->file_descriptors->entries[i] = NULL;
        }
    }

    entry_func_t entry_func = (entry_func_t)entry;

    //const char *args[2] = {"Test", "Fisk"};

    int ret = entry_func(argc, (const char **)argv_);

    __builtin_unreachable();
}

//=============================================================================
// End of file
//=============================================================================