/**
 * @file elf64.h
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-04-16
 * 
 * @brief ELF 64-bit binary format types and definitons
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

#ifndef _ELF64_H
#define _ELF64_H

#include <boot/multiboot2.h>
#include <mm/memory_info.h>
#include <stdint.h>

//=============================================================================
// Primitive types
//=============================================================================

typedef uint64_t Elf64_Addr_t;
typedef uint64_t Elf64_Off_t;
typedef uint16_t Elf64_Half_t;
typedef uint32_t Elf64_Word_t;
typedef int32_t Elf64_Sword_t;
typedef uint64_t Elf64_Xword_t;
typedef int64_t Elf64_Sxword_t;

//=============================================================================
// Program header
//=============================================================================

typedef struct
{
    unsigned char e_ident[16];
    Elf64_Half_t e_type;
    Elf64_Half_t e_machine;
    Elf64_Word_t e_version;
    Elf64_Addr_t e_entry;
    Elf64_Off_t e_phoff;
    Elf64_Off_t e_shoff;
    Elf64_Word_t e_flags;
    Elf64_Half_t e_ehsize;
    Elf64_Half_t e_phentsize;
    Elf64_Half_t e_phnum;
    Elf64_Half_t e_shentsize;
    Elf64_Half_t e_shnum;
    Elf64_Half_t e_shstrndx;

} __attribute__((packed)) Elf64_Ehdr_t;

#define ELFMAG0 0x7f
#define ELFMAG1 'E'
#define ELFMAG2 'L'
#define ELFMAG3 'F'
#define EI_NIDENT 16

#define EI_MAG0 0
#define EI_MAG1 1
#define EI_MAG2 2
#define EI_MAG3 3
#define EI_CLASS 4
#define EI_DATA 5
#define EI_VERSION 6
#define EI_OSABI 7
#define EI_AVIVERSION 8
#define EI_PAD 9
#define EI_NIDENT 16

#define ELFCLASS32 1
#define ELFCLASS64 2

#define ELFDATA2LSB 1
#define ELFDATA2MSB 2

#define ELFOSABI_SYSV 0
#define ELFOSABI_HPUX 1
#define ELFOSABI_STANDALONE 255

#define ET_NONE 0
#define ET_REL 1
#define ET_EXEC 2
#define ET_DYN 3
#define ET_CORE 4
#define ET_LOOS 0xFE00
#define ET_HIOS 0xFEFF
#define ET_LOPROC 0xFF00
#define ET_HIPROC 0xFFFF

#define EM_NONE 0
#define EM_SPARC 2
#define EM_386 3
#define EM_SPARC32PLUS 18
#define EM_SPARCV9 43
#define EM_AMD64 62

#define EV_CURRENT 1

//=============================================================================
// Section header
//=============================================================================

typedef struct
{
    Elf64_Word_t sh_name;
    Elf64_Word_t sh_type;
    Elf64_Xword_t sh_flags;
    Elf64_Addr_t sh_addr;
    Elf64_Off_t sh_offset;
    Elf64_Xword_t sh_size;
    Elf64_Word_t sh_link;
    Elf64_Word_t sh_info;
    Elf64_Xword_t sh_addralign;
    Elf64_Xword_t sh_entsize;

} __attribute__((packed)) Elf64_Shdr_t;

#define SHN_UNDEF 0
#define SHN_LOPROC 0xFF00
#define SHN_HIPROC 0xFF1F
#define SHN_LOOS 0xFF20
#define SHN_HIOS 0xFF3F
#define SHN_ABS 0xFFF1
#define SHN_COMMON 0xFFF2

#define SHT_NULL 0
#define SHT_PROGBITS 1
#define SHT_SYMTAB 2
#define SHT_STRTAB 3
#define SHT_RELA 4
#define SHT_HASH 5
#define SHT_DYNAMIC 6
#define SHT_NOTE 7
#define SHT_NOBITS 8
#define SHT_REL 9
#define SHT_SHLIB 10
#define SHT_DYNSYM 11
#define SHT_LOOS 0x60000000
#define SHT_HIOS 0x6FFFFFFF
#define SHT_LOPROC 0x70000000
#define SHT_HIPROC 0x7FFFFFFF

#define SHF_WRITE 0x1
#define SHF_ALLOC 0x2
#define SHF_EXECINSTR 0x4
#define SHF_MASKOS 0x0F000000
#define SHF_MASKPROC 0xF0000000

//=============================================================================
// Symbol section
//=============================================================================

typedef struct
{
    Elf64_Word_t st_name;
    unsigned char st_info;
    unsigned char st_other;
    Elf64_Half_t st_shndx;
    Elf64_Addr_t st_value;
    Elf64_Xword_t st_size;

} __attribute__((packed)) Elf64_Sym_t;

#define ELF64_ST_BIND(INFO) ((INFO) >> 4)
#define ELF64_ST_TYPE(INFO) ((INFO)&0x0F)

#define STB_LOCAL 0
#define STB_GLOBAL 1
#define STB_WEAK 2
#define STB_LOOS 10
#define STB_HIOS 12
#define STB_LOPROC 13
#define STB_HIPROC 15

#define STT_NOTYPE 0
#define STT_OBJECT 1
#define STT_FUNC 2
#define STT_SECTION 3
#define STT_FILE 4
#define STT_LOOS 10
#define STT_HIOS 12
#define STT_LOPROC 13
#define STT_HIPROC 15

//=============================================================================
// Relocation info
//=============================================================================

typedef struct
{
    Elf64_Addr_t r_offset;
    Elf64_Xword_t r_info;

} __attribute__((packed)) Elf64_Rel_t;

typedef struct
{
    Elf64_Addr_t r_offset;
    Elf64_Xword_t r_info;
    Elf64_Sxword_t r_addend;

} __attribute__((packed)) Elf64_Rela_t;

#define ELF64_R_SYM(i) ((i) >> 32)
#define ELF64_R_TYPE(i) ((i)&0xFFFFFFFF)
#define ELF64_R_INFO(s, t) (((s) << 32) + ((t)&0xFFFFFFFF))

//=============================================================================
// Program header
//=============================================================================

typedef struct
{
    Elf64_Word_t p_type;
    Elf64_Word_t p_flags;
    Elf64_Off_t p_offset;
    Elf64_Addr_t p_vaddr;
    Elf64_Addr_t p_paddr;
    Elf64_Xword_t p_filesz;
    Elf64_Xword_t p_memsz;
    Elf64_Xword_t p_align;

} __attribute__((packed)) Elf64_Phdr_t;

#define PT_NULL 0
#define PT_LOAD 1
#define PT_DYNAMIC 2
#define PT_INERP 3
#define PT_NOTE 4
#define PT_SHLIB 5
#define PT_PHDR 6
#define PT_LOOS 0x60000000
#define PT_HIOS 0x6FFFFFFF
#define PT_LOPROC 0x70000000
#define PT_HIPROC 0x7FFFFFFF

#define PF_X 0x1
#define PF_W 0x2
#define PF_R 0x4
#define PF_MASKOS 0x00FF0000
#define PF_MASKPROC 0xFF000000

//=============================================================================
// Dynamic section
//=============================================================================

typedef struct
{
    Elf64_Sxword_t d_tag;

    union {
        Elf64_Xword_t d_val;
        Elf64_Addr_t d_ptr;
    } d_un;

} __attribute__((packed)) Elf64_Dyn_t;

#define DT_NULL 0
#define DT_NEEDED 1
#define DT_PLTRELSZ 2
#define DT_PLTGOT 3
#define DT_HASH 4
#define DT_STRTAB 5
#define DT_SYMTAB 6
#define DT_RELA 7
#define DT_RELASZ 8
#define DT_RELAENT 9
#define DT_STRSZ 10
#define DT_SYMENT 11
#define DT_INIT 12
#define DT_FINI 13
#define DT_SONAME 14
#define DT_RPATH 15
#define DT_SYMBOLIC 16
#define DT_REL 17
#define DT_RELSZ 18
#define DT_RELENT 19
#define DT_PLTREL 20
#define DT_DEBUG 21
#define DT_TEXTREL 22
#define DT_JMPREL 23
#define DT_BIND_NOW 24
#define DT_INIT_ARRAY 25
#define DT_FINI_ARRAY 26
#define DT_INIT_ARRAYSZ 27
#define DT_FINI_ARRAYSZ 28
#define DT_LOOS 0x60000000
#define DT_HIOS 0x6FFFFFFF
#define DT_LOPROC 0x70000000
#define DT_HIPROC 0x7FFFFFFF

//=============================================================================
// Functions
//=============================================================================

unsigned long elf64_hash(const unsigned char* name);
int kernel_lookup_symbol(void* addr);
void init_kernel_symbol_context(struct multiboot_tag_elf_sections* elf_sections, memory_info_t* mem_info);

//=============================================================================
// DWARF information
//=============================================================================

typedef struct
{
    uint32_t length;
    uint16_t version;
    uint32_t header_length;
    uint8_t min_instruction_length;
    uint8_t default_is_statement;
    int8_t line_base;
    uint8_t line_range;
    uint8_t opcode_base;
    uint8_t std_opcode_lengths[12];
} __attribute__((packed)) DwarfDebugLineHeader_t;

void dwarf_parse_debug_line_section(Elf64_Addr_t section_start, Elf64_Xword_t size, uint64_t addr);

//=============================================================================
// Image execution
//=============================================================================

int exec_elf(char* path, int argc, char** argv, char** env, int depth);

#endif

//=============================================================================
// End of file
//=============================================================================
