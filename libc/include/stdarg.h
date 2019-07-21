/**
 * @file stdarg.h
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

#ifndef _LIBC_STDARG_H
#define _LIBC_STDARG_H

#include <_cheader.h>

_c_header_begin;

#include <va_list.h>

#if 1

#define va_arg(ap, l) __builtin_va_arg(ap, l)
#define va_end(ap) __builtin_va_end(ap)
#define va_start(ap, last_arg) __builtin_va_start((ap), (last_arg))

#else

#define STACKITEM int

#define VA_SIZE(TYPE) \
    ((sizeof(TYPE) + sizeof(STACKITEM) - 1) & ~(sizeof(STACKITEM) - 1))

/**
 * Begins a variable argument expansion
 */
#define va_start(AP, LASTARG) \
    (AP = ((va_list) & (LASTARG) + VA_SIZE(LASTARG)))

/**
 * Ends a variable argument expansion
 */
#define va_end(AP)

/**
 * Retrieves the next variable interpreted as TYPE
 */
#define va_arg(AP, TYPE) \
    (AP += VA_SIZE(TYPE), *((TYPE *)(AP - VA_SIZE(TYPE))))

#endif

_c_header_end;

#endif