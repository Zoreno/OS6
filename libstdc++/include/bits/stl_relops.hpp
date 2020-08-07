/**
 * @file stl_relops.hpp
 * @author Joakim Bertils
 * @version 0.1
 * @date 2020-07-28
 * 
 * @brief Generic relational operators
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

#ifndef __STL_RELOPS_HPP
#define __STL_RELOPS_HPP

#pragma GCC system_header

#include <bits/c++config.hpp>

OS6STD_BEGIN_SYSTEM_HEADER

namespace OS6STD
{
namespace rel_ops
{
template <class _T>
inline bool operator!=(const _T &__x, const _T &__y)
{
    return !(__x == __y);
}

template <class _T>
inline bool operator>(const _T &__x, const _T &__y)
{
    return (__y < __x);
}

template <class _T>
inline bool operator<=(const _T &__x, const _T &__y)
{
    return !(__y < __x);
}

template <class _T>
inline bool operator>=(const _T &__x, const _T &__y)
{
    return !(__x < __y);
}

} // namespace rel_ops
} // namespace OS6STD

OS6STD_END_SYSTEM_HEADER

#endif

//=============================================================================
// End of file
//=============================================================================