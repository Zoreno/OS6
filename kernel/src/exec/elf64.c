#include <exec/elf64.h>

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

// Elf based symbol lookup
static Elf64_Sym_t *_symbol_section = NULL;
static int _symbol_count = 0;
static const char *_symbol_string_table = NULL;

// Dwarf based symbol lookup
static Elf64_Addr_t _dwarf_line_info = NULL;
static Elf64_Xword_t _dwarf_size = 0;

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

void elf_kernel_lookup_symbol(void *addr)
{
    if (!_symbol_section || !_symbol_string_table)
    {
        return;
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
        return;
    }

    uint64_t offset = (Elf64_Addr_t)addr - best_match->st_value;

    printf("%s", _symbol_string_table + best_match->st_name);
}

void dwarf_kernel_lookup_symbol(void *addr)
{
    //printf("DWARF line info: %#016x, size: %i, addr: %#016x\n", _dwarf_line_info, _dwarf_size, addr);
    dwarf_parse_debug_line_section(_dwarf_line_info, _dwarf_size, addr);
}

void kernel_lookup_symbol(void *addr)
{
    if (_symbol_section || _dwarf_line_info)
    {

        if (_symbol_section)
        {
            elf_kernel_lookup_symbol(addr);
            printf("()");
        }

        if (_dwarf_line_info)
        {
            printf(" [");
            dwarf_kernel_lookup_symbol(addr);
            printf("]");
        }
        return;
    }

    printf("Error looking up symbol");
}

