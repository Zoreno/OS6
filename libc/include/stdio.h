/**
 * @file stdio.h
 * @author Joakim Bertils
 * @version 0.1
 * @date 2020-12-31
 * 
 * @brief 
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

#ifndef _LIBC_STDIO_H
#define _LIBC_STDIO_H

#include <_cheader.h>

_c_header_begin;

#include <stdarg.h>

#define EOF (-1)

// TODO: Implement with syscall
int puts(const char *s);

int printf(const char *format, ...);
int vsprintf(char *str, const char *format, va_list arg);

_c_header_end;

#endif