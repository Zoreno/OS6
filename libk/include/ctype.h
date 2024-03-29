/**
 * @file ctype.h
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

#ifndef _LIBK_CTYPE_H
#define _LIBK_CTYPE_H

#include <__libk_common.h>

#include <sys/types.h>

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

int isalnum_l(char c, locale_t l);
int isalpha_l(char c, locale_t l);
int iscntrl_l(char c, locale_t l);
int isgraph_l(char c, locale_t l);
int isdigit_l(char c, locale_t l);
int islower_l(char c, locale_t l);
int isprint_l(char c, locale_t l);
int ispunct_l(char c, locale_t l);
int isspace_l(char c, locale_t l);
int isupper_l(char c, locale_t l);
int isxdigit_l(char c, locale_t l);

char tolower_l(char c, locale_t l);
char toupper_l(char c, locale_t l);

int isascii_l(char c, locale_t l);
char toascii_l(char c, locale_t l);

#endif

//==============================================================================
// End of file
//==============================================================================