void init_kernel_symbol_context(struct multiboot_tag_elf_sections *elf_sections, memory_info_t *mem_info)
{
    printf("Section count: %i\n", elf_sections->num);
    printf("Section entry size: %i\n", elf_sections->entsize);
    printf("Section shndx: %i\n", elf_sections->shndx);

    Elf64_Shdr_t *sec_header = (Elf64_Shdr_t *)elf_sections->sections;

    Elf64_Shdr_t *string_header = ((Elf64_Shdr_t *)elf_sections->sections) + elf_sections->shndx;

    const char *string_table = (const char *)string_header->sh_addr;

    for (uint32_t i = 0; i < elf_sections->num; ++i)
    {

        //printf("Section %i:\n", i);
        //printf("Name: %i (%s)\n", sec_header[i].sh_name, string_table + sec_header[i].sh_name);
        //printf("Type: %i\n", sec_header[i].sh_type);
        //printf("Addr: %#016x\n", sec_header[i].sh_addr);
        //printf("Size: %i\n", sec_header[i].sh_size);

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

#ifdef OS6_DEBUG

        if (strcmp(string_table + sec_header[i].sh_name, ".debug_line") == 0)
        {
            //dwarf_parse_debug_line_section(sec_header[i].sh_addr, sec_header[i].sh_size);

            _dwarf_line_info = sec_header[i].sh_addr;
            _dwarf_size = sec_header[i].sh_size;

            printf("Found .debug_line at: %#016x\n", _dwarf_line_info);
        }

#endif

        if (sec_header[i].sh_addr + sec_header[i].sh_size > mem_info->kernel_end)
        {
            mem_info->kernel_end = sec_header[i].sh_addr + sec_header[i].sh_size;
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

void dwarf_parse_debug_line_program(DwarfDebugLineHeader_t *header, uint64_t *start_addr_p,
                                    uint64_t *end_addr_p, uint32_t *line_p, uint32_t *file_p, uint64_t target_addr)
{
    uint8_t *op = (uint8_t *)header +
                  offsetof(DwarfDebugLineHeader_t, header_length) +
                  sizeof(header->header_length) +
                  header->header_length;
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

        //printf("Current opcode: %#02x\n", (uint64_t)current_opcode);

        // Extended set
        if (current_opcode == 0)
        {
            //printf("Extended Opcode\n");

            op += 1;

            uint64_t bytes;
            size_t n = read_uleb128(op, end, &bytes);
            op += n;

            //printf("Size: %i\n", bytes);

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

                //printf("Append line: %i, file: %i, address: %#016x\n", (uint64_t)line, file, addr);

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

                //printf("DW_LNE_end_sequence\n");
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
                //printf("DW_LNE_set_address: %#016x\n", new_addr);
            }
            break;
            case DW_LNE_set_discriminator:
            {
                uint64_t operand;
                size_t n = read_uleb128(op + 1, end, &operand);
                discriminator = operand;
                op += n;

                //printf("DW_LNE_set_discriminator: %i\n", operand);
            }
            break;
            default:
            {
                printf("WARNING: Skipping extended opcode %i\n", (uint64_t)ext_opcode);
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
                //printf("Append line: %i, file: %i, address: %#016x\n", (uint64_t)line, file, addr);

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

                //printf("DW_LNS_copy\n");
            }
            break;
            case DW_LNS_advance_pc:
            {
                uint64_t operand;
                size_t n = read_uleb128(op + 1, end, &operand);
                addr += operand;
                op += n;

                //printf("DW_LNS_advance_pc: %i\n", operand);
            }
            break;
            case DW_LNS_advance_line:
            {
                int64_t operand;
                size_t n = read_sleb128(op + 1, end, &operand);
                line += operand;
                op += n;

                //printf("DW_LNS_advance_line: %i\n", operand);
            }
            break;
            case DW_LNS_set_file:
            {
                uint64_t operand;
                size_t n = read_uleb128(op + 1, end, &operand);
                file = operand;
                op += n;

                //printf("DW_LNS_advance_file: %i\n", operand);
            }
            break;
            case DW_LNS_set_column:
            {
                uint64_t operand;
                size_t n = read_uleb128(op + 1, end, &operand);
                column = operand;
                op += n;

                //printf("DW_LNS_set_column: %i\n", operand);
            }
            case DW_LNS_negate_stmt:
            {
                is_stmt = 1 - is_stmt;

                //printf("DW_LNS_negate_stmt\n");
            }
            break;
            case DW_LNS_set_basic_block:
            {
                basic_block = 1;

                //printf("DW_LNS_set_basic_block\n");
            }
            break;
            case DW_LNS_const_add_pc:
            {
                uint8_t adjusted_opcode = 255 - header->opcode_base;
                uint8_t operation_advance = adjusted_opcode / header->line_range;

                addr += header->min_instruction_length * operation_advance;

                //printf("DW_LNS_const_add_pc\n");
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

                //printf("DW_LNS_set_isa: %i\n", operand);
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

            //printf("Append line: %i, file: %i, address: %#016x\n", (uint64_t)line, file, addr);

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

void dwarf_parse_debug_line_section(Elf64_Addr_t section_start, Elf64_Xword_t size, uint64_t addr)
{
    //printf("=======================================================\n");
    //printf("Dwarf debug line info:\n");
    //printf("Section start: %#016x, size: %i\n", section_start, size);
    //printf("=======================================================\n");

    DwarfDebugLineHeader_t *header = (DwarfDebugLineHeader_t *)section_start;

    while (header < section_start + size)
    {

        //printf("Length: %i\n", header->length);
        //printf("DWARF version: %i\n", header->version);
        //printf("Header length: %i\n", header->header_length);
        //printf("Minimum instruction length: %i\n", header->min_instruction_length);
        //printf("Line base: %i\n", (int64_t)header->line_base);
        //printf("Line range: %i\n", header->line_range);
        //printf("Opcode base: %i\n", header->opcode_base);

        uint8_t *end_of_header = (uint8_t *)header + header->header_length;

        const char *dir_table = ((uint8_t *)header) + sizeof(DwarfDebugLineHeader_t);
        const char *dir_table_start = dir_table;

        //printf("Directory table:\n");

        while (*dir_table)
        {
            //printf("%s\n", dir_table);

            dir_table += strlen(dir_table) + 1;
        }

        const char *file_table = dir_table + 1;
        const char *file_table_start = file_table;

        //printf("File table:\n");

        while (*file_table)
        {
            //printf(" * %s ", file_table);

            file_table += strlen(file_table) + 1;

            uint64_t dir;
            uint64_t mod;
            uint64_t len;

            read_uleb128(file_table, end_of_header, &dir);
            read_uleb128(file_table + 1, end_of_header, &mod);
            read_uleb128(file_table + 2, end_of_header, &len);

            file_table += 3;

            //printf("dir: %i, mod: %i, len: %i\n", dir, mod, len);
        }

        uint64_t start_addr;
        uint64_t end_addr;
        uint32_t line = 1;
        uint32_t file = 1;

        dwarf_parse_debug_line_program(header, &start_addr, &end_addr, &line, &file, addr);

        //printf("Start addr: %#016x, end addr: %#016x\n", start_addr, end_addr);

        if (addr <= end_addr && addr >= start_addr)
        {
            printf("%s:%i", file_table_start, line);
        }

        // Advance to the next header
        header = ((uint8_t *)header) + header->length + sizeof(header->length);
    }
}