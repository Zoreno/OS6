/**
 * @file pic.h
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

#ifndef _ARCH_X86_64_PIC_H
#define _ARCH_X86_64_PIC_H

#include <stdint.h>

#define PIC_IRQ_TIMER 0
#define PIC_IRQ_KEYBOARD 1
#define PIC_IRQ_SERIAL2 3
#define PIC_IRQ_SERIAL1 4
#define PIC_IRQ_PARALLEL2 5
#define PIC_IRQ_DISKETTE 6
#define PIC_IRQ_PARALLEL1 7

#define PIC_IRQ_CMOSTIMER 0
#define PIC_IRQ_CGARETRACE 1
#define PIC_IRQ_AUXILLARY 4
#define PIC_IRQ_FPU 5
#define PIC_IRQ_HDC 6

#define PIC_OCW2_MASK_L1 1
#define PIC_OCW2_MASK_L2 2
#define PIC_OCW2_MASK_L3 4
#define PIC_OCW2_MASK_EOI 0x20
#define PIC_OCW2_MASK_SL 0x40
#define PIC_OCW2_MASK_ROTATE 0x80

#define PIC_OCW3_MASK_RIS 1
#define PIC_OCW3_MASK_RIR 2
#define PIC_OCW3_MASK_MODE 4
#define PIC_OCW3_MASK_SMM 0x20
#define PIC_OCW3_MASK_ESMM 0x40
#define PIC_OCW3_MASK_D7 0x80

uint8_t arch_x86_64_pic_read_data(uint8_t pic_num);
void arch_x86_64_pic_send_data(uint8_t data, uint8_t pic_num);
void arch_x86_64_pic_send_command(uint8_t command, uint8_t pic_num);
void arch_x86_64_pic_set_mask_interrupt(uint8_t irq);
void arch_x86_64_pic_clear_mask_interrupt(uint8_t irq);
void arch_x86_64_initialize_pic(uint8_t base0, uint8_t base1);

#endif

//=============================================================================
// End of file
//=============================================================================
