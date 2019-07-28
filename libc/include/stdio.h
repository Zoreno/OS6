#ifndef _LIBC_STDIO_H
#define _LIBC_STDIO_H

#include <_cheader.h>

_c_header_begin;

#include <stdarg.h>

// TODO: Implement with syscall
int puts(const char *s);

int printf(const char *format, ...);
int vsprintf(char *str, const char *format, va_list arg);

_c_header_end;

#endif