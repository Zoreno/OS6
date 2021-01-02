/**
 * @file atomic.h
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-06-21
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

#ifndef _ARCH_X86_64_ATOMIC_H
#define _ARCH_X86_64_ATOMIC_H

int arch_x86_64_atomic_swap(volatile long long int *x, long long int v);
void arch_x86_64_atomic_store(volatile long long int *p, long long int x);
void arch_x86_64_atomic_inc(volatile long long int *x);
void arch_x86_64_atomic_dec(volatile long long int *x);

#endif

//=============================================================================
// End of file
//=============================================================================