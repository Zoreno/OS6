/**
 * @file fpu.h
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-04-26
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

#ifndef _ARCH_X86_64_FPU_H
#define _ARCH_X86_64_FPU_H

#include <stdint.h>

// TODO: This should probably not ne included here
#include <process/process.h>

#define FPU_ROUND_NEAREST 0
#define FPU_ROUND_DOWN 1
#define FPU_ROUND_UP 2
#define FPU_ROUND_TRUNCATE 3

typedef struct
{
    uint16_t im : 1;
    uint16_t dm : 1;
    uint16_t zm : 1;
    uint16_t om : 1;
    uint16_t um : 1;
    uint16_t pm : 1;
    uint16_t : 1;
    uint16_t iem : 1;
    uint16_t pc : 2;
    uint16_t rc : 2;
    uint16_t ic : 1;
    uint16_t : 3;
} __attribute__((packed)) arch_x86_64_fpu_cw_t;

void arch_x64_64_set_fpu_cw(const uint16_t cw);
void arch_x64_64_get_fpu_cw(uint16_t *cw);
void arch_x86_64_set_rounding_mode(uint16_t mode);
uint16_t arch_x86_64_get_rounding_mode();
void arch_x64_64_enable_fpu();
void arch_x64_64_disable_fpu();
void arch_x64_64_init_fpu();
void arch_x64_64_install_fpu();
void arch_x64_64_restore_fpu(process_t *process);
void arch_x64_64_save_fpu(process_t *process);

#endif