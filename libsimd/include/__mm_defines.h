/**
 * @file __mm_defines.h
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-07-01
 * 
 * @brief Simd preprocessor definitions
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

#ifndef _LIBSIMD___MM_DEFINES_H
#define _LIBSIMD___MM_DEFINES_H

//=============================================================================
// Preprocessor macros
//=============================================================================

#define __ATTR(x) __attribute__(x)
#define __VSIZE(x) __vector_size__(x)
#define __ALIAS __may_alias__
#define __ALIGN(x) __aligned__(x)
#define __inline inline
#define __always_inline(x) extern __inline x __attribute__((__gnu_inline__, __always_inline__, __artificial__))

#endif

//=============================================================================
// End of file
//=============================================================================