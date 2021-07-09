/**
 * @file msr.c
 * @author Joakim Bertils
 * @version 0.1
 * @date 2021-07-09
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

#include <arch/x86-64/msr.h>

uint64_t rdmsr(msr_registers_t msr)
{
    uint32_t low;
    uint32_t high;

    __asm__ volatile("rdmsr" : "=a"(low), "=d"(high) : "c"((uint64_t)msr));

    return ((uint64_t)high << 32) | low;
}

void wrmsr(msr_registers_t msr, uint64_t value)
{
    uint32_t low = value & 0xFFFFFFFF;
    uint32_t high = value >> 32;

    __asm__ volatile("wrmsr" ::"c"((uint64_t)msr), "a"(low), "d"(high));
}

//=============================================================================
// End of file
//=============================================================================
