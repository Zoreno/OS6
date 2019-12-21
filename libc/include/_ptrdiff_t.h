/** \addtogroup libc 
 *  @{
 */

/**
 * @file libc/include/_ptrdiff_t.h
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-04-25
 * 
 * @brief Defines a type that can be used to compare pointers
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

#ifndef _LIBC__PTRDIFF_T_H
#define _LIBC__PTRDIFF_T_H

#include <_cheader.h>

_c_header_begin;

/**
 * @brief Pointer difference type
 * 
 * 
 */
typedef long long ptrdiff_t;

_c_header_end;

#endif

/** @}*/

//=============================================================================
// End of file
//=============================================================================