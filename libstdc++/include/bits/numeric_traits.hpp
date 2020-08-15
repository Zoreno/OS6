/**
 * @file numeric_traits.hpp
 * @author Joakim Bertils
 * @version 0.1
 * @date 2020-07-28
 * 
 * @brief Numeric traits
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

#ifndef __NUMERIC_TRAITS_HPP
#define __NUMERIC_TRAITS_HPP

#pragma GCC system_header

#include <bits/c++config.hpp>

#include <bits/cpp_type_traits.hpp>
#include <bits/type_traits.hpp>

OS6STD_BEGIN_SYSTEM_HEADER

namespace OS6STD
{
#define __signed(_Tp) ((_Tp)(-1) < 0)
#define __digits(_Tp) (sizeof(_Tp) * __CHAR_BIT__ - __signed(_Tp))
#define __min(_Tp) (__signed(_Tp) ? (_Tp)1 << __digits(_Tp) : (_Tp)0)
#define __max(_Tp) (__signed(_Tp) ? (((((_Tp)1 << (__digits(_Tp) - 1)) - 1) << 1) + 1) : ~(_Tp)0)

template <typename _Value>
struct __numeric_traits_integer
{
    static const _Value __min = __min(_Value);
    static const _Value __max = __max(_Value);
    static const bool __is_signed = __signed(_Value);
    static const int __digits = __digits(_Value);
};

template <typename _Value>
const _Value __numeric_traits_integer<_Value>::__min;

template <typename _Value>
const _Value __numeric_traits_integer<_Value>::__max;

template <typename _Value>
const bool __numeric_traits_integer<_Value>::__is_signed;

template <typename _Value>
const int __numeric_traits_integer<_Value>::__digits;

#undef __signed
#undef __digits
#undef __min
#undef __max

#define __floating(_Tp, _Fval, _Dval, _LDval)     \
    (std::__are_same<_Tp, float>::__value ? _Fval \
                                          : std::__are_same<_Tp, double>::__value ? _Dval : _LDval)

#define __max_digits10(_Tp)                                  \
    (2 + __floating(_Tp, __FLT_MANT_DIG__, __DBL_MANT_DIG__, \
                    __LDBL_MANT_DIG__) *                     \
             643L / 2136)

#define __digits10(_Tp) \
    __floating(_Tp, __FLT_DIG__, __DBL_DIG__, __LDBL_DIG__)

#define __max_exponent10(_Tp)                               \
    __floating(_Tp, __FLT_MAX_10_EXP__, __DBL_MAX_10_EXP__, \
               __LDBL_MAX_10_EXP__)

template <typename _Value>
struct __numeric_traits_floating
{
    static const int __max_digits10 = __max_digits10(_Value);
    static const bool __is_signed = true;
    static const int __digits10 = __digits10(_Value);
    static const int __max_exponent10 = __max_exponent10(_Value);
};

template <typename _Value>
const int __numeric_traits_floating<_Value>::__max_digits10;

template <typename _Value>
const bool __numeric_traits_floating<_Value>::__is_signed;

template <typename _Value>
const int __numeric_traits_floating<_Value>::__digits10;

template <typename _Value>
const int __numeric_traits_floating<_Value>::__max_exponent10;

template <typename _Value>
struct __numeric_traits
    : public __conditional_type<std::__is_integer<_Value>::__value,
                                __numeric_traits_integer<_Value>,
                                __numeric_traits_floating<_Value>>::__type
{
};

#undef __floating
#undef __max_digits10
#undef __digits10
#undef __max_exponent10

}  // namespace OS6STD

OS6STD_END_SYSTEM_HEADER

#endif

//=============================================================================
// End of file
//=============================================================================
