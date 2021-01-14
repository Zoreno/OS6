/**
 * @file _mbstate_t.h
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

#ifndef _LIBC__MBSTATE_T_H
#define _LIBC__MBSTATE_T_H

#include <_cheader.h>
#include <_wint_t.h>

_c_header_begin;

/**
 * @brief Type representing the multichar conversion state
 * 
 * 
 */
typedef struct
{
    int __count;
    union
    {
        wint_t __wch;
        unsigned char __wchb[4];
    } __value;
} _mbstate_t;

_c_header_end;

#endif

//=============================================================================
// End of file
//=============================================================================