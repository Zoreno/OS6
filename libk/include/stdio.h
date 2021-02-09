/**
 * @file stdio.h
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-04-27
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

#ifndef _LIBK_STDIO_H
#define _LIBK_STDIO_H

#include <__libk_common.h>

#include <sys/types.h>

#include <stdarg.h>

extern FILE *stdout;

#define EOF (-1)

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

void set_stdout(FILE *file);

int fputc(char c, FILE *file);
int fputs(const char *str, FILE *file);
int putchar(char c);
int puts(const char *str);
int fprintf(FILE *file, const char *format, ...); //__attribute__((format(printf, 2, 3)));
int printf(const char *format, ...);              //__attribute__((format(printf, 1, 2)));
int sprintf(char *buf, const char *format, ...);  //__attribute__((format(printf, 2, 3)));
int vsprintf(char *str, const char *format, va_list arg);

char getc();
char getchar();
char fgetc(FILE *file);
char *fgets(char *s, int size, FILE *file);

int feof(FILE *file);

#endif

//=============================================================================
// End of file
//=============================================================================