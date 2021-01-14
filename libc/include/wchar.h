/**
 * @file wchar.h
 * @author Joakim Bertils
 * @version 0.1
 * @date 2021-01-10
 * 
 * @brief 
 * 
 * @copyright Copyright (C) 2021,
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

#ifndef _LIBC_WCHAR_H
#define _LIBC_WCHAR_H

#include <_cheader.h>
#include <_mbstate_t.h>
#include <_wint_t.h>

_c_header_begin;

typedef _mbstate_t mbstate_t;

#define WEOF ((wint_t)-1)

_c_header_end;

#endif

//=============================================================================
// End of file
//=============================================================================