/**
 * @file type_traits.hpp
 * @author Joakim Bertils
 * @version 0.1
 * @date 2020-07-28
 * 
 * @brief Type traits.
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

#ifndef __TYPE_TRAITS_HPP
#define __TYPE_TRAITS_HPP

#pragma GCC system_header

#include <bits/c++config.hpp>

OS6STD_BEGIN_SYSTEM_HEADER

namespace OS6STD
{
template <bool, typename>
struct __enable_if
{
};

template <typename _T>
struct __enable_if<true, _T>
{
    typedef _T __type;
};

template <bool _Cond, typename _Iftrue, typename _Iffalse>
struct __conditional_type
{
    typedef _Iftrue __type;
};

template <typename _Iftrue, typename _Iffalse>
struct __conditional_type<false, _Iftrue, _Iffalse>
{
    typedef _Iffalse __type;
};

// Given an integral builtin type, return the corresponding unsigned type.
template <typename _T>
struct __add_unsigned
{
private:
    typedef __enable_if<std::__is_integer<_T>::__value, _T> __if_type;

public:
    typedef typename __if_type::__type __type;
};

template <>
struct __add_unsigned<char>
{
    typedef unsigned char __type;
};

template <>
struct __add_unsigned<signed char>
{
    typedef unsigned char __type;
};

template <>
struct __add_unsigned<short>
{
    typedef unsigned short __type;
};

template <>
struct __add_unsigned<int>
{
    typedef unsigned int __type;
};

template <>
struct __add_unsigned<long>
{
    typedef unsigned long __type;
};

template <>
struct __add_unsigned<long long>
{
    typedef unsigned long long __type;
};

template <>
struct __add_unsigned<bool>;

template <>
struct __add_unsigned<wchar_t>;

template <typename _T>
struct __remove_unsigned
{
private:
    typedef __enable_if<std::__is_integer<_T>::__value, _T> __if_type;

public:
    typedef typename __if_type::__type __type;
};

template <>
struct __remove_unsigned<char>
{
    typedef signed char __type;
};

template <>
struct __remove_unsigned<unsigned char>
{
    typedef signed char __type;
};

template <>
struct __remove_unsigned<unsigned short>
{
    typedef short __type;
};

template <>
struct __remove_unsigned<unsigned int>
{
    typedef int __type;
};

template <>
struct __remove_unsigned<unsigned long>
{
    typedef long __type;
};

template <>
struct __remove_unsigned<unsigned long long>
{
    typedef long long __type;
};

template <>
struct __remove_unsigned<bool>;

template <>
struct __remove_unsigned<wchar_t>;

template <typename _Type>
inline bool __is_null_pointer(_Type *__ptr)
{
    return __ptr == 0;
}

template <typename _Type>
inline bool __is_null_pointer(_Type)
{
    return false;
}

inline bool __is_null_pointer(std::nullptr_t)
{
    return true;
}

template <typename _T, bool = std::__is_integer<_T>::__value>
struct __promote
{
    typedef double __type;
};

template <typename _T>
struct __promote<_T, false>
{
};

template <>
struct __promote<long double>
{
    typedef long double __type;
};

template <>
struct __promote<double>
{
    typedef double __type;
};

template <>
struct __promote<float>
{
    typedef float __type;
};

template <typename _T,
          typename _U,
          typename _T2 = typename __promote<_T>::__type,
          typename _U2 = typename __promote<_U>::__type>
struct __promote_2
{
    typedef __typeof__(_T2() + _U2()) __type;
};

template <typename _T,
          typename _U,
          typename _V,
          typename _T2 = typename __promote<_T>::__type,
          typename _U2 = typename __promote<_U>::__type,
          typename _V2 = typename __promote<_V>::__type>
struct __promote_3
{
    typedef __typeof__(_T2() + _U2() + _V2()) __type;
};

template <typename _T,
          typename _U,
          typename _V,
          typename _W,
          typename _T2 = typename __promote<_T>::__type,
          typename _U2 = typename __promote<_U>::__type,
          typename _V2 = typename __promote<_V>::__type,
          typename _W2 = typename __promote<_W>::__type>
struct __promote_4
{
    typedef __typeof__(_T2() + _U2() + _V2() + _W2()) __type;
};

}  // namespace OS6STD

OS6STD_END_SYSTEM_HEADER

#endif

//=============================================================================
// End of file
//=============================================================================
