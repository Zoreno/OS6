/**
 * @file _syscall.h
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-04-25
 * 
 * @brief Defines helpers for performing system calls
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

#ifndef _LIBC_SYSCALL_H
#define _LIBC_SYSCALL_H

#include <_cheader.h>

_c_header_begin;

#include <stdint.h>

#define SYSCALL_INTNO 0x80

#define SYSCALL_DEBUG_PRINT 0
#define SYSCALL_EXIT 1
#define SYSCALL_YIELD 2
#define SYSCALL_SLEEP 3

#define SYSCALL_FORK 4
#define SYSCALL_CLONE 5
#define SYSCALL_KILL 6

#define SYSCALL_OPEN 7
#define SYSCALL_CLOSE 8
#define SYSCALL_READ 9
#define SYSCALL_WRITE 10
#define SYSCALL_SEEK 11
#define SYSCALL_READDIR 12
#define SYSCALL_ACCESS 13
#define SYSCALL_IOCTL 14
#define SYSCALL_STATF 15
#define SYSCALL_STAT 16
#define SYSCALL_MKDIR 17
#define SYSCALL_UNLINK 18
#define SYSCALL_SYMLINK 19
#define SYSCALL_READLINK 20
#define SYSCALL_LSTAT 21

#define SYSCALL_CHMOD 22
#define SYSCALL_CHOWN 23

#define SYSCALL_SBRK 24

#define SYSCALL_GETPID 25
#define SYSCALL_GETTID 26

#define SYSCALL_CHDIR 27
#define SYSCALL_GETCWD 28

int64_t do_syscall0(int64_t syscall);
int64_t do_syscall1(int64_t syscall, int64_t arg1);
int64_t do_syscall2(int64_t syscall, int64_t arg1, int64_t arg2);
int64_t do_syscall3(int64_t syscall, int64_t arg1, int64_t arg2, int64_t arg3);

_c_header_end;

#endif