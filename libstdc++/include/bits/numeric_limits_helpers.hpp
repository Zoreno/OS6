/**
 * @file numeric_limits_helpers.hpp
 * @author Joakim Bertils
 * @version 0.1
 * @date 2020-07-28
 * 
 * @brief Numeric limits helper macros
 * 
 * @copyright Copyright (C) 2020,
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

#ifndef __NUMERIC_LIMITS_HELPERS_HPP
#define __NUMERIC_LIMITS_HELPERS_HPP

#pragma GCC system_header

#include <bits/c++config.hpp>

// Implementation borrowed from GCC
// TODO: Replace with own implementation
#define __signed_b(T, B) ((T)(-1) < 0)
#define __min_b(T, B) (__signed_b(T, B) ? -__max_b(T, B) - 1 : (T)0)
#define __max_b(T, B) (__signed_b(T, B) ? (((((T)1 << (__digits_b(T, B) - 1)) - 1) << 1) + 1) : ~(T)0)
#define __digits_b(T, B) (B - __signed_b(T, B))
#define __digits10_b(T, B) (__digits_b(T, B) * 643L / 2136)
#define __signed(T) __signed_b(T, sizeof(T) * __CHAR_BIT__)
#define __min(T) __min_b(T, sizeof(T) * __CHAR_BIT__)
#define __max(T) __max_b(T, sizeof(T) * __CHAR_BIT__)
#define __digits(T) __digits_b(T, sizeof(T) * __CHAR_BIT__)
#define __digits10(T) __digits10_b(T, sizeof(T) * __CHAR_BIT__)
#define __max_digits10(T) (2 + (T)*643L / 2136)

#endif

//=============================================================================
// End of file
//=============================================================================
