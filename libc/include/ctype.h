/** \addtogroup libc 
 *  @{
 */

/**
 * @file libc/include/ctype.h
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

#ifndef _LIBC_CTYPE_H
#define _LIBC_CTYPE_H

#include <_cheader.h>

_c_header_begin;

int isalnum(char c);
int isalpha(char c);
int iscntrl(char c);
int isgraph(char c);
int isdigit(char c);
int islower(char c);
int isprint(char c);
int ispunct(char c);
int isspace(char c);
int isupper(char c);
int isxdigit(char c);

char tolower(char c);
char toupper(char c);

int isascii(char c);
char toascii(char c);

_c_header_end;

#endif

/** @}*/

//=============================================================================
// End of file
//=============================================================================