/**
 * @file range_access.hpp
 * @author Joakim Bertils
 * @version 0.1
 * @date 2020-07-29
 * 
 * @brief Generic range access functions
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

#ifndef __RANGE_ACCESS_HPP
#define __RANGE_ACCESS_HPP

#pragma GCC system_header

#include <bits/c++config.hpp>

#if __cplusplus >= 201103L

#if __cplusplus >= 201402L
#define CONSTEXPR constexpr
#else
#define CONSTEXPR
#endif

#include <initializer_list>

OS6STD_BEGIN_SYSTEM_HEADER

namespace OS6STD
{

template <typename _C>
inline CONSTEXPR auto begin(_C &__cont) -> decltype(__cont.begin())
{
    return __cont.begin();
}

template <typename _C>
inline CONSTEXPR auto begin(const _C &__cont) -> decltype(__cont.begin())
{
    return __cont.begin();
}

template <typename _C>
inline CONSTEXPR auto end(_C &__cont) -> decltype(__cont.end())
{
    return __cont.end();
}

template <typename _C>
inline CONSTEXPR auto end(const _C &__cont) -> decltype(__cont.end())
{
    return __cont.end();
}

template <typename _T, size_t _Size>
inline CONSTEXPR _T *begin(_T (&__arr)[_Size])
{
    return __arr;
}

template <typename _T, size_t _Size>
inline CONSTEXPR _T *
    end(_T (&__arr)[_Size])
{
    return __arr + _Size;
}

} // namespace OS6STD

OS6STD_END_SYSTEM_HEADER

#endif

#endif