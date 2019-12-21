/** \addtogroup libk 
 *  @{
 */

/**
 * @file libk/include/__libk_symbols.h
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-07-01
 * 
 * @brief Defines macros for symbol aliases
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

#ifndef ___LIBK_SYMBOLS_H
#define ___LIBK_SYMBOLS_H

#define __typeof(x) __typeof__(x)

#define weak_alias(name, aliasname) _weak_alias(name, aliasname)
#ifdef _LIBK
#define _weak_alias(name, aliasname) \
	extern __typeof(name) aliasname __attribute__((weak, alias(#name)));
#else
#define _weak_alias(name, aliasame)
#endif

#endif

/** @}*/

//==============================================================================
// End of file
//==============================================================================
