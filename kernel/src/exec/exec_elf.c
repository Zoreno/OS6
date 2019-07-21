/**
 * @file exec_elf.c
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-07-19
 * 
 * @brief Hashmap
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

#include <stdio.h>
#include <vfs/vfs.h>
#include <mm/virt_mem.h>
#include <string.h>

typedef int (*entry_func_t)(int argc, const char **argv);

int exec_elf(char *path, int argc, char **argv, char **env, int depth)
{
    Elf64_Ehdr_t header;

    printf("Opening %s\n", path);

    fs_node_t *file = kopen(path, 0);

    if (!file)
    {
        printf("Could not open file: %s\n", path);
        return -1;
    }

    read_fs(file, 0, sizeof(Elf64_Ehdr_t), (uint8_t *)&header);

    if (header.e_ident[0] != ELFMAG0 ||
        header.e_ident[1] != ELFMAG1 ||
        header.e_ident[2] != ELFMAG2 ||
        header.e_ident[3] != ELFMAG3)
    {
        printf("\"%s\" is not a valid ELF executable\n", path);
        close_fs(file);
        return -1;
    }

    // TODO: Check permissions

    for (uintptr_t x = 0;
         x < (uint64_t)header.e_phentsize * header.e_phnum;
         x += header.e_phentsize)
    {
        Elf64_Phdr_t phdr;

        read_fs(file, header.e_phoff + x, sizeof(Elf64_Phdr_t), (uint8_t *)&phdr);

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

    //printf("Entry: %#016x\n", entry);
    //printf("Base address: %#016x\n", base_address);
    //printf("Size: %#016x\n", end_address - base_address);

    for (uintptr_t x = 0;
         x < (uint64_t)header.e_phentsize * header.e_phnum;
         x += header.e_phentsize)
    {
        Elf64_Phdr_t phdr;

        //printf("Found program header: %i\n", phdr.p_type);

        read_fs(file, header.e_phoff + x, sizeof(Elf64_Phdr_t), (uint8_t *)&phdr);

        if (phdr.p_type == PT_LOAD)
        {
            if (phdr.p_vaddr < 0x400000 && phdr.p_vaddr >= 0x20000000)
            {
                printf("Invalid load address %#016x\n", phdr.p_vaddr);
                close_fs(file);
                return -1;
            }

            for (uintptr_t i = phdr.p_vaddr; i < phdr.p_vaddr + phdr.p_memsz; i += 0x1000)
            {
                // TODO: Check page alignment

                void *paddr = phys_mem_alloc_block();

                virt_mem_map_page(paddr, (void *)i, 0);
            }

            read_fs(file, phdr.p_offset, phdr.p_filesz, (uint8_t *)phdr.p_vaddr);

            memset((void *)(phdr.p_vaddr + phdr.p_filesz), 0, (size_t)(phdr.p_memsz - phdr.p_filesz));
        }
    }

    close_fs(file);

    entry_func_t entry_func = (entry_func_t)entry;

    const char *args[2] = {"Test", "Fisk"};

    int ret = entry_func(2, args);

    printf("Done executing %s. Return code: %i\n", path, ret);

    return -1;
}