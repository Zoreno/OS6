/**
 * @file do_syscall.c
 * @author Joakim Bertils
 * @version 0.1
 * @date 2020-08-08
 *
 * @brief Contains helpers for calling system call functions by interrupt.
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

#include <syscall/syscall.h>

int64_t do_syscall0(int64_t syscall)
{
    int64_t ret;

    __asm__ volatile("int $0x80" : "=a"(ret) : "a"(syscall) : "memory");

    return ret;
}

int64_t do_syscall1(int64_t syscall, int64_t arg1)
{
    int64_t ret;

    __asm__ volatile("int $0x80"
                     : "=a"(ret)
                     : "a"(syscall), "b"(arg1)
                     : "memory");

    return ret;
}

int64_t do_syscall2(int64_t syscall, int64_t arg1, int64_t arg2)
{
    int64_t ret;

    __asm__ volatile("int $0x80"
                     : "=a"(ret)
                     : "a"(syscall), "b"(arg1), "c"(arg2)
                     : "memory");

    return ret;
}

int64_t do_syscall3(int64_t syscall, int64_t arg1, int64_t arg2, int64_t arg3)
{
    int64_t ret;

    __asm__ volatile("int $0x80"
                     : "=a"(ret)
                     : "a"(syscall), "b"(arg1), "c"(arg2), "d"(arg3)
                     : "memory");

    return ret;
}

//=============================================================================
// End of file
//=============================================================================
