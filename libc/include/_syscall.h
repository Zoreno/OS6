#ifndef _LIBC_SYSCALL_H
#define _LIBC_SYSCALL_H

#include <_cheader.h>

_c_header_begin;

#include <stdint.h>

int64_t do_syscall0(int64_t syscall);
int64_t do_syscall1(int64_t syscall, int64_t arg1);
int64_t do_syscall2(int64_t syscall, int64_t arg1, int64_t arg2);
int64_t do_syscall3(int64_t syscall, int64_t arg1, int64_t arg2, int64_t arg3);

_c_header_end;

#endif