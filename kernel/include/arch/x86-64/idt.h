/**
 * @file idt.h
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

#ifndef _ARCH_X86_64_IDT_H
#define _ARCH_X86_64_IDT_H

#include <arch/arch.h>

#include <stdint.h>

#define ARCH_X86_64_IDT_MAX_INTERRUPTS 256

#define ARCH_X86_64_IDT_DESC_BIT16 0x06
#define ARCH_X86_64_IDT_DESC_BIT32 0x0E

#define ARCH_X86_64_IDT_DESC_RING1 0x40
#define ARCH_X86_64_IDT_DESC_RING2 0x20
#define ARCH_X86_64_IDT_DESC_RING3 0x60

#define ARCH_X86_64_IDT_DESC_PRESENT 0x80

typedef struct
{
    uint16_t baseLo;
    uint16_t sel;
    uint8_t res;
    uint8_t flags;
    uint16_t baseMid;
    uint32_t baseHi;
    uint32_t res2;
} __attribute__((packed)) arch_x86_64_idt_descriptor;

arch_x86_64_idt_descriptor *arch_x86_64_get_ir(uint32_t i);

void arch_x86_64_install_ir(uint32_t i,
                            uint16_t flags,
                            uint16_t sel,
                            INT_HANDLER int_hander);

void arch_x86_64_install_irq(int irq, IRQ_HANDLER irq_handler);

void arch_x86_64_initialize_idt(uint16_t code_sel);

#endif

//=============================================================================
// End of file
//=============================================================================
