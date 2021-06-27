/**
 * @file apic.h
 * @author Joakim Bertils
 * @version 0.1
 * @date 2021-06-27
 *
 * @brief
 *
 * @copyright Copyright (C) 2021,
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

#ifndef _ARCH_X86_64_APIC_H
#define _ARCH_X86_64_APIC_H

#include <arch/arch.h>

#include <stdint.h>

typedef enum
{
    LAPIC_REG_ID = 0x20,
    LAPIC_REG_VERSION = 0x30,
    LAPIC_REG_TPR = 0x80,
    LAPIC_REG_APR = 0x90,
    LAPIC_REG_PPR = 0xA0,
    LAPIC_REG_EOI = 0xB0,
    LAPIC_REG_SPURIOUS = 0xF0,
    LAPIC_REG_RRD = 0xC0,
    LAPIC_REG_ICR0 = 0x300,
    LAPIC_REG_ICR1 = 0x310,
    LAPIC_REG_LVT_TIMER = 0x320,
    LAPIC_REG_TIMER_INITCNT = 0x380,
    LAPIC_REG_TIMER_CURRCNT = 0x390,
    LAPIC_REG_TIMER_DIV = 0x3e0,

} lapic_reg_t;

#define LAPIC_ENABLE 0x800

void apic_initialize();
void apic_eoi();
uint8_t apic_current_processor_id();
void apic_enable();

#endif

//=============================================================================
// End of file
//=============================================================================
