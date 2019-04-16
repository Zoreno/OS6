#include <exec/elf64.h>

#include <stddef.h>
#include <stdio.h>
#include <string.h>

static Elf64_Sym_t *_symbol_section = NULL;
static int _symbol_count = 0;
static const char *_symbol_string_table = NULL;

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

const char *kernel_lookup_symbol(void *addr, int64_t *offset)
{
    if (!_symbol_section || !_symbol_string_table)
    {
        return NULL;
    }

    Elf64_Sym_t *best_match = NULL;

    // These could be placed in a map for log(N) access. This will do for now.
    for (int i = 0; i < _symbol_count; ++i)
    {
        // Only handle function symbols
        if ((_symbol_section[i].st_info & 0x0F) == STT_FUNC)
        {
            if (_symbol_section[i].st_value <= (Elf64_Addr_t)addr && (!best_match || best_match->st_value < _symbol_section[i].st_value))
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
        *offset = (Elf64_Addr_t)addr - best_match->st_value;
    }

    return _symbol_string_table + best_match->st_name;
}

void init_kernel_symbol_context(struct multiboot_tag_elf_sections *elf_sections)
{
    printf("Section count: %i\n", elf_sections->num);
    printf("Section entry size: %i\n", elf_sections->entsize);
    printf("Section shndx: %i\n", elf_sections->shndx);

    Elf64_Shdr_t *sec_header = (Elf64_Shdr_t *)elf_sections->sections;

    Elf64_Shdr_t *string_header = ((Elf64_Shdr_t *)elf_sections->sections) + elf_sections->shndx;

    const char *string_table = (const char *)string_header->sh_addr;

    for (uint32_t i = 0; i < elf_sections->num; ++i)
    {

        printf("Section %i:\n", i);
        printf("Name: %i (%s)\n", sec_header[i].sh_name, string_table + sec_header[i].sh_name);
        printf("Type: %i\n", sec_header[i].sh_type);
        printf("Addr: %#016x\n", sec_header[i].sh_addr);
        printf("Size: %i\n", sec_header[i].sh_size);

        if (strcmp(string_table + sec_header[i].sh_name, ".strtab") == 0)
        {
            _symbol_string_table = (const char *)sec_header[i].sh_addr;
            printf("Found .strtab at: %#016x\n", _symbol_string_table);
        }

        if (strcmp(string_table + sec_header[i].sh_name, ".symtab") == 0)
        {
            _symbol_section = (Elf64_Sym_t *)sec_header[i].sh_addr;
            _symbol_count = sec_header[i].sh_size / sizeof(Elf64_Sym_t);
            printf("Found .symtab at: %#016x. (%i entries)\n", _symbol_section, _symbol_count);
        }

        if (strcmp(string_table + sec_header[i].sh_name, ".debug_line") == 0)
        {
            dwarf_parse_debug_line_section(sec_header[i].sh_addr, sec_header[i].sh_size);
        }
    }
}

static size_t read_uleb128(const unsigned char *buf, const unsigned char *buf_end, uint64_t *r)
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

static size_t read_sleb128(const unsigned char *buf, const unsigned char *buf_end, int64_t *r)
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

        if ((byte & 0x80) == 0)
        {
            break;
        }

        shift += 7;
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

void dwarf_parse_debug_line_program(DwarfDebugLineHeader_t *header)
{
    uint8_t *op = (uint8_t *)header + header->header_length;
    uint8_t *end = (uint8_t *)header + header->length + sizeof(header->length);

    uint64_t addr = 0;
    uint32_t op_index = 0;
    uint32_t file = 1;
    uint32_t line = 1;
    uint32_t column = 1;
    uint8_t is_stmt = header->default_is_statement;
    uint8_t basic_block = 0;
    uint8_t end_sequence = 0;
    uint8_t prologue_end = 0;
    uint8_t epilogue_begin = 0;
    uint8_t isa = 0;
    uint32_t discriminator = 0;

    while (op < end)
    {
        uint8_t current_opcode = *op;

        //printf("Current opcode: %#02x\n", (uint64_t)current_opcode);

        if (current_opcode <= 12)
        {
            switch (current_opcode)
            {
            case DW_LNS_copy:
            {
                printf("Append line: %i, file: %i, address: %#016x\n", (uint64_t)line, file, addr);
                discriminator = 0;
                basic_block = 0;
                prologue_end = 0;
                epilogue_begin = 0;

                printf("DW_LNS_copy\n");
            }
            break;
            case DW_LNS_advance_pc:
            {
                uint64_t operand;
                size_t n = read_uleb128(op + 1, end, &operand);
                addr += operand;
                op += n;

                printf("DW_LNS_advance_pc: %i\n", operand);
            }
            break;
            case DW_LNS_advance_line:
            {
                uint64_t operand;
                size_t n = read_uleb128(op + 1, end, &operand);
                line += operand;
                op += n;

                printf("DW_LNS_advance_line: %i\n", operand);
            }
            break;
            case DW_LNS_set_file:
            {
                uint64_t operand;
                size_t n = read_uleb128(op + 1, end, &operand);
                file = operand;
                op += n;

                printf("DW_LNS_advance_file: %i\n", operand);
            }
            break;
            case DW_LNS_negate_stmt:
            {
                is_stmt = 1 - is_stmt;

                printf("DW_LNS_negate_stmt\n");
            }
            break;
            case DW_LNS_set_basic_block:
            {
                basic_block = 1;

                printf("DW_LNS_set_basic_block\n");
            }
            break;
            case DW_LNS_const_add_pc:
            {
                line += header->line_base + (255 % header->line_range);
                printf("DW_LNS_const_add_pc\n");
            }
            break;
            case DW_LNS_fixed_advance_pc:
            {
                uint16_t operand;

                operand = (*(op + 1)) << 8 | (*(op + 2));
                addr += operand;
                printf("DW_LNS_const_add_pc: %i\n", operand);
                op += 2;
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

                printf("DW_LNS_set_isa: %i\n", operand);
            }
            break;
            }
        }
        else
        {
            uint8_t adjusted_opcode = current_opcode - header->opcode_base;
            uint8_t operation_advance = adjusted_opcode / header->line_range;

            addr += header->min_instruction_length * operation_advance;
            line += header->line_base + (adjusted_opcode % header->line_range);
        }

        ++op;
    }

    for (;;)
        ;
}

void dwarf_parse_debug_line_section(Elf64_Addr_t section_start, Elf64_Xword_t size)
{
    printf("=======================================================\n");
    printf("Dwarf debug line info:\n");
    printf("Section start: %#016x, size: %i\n", section_start, size);
    printf("=======================================================\n");

    DwarfDebugLineHeader_t *header = (DwarfDebugLineHeader_t *)section_start;

    while (header < section_start + size)
    {

        printf("Length: %i\n", header->length);
        printf("DWARF version: %i\n", header->version);
        printf("Header length: %i\n", header->header_length);
        printf("Minimum instruction length: %i\n", header->min_instruction_length);
        printf("Line base: %i\n", (int64_t)header->line_base);
        printf("Line range: %i\n", header->line_range);
        printf("Opcode base: %i\n", header->opcode_base);

        const char *dir_table = ((uint8_t *)header) + sizeof(DwarfDebugLineHeader_t);

        printf("Directory table:\n");

        while (*dir_table)
        {
            printf("%s\n", dir_table);

            dir_table += strlen(dir_table) + 1;
        }

        const char *file_table = dir_table + 1;

        printf("File table:\n");

        while (*file_table)
        {
            printf("%s\n", file_table);

            file_table += strlen(file_table) + 1 + 3;
        }

        dwarf_parse_debug_line_program(header);

        // Advance to the next header
        header = ((uint8_t *)header) + header->length + sizeof(header->length);
    }
}
