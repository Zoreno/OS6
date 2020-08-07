#ifndef _LIBUNWIND_EH_FRAME_H
#define _LIBUNWIND_EH_FRAME_H

#include <stdint.h>
#include <registers_intel_x64.h>
#include <eh_framelist.h>

class common_entry;
class ci_entry;
class fd_entry;

// Data Format
#define DW_EH_PE_absptr 0x00
#define DW_EH_PE_uleb128 0x01
#define DW_EH_PE_udata2 0x02
#define DW_EH_PE_udata4 0x03
#define DW_EH_PE_udata8 0x04
#define DW_EH_PE_sleb128 0x09
#define DW_EH_PE_sdata2 0x0A
#define DW_EH_PE_sdata4 0x0B
#define DW_EH_PE_sdata8 0x0C

// Data Application
#define DW_EH_PE_pcrel 0x10
#define DW_EH_PE_textrel 0x20
#define DW_EH_PE_datarel 0x30
#define DW_EH_PE_funcrel 0x40
#define DW_EH_PE_aligned 0x50

// Special
#define DW_EH_PE_omit 0xFF

uint64_t decode_pointer(char **addr, uint64_t encoding);

#define DW_CFA_GNU_args_size 0x2E
#define DW_CFA_GNU_negative_offset_extended 0x2F

class eh_frame
{
public:
    static fd_entry find_fde(register_state *state);
};

class common_entry
{
public:
    common_entry();

    explicit common_entry(const eh_frame_t &eh_frame);

    virtual ~common_entry() = default;

    common_entry(common_entry &&) noexcept = default;

    common_entry(const common_entry &) = default;

    common_entry &operator=(common_entry &&) noexcept = default;

    common_entry &operator=(const common_entry &) = default;

    common_entry &operator++();

    operator bool() const
    {
        return m_entry_start != nullptr;
    }

    bool is_cie() const
    {
        return m_is_cie;
    }

    bool is_fde() const
    {
        return !m_is_cie;
    }

    char *entry_start() const
    {
        return m_entry_start;
    }

    char *entry_end() const
    {
        return m_entry_end;
    }

    char *payload_start() const
    {
        return m_payload_start;
    }

    char *payload_end() const
    {
        return m_payload_end;
    }

    eh_frame_t eh_frame() const
    {
        return m_eh_frame;
    }

protected:
    virtual void parse(char *addr) = 0;
    void non_virtual_parse(char *addr);

protected:
    bool m_is_cie;

    char *m_entry_start;
    char *m_entry_end;

    char *m_payload_start;
    char *m_payload_end;

    eh_frame_t m_eh_frame;
};

class ci_entry : public common_entry
{
public:
    ci_entry();

    explicit ci_entry(const eh_frame_t &eh_frame);

    explicit ci_entry(const eh_frame_t &eh_frame, void *addr);

    ~ci_entry() override = default;

    ci_entry(ci_entry &&) noexcept = default;

    ci_entry(const ci_entry &) = default;

    ci_entry &operator=(ci_entry &&) noexcept = default;

    ci_entry &operator=(const ci_entry &) = default;

    char augmentation_string(uint64_t index) const
    {
        return m_augmentation_string[index];
    }

    uint64_t code_alignment() const
    {
        return m_code_alignment;
    }

    int64_t data_alignment() const
    {
        return m_data_alignment;
    }

    uint64_t return_address_reg() const
    {
        return m_return_address_reg;
    }

    uint64_t pointer_encoding() const
    {
        return m_pointer_encoding;
    }

    uint64_t lsda_encoding() const
    {
        return m_lsda_encoding;
    }

    uint64_t personality_encoding() const
    {
        return m_personality_encoding;
    }

    uint64_t personality_function() const
    {
        return m_personality_function;
    }

    char *initial_instructions() const
    {
        return m_initial_instructions;
    }

protected:
    void parse(char *addr) override;
    void non_virtual_parse(char *addr);

public:
    const char *m_augmentation_string;
    uint64_t m_code_alignment;
    int64_t m_data_alignment;
    uint64_t m_return_address_reg;
    uint64_t m_pointer_encoding;
    uint64_t m_lsda_encoding;
    uint64_t m_personality_encoding;
    uint64_t m_personality_function;
    char *m_initial_instructions;
};

class fd_entry : public common_entry
{
public:
    fd_entry();

    explicit fd_entry(const eh_frame_t &eh_frame);

    explicit fd_entry(const eh_frame_t &eh_frame, void *addr);

    ~fd_entry() override = default;

    fd_entry(fd_entry &&) noexcept = default;

    fd_entry(const fd_entry &) = default;

    fd_entry &operator=(fd_entry &&) noexcept = default;

    fd_entry &operator=(const fd_entry &) = default;

    bool is_in_range(uint64_t pc) const
    {  
        printf("is_in_range: pc: %#016x, pc_begin: %#016x, pc_end: %#016x\n", pc, m_pc_begin, m_pc_begin + m_pc_range);

        return (pc > m_pc_begin) && (pc <= m_pc_begin + m_pc_range);
    }

    uint64_t pc_begin() const
    {
        return m_pc_begin;
    }

    uint64_t pc_range() const
    {
        return m_pc_range;
    }

    uint64_t lsda() const
    {
        return m_lsda;
    }

    char *instructions() const
    {
        return m_instructions;
    }

    const ci_entry &cie() const
    {
        return m_cie;
    }

protected:
    void parse(char *addr) override;
    void non_virtual_parse(char *addr);

private:
    uint64_t m_pc_begin;
    uint64_t m_pc_range;
    uint64_t m_lsda;
    char *m_instructions;

    ci_entry m_cie;
};

#endif