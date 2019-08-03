/**
 * @file fpu.c
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-06-22
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

#include <arch/x86-64/fpu.h>

#include <string.h>

void arch_x64_64_set_fpu_cw(const uint16_t cw)
{
    __asm__ volatile("fldcw %0" ::"m"(cw));
}

void arch_x64_64_get_fpu_cw(uint16_t *cw)
{
    __asm__ volatile("fstcw %0"
                     : "=m"(*cw));
}

void arch_x86_64_set_rounding_mode(uint16_t mode)
{
    union {
        uint16_t cwi;
        arch_x86_64_fpu_cw_t cw;
    } u;

    if (mode > 3)
    {
        return;
    }

    arch_x64_64_get_fpu_cw(&u.cwi);

    u.cw.rc = mode;

    arch_x64_64_set_fpu_cw(u.cwi);
}

uint16_t arch_x86_64_get_rounding_mode()
{
    union {
        uint16_t cwi;
        arch_x86_64_fpu_cw_t cw;
    } u;

    arch_x64_64_get_fpu_cw(&u.cwi);

    return u.cw.rc;
}

void arch_x64_64_enable_fpu()
{
    __asm__ volatile("clts");

    size_t t;

    __asm__ volatile("mov %%cr0, %0"
                     : "=r"(t));

    t &= ~(1 << 2);
    t |= (1 << 1);

    __asm__ volatile("mov %0, %%cr0" ::"r"(t));

    __asm__ volatile("mov %%cr4, %0"
                     : "=r"(t));

    t |= 3 << 9;

    __asm__ volatile("mov %0, %%cr4" ::"r"(t));
}

void arch_x64_64_disable_fpu()
{
    size_t t;

    __asm__ volatile("mov %%cr0, %0"
                     : "=r"(t));

    t |= (1 << 3);

    __asm__ volatile("mov %0, %%cr0" ::"r"(t));
}

void arch_x64_64_init_fpu()
{
    __asm__ volatile("fninit");
}

void arch_x64_64_install_fpu()
{
    arch_x64_64_enable_fpu();
    arch_x64_64_init_fpu();
}

// This buffer must be at least 512 bytes large and
// the memory must be 16 byte aligned to avoid GP faults
static uint8_t __fxsave_buffer[512] __attribute__((aligned(16)));

void arch_x64_64_restore_fpu(void *buffer)
{
    memcpy(__fxsave_buffer, buffer, 512);

    __asm__ volatile("fxrstor (%0)" ::"r"(__fxsave_buffer));
}

void arch_x64_64_save_fpu(void *buffer)
{
    __asm__ volatile("fxsave (%0)" ::"r"(__fxsave_buffer));

    memcpy(buffer, __fxsave_buffer, 512);
}