/**
 * @file cpp_type_traits.hpp
 * @author Joakim Bertils
 * @version 0.1
 * @date 2020-07-29
 * 
 * @brief Simple type traits
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

#ifndef __CPP_TYPE_TRAITS_HPP
#define __CPP_TYPE_TRAITS_HPP

#pragma GCC system_header

#include <bits/c++config.hpp>

OS6STD_BEGIN_SYSTEM_HEADER

namespace OS6STD
{
struct __true_type
{
};
struct __false_type
{
};

template <bool>
struct __truth_type
{
    typedef __false_type __type;
};

template <>
struct __truth_type<true>
{
    typedef __true_type __type;
};

template <class _S, class _T>
struct __traitor
{
    enum
    {
        __value = bool(_S::__value) || bool(_T::__value)
    };
    typedef typename __truth_type<__value>::__type __type;
};

template <typename, typename>
struct __are_same
{
    enum
    {
        __value = 0
    };
    typedef __false_type __type;
};

template <typename _T>
struct __are_same<_T, _T>
{
    enum
    {
        __value = 1
    };
    typedef __true_type __type;
};

template <typename _T>
struct __is_void
{
    enum
    {
        __value = 0
    };
    typedef __false_type __type;
};

template <>
struct __is_void<void>
{
    enum
    {
        __value = 1
    };
    typedef __true_type __type;
};

template <typename _T>
struct __is_integer
{
    enum
    {
        __value = 0
    };
    typedef __false_type __type;
};

template <>
struct __is_integer<bool>
{
    enum
    {
        __value = 1
    };
    typedef __true_type __type;
};

template <>
struct __is_integer<char>
{
    enum
    {
        __value = 1
    };
    typedef __true_type __type;
};

template <>
struct __is_integer<signed char>
{
    enum
    {
        __value = 1
    };
    typedef __true_type __type;
};

template <>
struct __is_integer<unsigned char>
{
    enum
    {
        __value = 1
    };
    typedef __true_type __type;
};

template <>
struct __is_integer<char16_t>
{
    enum
    {
        __value = 1
    };
    typedef __true_type __type;
};

template <>
struct __is_integer<char32_t>
{
    enum
    {
        __value = 1
    };
    typedef __true_type __type;
};

template <>
struct __is_integer<short>
{
    enum
    {
        __value = 1
    };
    typedef __true_type __type;
};

template <>
struct __is_integer<unsigned short>
{
    enum
    {
        __value = 1
    };
    typedef __true_type __type;
};

template <>
struct __is_integer<int>
{
    enum
    {
        __value = 1
    };
    typedef __true_type __type;
};

template <>
struct __is_integer<unsigned int>
{
    enum
    {
        __value = 1
    };
    typedef __true_type __type;
};

template <>
struct __is_integer<long>
{
    enum
    {
        __value = 1
    };
    typedef __true_type __type;
};

template <>
struct __is_integer<unsigned long>
{
    enum
    {
        __value = 1
    };
    typedef __true_type __type;
};

template <>
struct __is_integer<long long>
{
    enum
    {
        __value = 1
    };
    typedef __true_type __type;
};

template <>
struct __is_integer<unsigned long long>
{
    enum
    {
        __value = 1
    };
    typedef __true_type __type;
};

template <typename _T>
struct __is_floating
{
    enum
    {
        __value = 0
    };
    typedef __false_type __type;
};

template <>
struct __is_floating<float>
{
    enum
    {
        __value = 1
    };
    typedef __true_type __type;
};

template <>
struct __is_floating<double>
{
    enum
    {
        __value = 1
    };
    typedef __true_type __type;
};

template <>
struct __is_floating<long double>
{
    enum
    {
        __value = 1
    };
    typedef __true_type __type;
};

template <typename _T>
struct __is_pointer
{
    enum
    {
        __value = 0
    };
    typedef __false_type __type;
};

template <typename _T>
struct __is_pointer<_T *>
{
    enum
    {
        __value = 1
    };
    typedef __true_type __type;
};

template <typename _T>
struct __is_arithmetic
    : public __traitor<__is_integer<_T>, __is_floating<_T>>{};

template <typename _T>
struct __is_scalar
    : public __traitor<__is_arithmetic<_T>, __is_pointer<_T>>{};

template <typename _T>
struct __is_char
{
    enum
    {
        __value = 0
    };
    typedef __false_type __type;
};

template <>
struct __is_char<char>
{
    enum
    {
        __value = 1
    };
    typedef __true_type __type;
};

template <typename _T>
struct __is_byte
{
    enum
    {
        __value = 0
    };
    typedef __false_type __type;
};

template <>
struct __is_byte<char>
{
    enum
    {
        __value = 1
    };
    typedef __true_type __type;
};

template <>
struct __is_byte<signed char>
{
    enum
    {
        __value = 1
    };
    typedef __true_type __type;
};

template <>
struct __is_byte<unsigned char>
{
    enum
    {
        __value = 1
    };
    typedef __true_type __type;
};

#if __cplusplus >= 201703L
enum class byte : unsigned char;

template <>
struct __is_byte<byte>
{
    enum
    {
        __value = 1
    };
    typedef __true_type __type;
};
#endif  // C++17

template <typename _T>
struct __is_move_iterator
{
    enum
    {
        __value = 0
    };
    typedef __false_type __type;
};

template <typename _Iterator>
inline _Iterator __miter_base(_Iterator __it)
{
    return __it;
}

}  // namespace OS6STD

OS6STD_END_SYSTEM_HEADER

#endif
