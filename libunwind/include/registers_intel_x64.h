/**
 * @file registers_intel_x64.h
 * @author Joakim Bertils
 * @version 0.1
 * @date 2020-12-31
 * 
 * @brief 
 * 
 * @copyright Copyright (C) 2020,
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

#ifndef _LIBUNWIND_REGISTERS_INTEL_X86_H
#define _LIBUNWIND_REGISTERS_INTEL_X86_H

#include <abort.h>
#include <registers.h>
#include <stdint.h>

#define LOG(...)

struct registers_intel_x64_t
{
    uint64_t rax;
    uint64_t rdx;
    uint64_t rcx;
    uint64_t rbx;
    uint64_t rdi;
    uint64_t rsi;
    uint64_t rbp;
    uint64_t rsp;
    uint64_t r08;
    uint64_t r09;
    uint64_t r10;
    uint64_t r11;
    uint64_t r12;
    uint64_t r13;
    uint64_t r14;
    uint64_t r15;
    uint64_t rip;
} __attribute__((packed));

// ASM functions
extern "C" void __store_registers_intel_x64(registers_intel_x64_t *state);
extern "C" void __load_registers_intel_x64(registers_intel_x64_t *state);

class register_state_intel_x64 : public register_state
{
public:
    register_state_intel_x64(const registers_intel_x64_t &registers) : m_registers(registers),
                                                                       m_tmp_registers(registers)

    {
    }

    ~register_state_intel_x64() override = default;

    register_state_intel_x64(register_state_intel_x64 &&) noexcept = default;
    register_state_intel_x64(const register_state_intel_x64 &) = default;

    register_state_intel_x64 &operator=(register_state_intel_x64 &&) noexcept = default;
    register_state_intel_x64 &operator=(const register_state_intel_x64 &) = default;

    uint64_t get_ip() const override
    {
        return m_registers.rip;
    }

    register_state &set_ip(uint64_t value) override
    {
        m_tmp_registers.rip = value;
        return *this;
    }

    uint64_t get(uint64_t index) const override
    {
        if (index >= max_num_registers())
        {
            ABORT("register index out of bounds");
        }

        return reinterpret_cast<const uint64_t *>(&m_registers)[index];
    }

    register_state &set(uint64_t index, uint64_t value) override
    {
        if (index >= max_num_registers())
        {
            ABORT("register index out of bounds");
        }

        reinterpret_cast<uint64_t *>(&m_tmp_registers)[index] = value;

        return *this;
    }

    void commit() override
    {
        m_registers = m_tmp_registers;
    }

    void commit(uint64_t cfa) override
    {
        m_tmp_registers.rsp = cfa;
        commit();
    }

    void resume() override
    {
        __load_registers_intel_x64(&m_registers);
    }

    uint64_t max_num_registers() const override
    {
        return 17;
    }

    const char *name(uint64_t index) const override
    {
        if (index >= max_num_registers())
        {
            ABORT("register index out of bounds");
        }

        switch (index)
        {
        case 0x00:
            return "rax";
        case 0x01:
            return "rdx";
        case 0x02:
            return "rcx";
        case 0x03:
            return "rbx";
        case 0x04:
            return "rdi";
        case 0x05:
            return "rsi";
        case 0x06:
            return "rbp";
        case 0x07:
            return "rsp";
        case 0x08:
            return "r08";
        case 0x09:
            return "r09";
        case 0x0A:
            return "r10";
        case 0x0B:
            return "r11";
        case 0x0C:
            return "r12";
        case 0x0D:
            return "r13";
        case 0x0E:
            return "r14";
        case 0x0F:
            return "r15";
        case 0x10:
            return "rip";
        default:
            return "";
        }
    }

    void dump() const override
    {
        LOG("Register State:\n");
        LOG("  rax: 0x%08lx\n", m_registers.rax);
        LOG("  rdx: 0x%08lx\n", m_registers.rdx);
        LOG("  rcx: 0x%08lx\n", m_registers.rcx);
        LOG("  rbx: 0x%08lx\n", m_registers.rbx);
        LOG("  rdi: 0x%08lx\n", m_registers.rdi);
        LOG("  rsi: 0x%08lx\n", m_registers.rsi);
        LOG("  rbp: 0x%08lx\n", m_registers.rbp);
        LOG("  rsp: 0x%08lx\n", m_registers.rsp);
        LOG("  r08: 0x%08lx\n", m_registers.r08);
        LOG("  r09: 0x%08lx\n", m_registers.r09);
        LOG("  r10: 0x%08lx\n", m_registers.r10);
        LOG("  r11: 0x%08lx\n", m_registers.r11);
        LOG("  r12: 0x%08lx\n", m_registers.r12);
        LOG("  r13: 0x%08lx\n", m_registers.r13);
        LOG("  r14: 0x%08lx\n", m_registers.r14);
        LOG("  r15: 0x%08lx\n", m_registers.r15);
        LOG("  rip: 0x%08lx\n", m_registers.rip);
        LOG("\n")
    }

private:
    registers_intel_x64_t m_registers;
    registers_intel_x64_t m_tmp_registers;
};

#endif

//=============================================================================
// End of file
//=============================================================================