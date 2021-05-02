/**
 * @file pit.h
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

#ifndef _ARCH_X86_64_PIT_H
#define _ARCH_X86_64_PIT_H

#include <arch/arch.h>
#include <stddef.h>
#include <stdint.h>

#define ARCH_X86_64_PIT_OCW_MASK_BINCOUNT 1
#define ARCH_X86_64_PIT_OCW_MASK_MODE 0xE
#define ARCH_X86_64_PIT_OCW_MASK_RL 0x30
#define ARCH_X86_64_PIT_OCW_MASK_COUNTER 0xC0

#define ARCH_X86_64_PIT_OCW_BINCOUNT_BINARY 0
#define ARCH_X86_64_PIT_OCW_BINCOUNT_BCD 1

#define ARCH_X86_64_PIT_OCW_MODE_TERMINALCOUNT 0
#define ARCH_X86_64_PIT_OCW_MODE_ONESHOT 0x2
#define ARCH_X86_64_PIT_OCW_MODE_RATEGEN 0x4
#define ARCH_X86_64_PIT_OCW_MODE_SQUAREWAVEGEN 0x6
#define ARCH_X86_64_PIT_OCW_MODE_SOFTWARETRIG 0x8
#define ARCH_X86_64_PIT_OCW_MODE_HARDWARETRIG 0xA

#define ARCH_X86_64_PIT_OCW_RL_LATCH 0
#define ARCH_X86_64_PIT_OCW_RL_LSBONLY 0x10
#define ARCH_X86_64_PIT_OCW_RL_MSBONLY 0x20
#define ARCH_X86_64_PIT_OCW_RL_DATA 0x30

#define ARCH_X86_64_PIT_OCW_COUNTER_0 0
#define ARCH_X86_64_PIT_OCW_COUNTER_1 0x40
#define ARCH_X86_64_PIT_OCW_COUNTER_2 0x80

void arch_x86_64_set_on_tick_handler(on_tick_handler_func on_tick_handler);
void arch_x86_64_pit_send_command(uint8_t cmd);
void arch_x86_64_pit_send_data(uint16_t counter, uint8_t data);
uint8_t arch_x86_64_pit_read_data(uint16_t counter);
tick_count_t arch_x86_64_pit_set_tick_count(tick_count_t i);
tick_count_t arch_x86_64_pit_get_tick_count();
void arch_x86_64_pit_start_counter(uint32_t freq,
                                   uint8_t counter,
                                   uint8_t mode);
void arch_x86_64_initialize_pit();
int arch_X86_64_pit_is_initialzed();

#endif

//=============================================================================
// End of file
//=============================================================================
