/**
 * @file stl_function.hpp
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

#ifndef __STL_FUNCTION_HPP
#define __STL_FUNCTION_HPP

#pragma GCC system_header

#include <initializer_list>

OS6STD_BEGIN_SYSTEM_HEADER

namespace OS6STD
{
template <typename _Arg, typename _Result>
struct unary_function
{
    using argument_type = _Arg;
    using result_type = _Result;
};

template <typename _Arg1, typename _Arg2, typename _Result>
struct binary_function
{
    using first_argument_type = _Arg1;
    using second_argument_type = _Arg2;
    using result_type = _Result;
};

template <typename _T>
struct less : public binary_function<_T, _T, bool>
{
    constexpr bool operator()(const _T &__x, const _T &__y) const
    {
        return __x < __y;
    }
};
}  // namespace OS6STD

OS6STD_END_SYSTEM_HEADER

#endif

//==============================================================================
// End of file
//==============================================================================
