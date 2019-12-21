/** \addtogroup libk 
 *  @{
 */

/**
 * @file libk/include/va_list.h
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-04-25
 * 
 * @brief Variable argument list
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

#ifndef _LIBK_VA_LIST_H
#define _LIBK_VA_LIST_H

#include <__libk_common.h>

// TODO: This should depend on the arch. x86 may use custom built.
#if 1

typedef __builtin_va_list va_list;

#else

typedef unsigned char *va_list;

#endif

#endif

/** @}*/

//==============================================================================
// End of file
//==============================================================================