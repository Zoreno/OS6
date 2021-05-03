/**
 * @file elf64.c
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-06-16
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
#include <logging/logging.h>

#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

static size_t read_sleb128(const unsigned char *buf,
                           const unsigned char *buf_end,
                           int64_t *r);
static size_t read_uleb128(const unsigned char *buf,
                           const unsigned char *buf_end,
                           uint64_t *r);

// Elf based symbol lookup
static Elf64_Sym_t *_symbol_section = NULL;
static int _symbol_count = 0;
static const char *_symbol_string_table = NULL;

// Dwarf based symbol lookup
static Elf64_Addr_t _dwarf_line_info = 0;
static Elf64_Xword_t _dwarf_size = 0;

static memory_info_t _mem_info;

unsigned long elf64_hash(const unsigned char *name)
{
    unsigned long h = 0;
    unsigned long g;

    while (*name)
    {
        h = (h << 4) + *name++;

        if ((g = (h & 0xF0000000)))
        {
            h ^= g >> 24;
        }

        h &= 0x0FFFFFFF;
    }

    return h;
}

int elf_kernel_lookup_symbol(void *addr)
{
    if (!_symbol_section || !_symbol_string_table)
    {
        return 0;
    }

    if (addr > (void *)_mem_info.kernel_end ||
        addr < (void *)_mem_info.kernel_load_addr)
    {
        printf("External code");
        return 1;
    }

    Elf64_Sym_t *best_match = NULL;

    // These could be placed in a map for log(N) access. This will do for now.
    for (int i = 0; i < _symbol_count; ++i)
    {
        // Only handle function symbols
        if ((_symbol_section[i].st_info & 0x0F) == STT_FUNC)
        {
            if (_symbol_section[i].st_value <= (Elf64_Addr_t)addr &&
                (!best_match ||
                 best_match->st_value < _symbol_section[i].st_value))
            {
                best_match = &_symbol_section[i];
            }
        }
    }

    if (!best_match)
    {
        return 0;
    }

    uint64_t offset = (Elf64_Addr_t)addr - best_match->st_value;

    printf("%s", _symbol_string_table + best_match->st_name);

    return 0;
}

void dwarf_kernel_lookup_symbol(void *addr)
{
    // printf("DWARF line info: %#016x, size: %i, addr: %#016x\n",
    // _dwarf_line_info, _dwarf_size, addr);
    dwarf_parse_debug_line_section(
        _dwarf_line_info, _dwarf_size, (uint64_t)addr);
}

int kernel_lookup_symbol(void *addr)
{
    int external_symbol = 0;

    if (_symbol_section || _dwarf_line_info)
    {
        if (_symbol_section)
        {
            int ext = elf_kernel_lookup_symbol(addr);

            if (ext)
            {
                external_symbol = 1;
            }

            printf("()");
        }

        if (_dwarf_line_info)
        {
            printf(" [");
            dwarf_kernel_lookup_symbol(addr);
            printf("]");
        }
    }
    else
    {
        printf("Error looking up symbol");
    }

    return external_symbol;
}

void init_kernel_symbol_context(struct multiboot_tag_elf_sections *elf_sections,
                                memory_info_t *mem_info)
{
    log_debug("Section count: %i", elf_sections->num);
    log_debug("Section entry size: %i", elf_sections->entsize);
    log_debug("Section shndx: %i", elf_sections->shndx);

    Elf64_Shdr_t *sec_header = (Elf64_Shdr_t *)elf_sections->sections;

    Elf64_Shdr_t *string_header =
        ((Elf64_Shdr_t *)elf_sections->sections) + elf_sections->shndx;

    const char *string_table = (const char *)string_header->sh_addr;

    for (uint32_t i = 0; i < elf_sections->num; ++i)
    {
        if (strcmp(string_table + sec_header[i].sh_name, ".strtab") == 0)
        {
            _symbol_string_table = (const char *)sec_header[i].sh_addr;
            log_debug("Found .strtab at: %#016x", _symbol_string_table);
        }

        if (strcmp(string_table + sec_header[i].sh_name, ".symtab") == 0)
        {
            _symbol_section = (Elf64_Sym_t *)sec_header[i].sh_addr;
            _symbol_count = sec_header[i].sh_size / sizeof(Elf64_Sym_t);
            log_debug("Found .symtab at: %#016x. (%i entries)",
                      _symbol_section,
                      _symbol_count);
        }

#ifdef OS6_DEBUG

        if (strcmp(string_table + sec_header[i].sh_name, ".debug_line") == 0)
        {
            // dwarf_parse_debug_line_section(sec_header[i].sh_addr,
            // sec_header[i].sh_size);

            _dwarf_line_info = sec_header[i].sh_addr;
            _dwarf_size = sec_header[i].sh_size;

            log_debug("Found .debug_line at: %#016x", _dwarf_line_info);
        }

#endif

        if (sec_header[i].sh_addr + sec_header[i].sh_size >
            mem_info->kernel_end)
        {
            mem_info->kernel_end =
                sec_header[i].sh_addr + sec_header[i].sh_size;
        }
    }

    memcpy(&_mem_info, mem_info, sizeof(memory_info_t));
}

static size_t read_uleb128(const unsigned char *buf,
                           const unsigned char *buf_end,
                           uint64_t *r)
{
    const unsigned char *p = buf;
    unsigned int shift = 0;
    uint64_t result = 0;
    unsigned char byte;

    while (1)
    {
        if (p >= buf_end)
        {
            return 0;
        }

        byte = *p++;

        result |= ((uint64_t)(byte & 0x7F)) << shift;

        if ((byte & 0x80) == 0)
        {
            break;
        }

        shift += 7;
    }

    *r = result;

    return p - buf;
}

static size_t read_sleb128(const unsigned char *buf,
                           const unsigned char *buf_end,
                           int64_t *r)
{
    const unsigned char *p = buf;
    unsigned int shift = 0;
    int64_t result = 0;
    unsigned char byte;

    while (1)
    {
        if (p >= buf_end)
        {
            return 0;
        }

        byte = *p++;

        result |= ((uint64_t)(byte & 0x7F)) << shift;

        shift += 7;

        if ((byte & 0x80) == 0)
        {
            break;
        }
    }

    if (shift < (sizeof(*r) * 8) && (byte & 0x40) != 0)
    {
        result |= -(((uint64_t)1) << shift);
    }

    *r = result;

    return p - buf;
}

#define DW_LNS_copy 1
#define DW_LNS_advance_pc 2
#define DW_LNS_advance_line 3
#define DW_LNS_set_file 4
#define DW_LNS_set_column 5
#define DW_LNS_negate_stmt 6
#define DW_LNS_set_basic_block 7
#define DW_LNS_const_add_pc 8
#define DW_LNS_fixed_advance_pc 9
#define DW_LNS_set_prologue_end 10
#define DW_LNS_set_epilogue_begin 11
#define DW_LNS_set_isa 12

#define DW_LNE_end_sequence 1
#define DW_LNE_set_address 2
#define DW_LNE_define_file 3
#define DW_LNE_set_discriminator 4

void dwarf_parse_debug_line_program(DwarfDebugLineHeader_t *header,
                                    uint64_t *start_addr_p,
                                    uint64_t *end_addr_p,
                                    uint32_t *line_p,
                                    uint32_t *file_p,
                                    uint64_t target_addr)
{
    uint8_t *op = (uint8_t *)header +
                  offsetof(DwarfDebugLineHeader_t, header_length) +
                  sizeof(header->header_length) + header->header_length;
    uint8_t *end = (uint8_t *)header + header->length + sizeof(header->length);

    // TODO: Move these to a state struct
    uint64_t addr = 0;
    uint32_t op_index = 0;
    uint32_t file = 1;
    int32_t line = 1;
    uint32_t column = 1;
    uint8_t is_stmt = header->default_is_statement;
    uint8_t basic_block = 0;
    uint8_t end_sequence = 0;
    uint8_t prologue_end = 0;
    uint8_t epilogue_begin = 0;
    uint8_t isa = 0;
    uint32_t discriminator = 0;

    uint64_t start_addr = UINT64_MAX;
    uint64_t end_addr = 0;

    uint32_t best_addr = end_addr;
    uint32_t best_file = 1;
    uint32_t best_line = 1;

    while (op < end)
    {
        uint8_t current_opcode = *op;

        // printf("Current opcode: %#02x\n", (uint64_t)current_opcode);

        // Extended set
        if (current_opcode == 0)
        {
            // printf("Extended Opcode\n");

            op += 1;

            uint64_t bytes;
            size_t n = read_uleb128(op, end, &bytes);
            op += n;

            // printf("Size: %i\n", bytes);

            uint8_t ext_opcode = *op;

            switch (ext_opcode)
            {
            case DW_LNE_end_sequence:
            {
                end_sequence = 1;

                if (addr > end_addr)
                {
                    end_addr = addr;
                }

                if (addr < start_addr)
                {
                    start_addr = addr;
                }

                if (addr < target_addr && addr >= best_addr)
                {
                    best_addr = addr;
                    best_file = file;
                    best_line = line;
                }

                // printf("Append line: %i, file: %i, address: %#016x\n",
                // (uint64_t)line, file, addr);

                addr = 0;
                op_index = 0;
                file = 1;
                line = 1;
                column = 1;
                is_stmt = header->default_is_statement;
                basic_block = 0;
                prologue_end = 0;
                epilogue_begin = 0;
                isa = 0;
                discriminator = 0;

                // printf("DW_LNE_end_sequence\n");
            }
            break;
            case DW_LNE_set_address:
            {
                uint64_t new_addr = 0;

                for (uint32_t i = 0; i < 8; ++i)
                {
                    new_addr |= (*(op + i + 1)) << (8 * i);
                }

                addr = new_addr;

                op += 8;
                // printf("DW_LNE_set_address: %#016x\n", new_addr);
            }
            break;
            case DW_LNE_set_discriminator:
            {
                uint64_t operand;
                size_t n = read_uleb128(op + 1, end, &operand);
                discriminator = operand;
                op += n;

                // printf("DW_LNE_set_discriminator: %i\n", operand);
            }
            break;
            default:
            {
                printf("WARNING: Skipping extended opcode %i\n",
                       (uint64_t)ext_opcode);
                op += bytes - 1;
            }
            break;
            }
        }
        else if (current_opcode < header->opcode_base)
        {
            // Standard set
            switch (current_opcode)
            {
            case DW_LNS_copy:
            {
                // printf("Append line: %i, file: %i, address: %#016x\n",
                // (uint64_t)line, file, addr);

                if (addr > end_addr)
                {
                    end_addr = addr;
                }

                if (addr < start_addr)
                {
                    start_addr = addr;
                }

                if (addr < target_addr && addr >= best_addr)
                {
                    best_addr = addr;
                    best_file = file;
                    best_line = line;
                }

                basic_block = 0;
                prologue_end = 0;
                epilogue_begin = 0;
                discriminator = 0;

                // printf("DW_LNS_copy\n");
            }
            break;
            case DW_LNS_advance_pc:
            {
                uint64_t operand;
                size_t n = read_uleb128(op + 1, end, &operand);
                addr += operand;
                op += n;

                // printf("DW_LNS_advance_pc: %i\n", operand);
            }
            break;
            case DW_LNS_advance_line:
            {
                int64_t operand;
                size_t n = read_sleb128(op + 1, end, &operand);
                line += operand;
                op += n;

                // printf("DW_LNS_advance_line: %i\n", operand);
            }
            break;
            case DW_LNS_set_file:
            {
                uint64_t operand;
                size_t n = read_uleb128(op + 1, end, &operand);
                file = operand;
                op += n;

                // printf("DW_LNS_advance_file: %i\n", operand);
            }
            break;
            case DW_LNS_set_column:
            {
                uint64_t operand;
                size_t n = read_uleb128(op + 1, end, &operand);
                column = operand;
                op += n;

                // printf("DW_LNS_set_column: %i\n", operand);
            }
            break;
            case DW_LNS_negate_stmt:
            {
                is_stmt = 1 - is_stmt;

                // printf("DW_LNS_negate_stmt\n");
            }
            break;
            case DW_LNS_set_basic_block:
            {
                basic_block = 1;

                // printf("DW_LNS_set_basic_block\n");
            }
            break;
            case DW_LNS_const_add_pc:
            {
                uint8_t adjusted_opcode = 255 - header->opcode_base;
                uint8_t operation_advance =
                    adjusted_opcode / header->line_range;

                addr += header->min_instruction_length * operation_advance;

                // printf("DW_LNS_const_add_pc\n");
            }
            break;
            case DW_LNS_fixed_advance_pc:
            {
                uint16_t operand;

                operand = (*(op + 1)) << 8 | (*(op + 2));
                addr += operand;
                op += 2;
                // printf("DW_LNS_const_add_pc: %i\n", operand);
            }
            break;
            case DW_LNS_set_prologue_end:
            {
                prologue_end = 1;
            }
            break;
            case DW_LNS_set_epilogue_begin:
            {
                epilogue_begin = 1;
            }
            break;
            case DW_LNS_set_isa:
            {
                uint64_t operand;
                size_t n = read_uleb128(op + 1, end, &operand);
                isa = operand;
                op += n;

                // printf("DW_LNS_set_isa: %i\n", operand);
            }
            break;
            }
        }
        else
        {
            // Special opcode
            uint8_t adjusted_opcode = current_opcode - header->opcode_base;
            uint8_t operation_advance = adjusted_opcode / header->line_range;

            addr += header->min_instruction_length * operation_advance;
            line += header->line_base + (adjusted_opcode % header->line_range);

            if (addr > end_addr)
            {
                end_addr = addr;
            }

            if (addr < start_addr)
            {
                start_addr = addr;
            }

            if (addr < target_addr && addr >= best_addr)
            {
                best_addr = addr;
                best_file = file;
                best_line = line;
            }

            // printf("Append line: %i, file: %i, address: %#016x\n",
            // (uint64_t)line, file, addr);

            basic_block = 0;
            prologue_end = 0;
            epilogue_begin = 0;
            discriminator = 0;
        }

        ++op;
    }

    *start_addr_p = start_addr;
    *end_addr_p = end_addr;
    *file_p = best_file;
    *line_p = best_line;
}

void dwarf_parse_debug_line_section(Elf64_Addr_t section_start,
                                    Elf64_Xword_t size,
                                    uint64_t addr)
{
    // printf("=======================================================\n");
    // printf("Dwarf debug line info:\n");
    // printf("Section start: %#016x, size: %i\n", section_start, size);
    // printf("=======================================================\n");

    DwarfDebugLineHeader_t *header = (DwarfDebugLineHeader_t *)section_start;

    int found = 0;

    while (header < (DwarfDebugLineHeader_t *)(section_start + size))
    {
        // printf("Length: %i\n", header->length);
        // printf("DWARF version: %i\n", header->version);
        // printf("Header length: %i\n", header->header_length);
        // printf("Minimum instruction length: %i\n",
        // header->min_instruction_length); printf("Line base: %i\n",
        // (int64_t)header->line_base); printf("Line range: %i\n",
        // header->line_range); printf("Opcode base: %i\n",
        // header->opcode_base);

        uint8_t *end_of_header = (uint8_t *)header + header->header_length;

        const char *dir_table =
            ((uint8_t *)header) + sizeof(DwarfDebugLineHeader_t);

        // printf("Directory table:\n");

        struct
        {
            const char *str;
        } dir_table_entries[16];

        memset(dir_table_entries, 0, sizeof(dir_table_entries));

        struct
        {
            const char *str;
            uint32_t dir;
        } file_table_entries[16];

        memset(file_table_entries, 0, sizeof(file_table_entries));

        uint32_t curr_dir = 1;

        while (*dir_table)
        {
            // printf("%s\n", dir_table);

            dir_table_entries[curr_dir].str = dir_table;

            dir_table += strlen(dir_table) + 1;

            curr_dir++;
        }

        const char *file_table = dir_table + 1;

        // printf("File table:\n");

        uint32_t curr_file = 1;

        while (*file_table)
        {
            // printf(" * %s ", file_table);

            file_table_entries[curr_file].str = file_table;

            file_table += strlen(file_table) + 1;

            uint64_t dir;
            uint64_t mod;
            uint64_t len;

            read_uleb128(file_table, end_of_header, &dir);
            read_uleb128(file_table + 1, end_of_header, &mod);
            read_uleb128(file_table + 2, end_of_header, &len);

            file_table_entries[curr_file].dir = dir;

            file_table += 3;

            curr_file++;

            // printf("dir: %i, mod: %i, len: %i\n", dir, mod, len);
        }

        uint64_t start_addr;
        uint64_t end_addr;
        uint32_t line = 1;
        uint32_t file = 1;

        dwarf_parse_debug_line_program(
            header, &start_addr, &end_addr, &line, &file, addr);

        // printf("Start addr: %#016x, end addr: %#016x\n", start_addr,
        // end_addr);

        if (addr <= end_addr && addr >= start_addr)
        {
            printf("%s/%s:%i",
                   dir_table_entries[file_table_entries[file].dir].str,
                   file_table_entries[file].str,
                   line);
            found = 1;
        }

        // Advance to the next header
        header =
            (DwarfDebugLineHeader_t *)(((uint8_t *)header) + header->length +
                                       sizeof(header->length));
    }

    if (!found)
    {
        printf("External code");
    }
}

//=============================================================================
// End of file
//=============================================================================
