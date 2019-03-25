#ifndef _LIBK_STDIO_H
#define _LIBK_STDIO_H

#include <_size_t.h>
#include <_null.h>
#include <_file.h>

#include <stdarg.h>

extern FILE *stdout;

void set_stdout(FILE *file);

int fputc(char c, FILE *file);
int fputs(const char *str, FILE *file);
int putchar(char c);
int puts(const char *str);
int fprintf(FILE *file, const char *format, ...);
int printf(const char *format, ...);
int sprintf(char *buf, const char *format, ...);
int vsprintf(char *str, const char *format, va_list arg);

#endif