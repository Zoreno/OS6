/**
 * @file stddef.h
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-04-25
 * 
 * @brief Defines the standard types
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

#ifndef _LIBC_STDDEF_H
#define _LIBC_STDDEF_H

#include <_cheader.h>

_c_header_begin;

#include <_size_t.h>
#include <_ptrdiff_t.h>
#include <_null.h>
#include <_wchar_t.h>
#include <_wint_t.h>

#define offsetof(type, member) ((size_t)((char *)&(((type *)0)->member) - (char *)0))

_c_header_end;

#endif