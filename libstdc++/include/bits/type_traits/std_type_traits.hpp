/**
 * @file std_type_traits.hpp
 * @author Joakim Bertils
 * @version 0.1
 * @date 2020-07-28
 * 
 * @brief Template utilities for compile-time introspection and modification
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

#ifndef __STD_TYPE_TRAITS_HPP_
#define __STD_TYPE_TRAITS_HPP_

#pragma GCC system_header

#include <bits/c++config.hpp>

OS6STD_BEGIN_SYSTEM_HEADER

namespace OS6STD
{
//=============================================================================
// Integral constant
//=============================================================================

template <typename _T, _T __v>
struct integral_constant
{
    static constexpr _T value = __v;

    using value_type = _T;
    using type = integral_constant<_T, __v>;

    constexpr operator value_type() const noexcept
    {
        return value;
    }

    constexpr value_type operator()() const noexcept
    {
        return value;
    }
};

template <typename _T, _T __v>
constexpr _T integral_constant<_T, __v>::value;

//=============================================================================
// Boolean constants
//=============================================================================

using true_type = integral_constant<bool, true>;
using false_type = integral_constant<bool, false>;

template <bool __v>
using __bool_constant = integral_constant<bool, __v>;

template <bool __v>
using bool_constant = integral_constant<bool, __v>;

//=============================================================================
// Conditionals
//=============================================================================

template <bool, typename, typename>
struct conditional;

//=============================================================================
// Or helper types
//=============================================================================

template <typename...>
struct __or_;

template <>
struct __or_<> : public false_type
{
};

template <typename _B1, typename _B2>
struct __or_<_B1, _B2>
    : public conditional<_B1::value, _B1, _B2>::type
{
};

template <typename _B1, typename _B2, typename _B3, typename... _Bn>
struct __or_<_B1, _B2, _B3, _Bn...>
    : public conditional<_B1::value, _B1, __or_<_B2, _B3, _Bn...>>::type
{
};

//=============================================================================
// And helper types
//=============================================================================

template <typename...>
struct __and_;

template <>
struct __and_<> : public true_type
{
};

template <typename _B1>
struct __and_<_B1> : public _B1
{
};

template <typename _B1, typename _B2>
struct __and_<_B1, _B2>
    : public conditional<_B1::value, _B2, _B1>::type
{
};

template <typename _B1, typename _B2, typename _B3, typename... _Bn>
struct __and_<_B1, _B2, _B3, _Bn...>
    : public conditional<_B1::value, __and_<_B2, _B3, _Bn...>, _B1>::type
{
};

//=============================================================================
// Not helper type
//=============================================================================

template <typename _Pp>
struct __not_
    : public __bool_constant<!bool(_Pp::value)>
{
};

//=============================================================================
// And and or value helpers
//=============================================================================

template <typename... _Bn>
inline constexpr bool __or_v = __or_<_Bn...>::value;
template <typename... _Bn>
inline constexpr bool __and_v = __and_<_Bn...>::value;

//=============================================================================
// Logical helper types
//=============================================================================

template <typename... _Bn>
struct conjunction : __and_<_Bn...>
{
};

template <typename... _Bn>
struct disjunction : __or_<_Bn...>
{
};

template <typename _Pp>
struct negation : __not_<_Pp>
{
};

template <typename... _Bn>
inline constexpr bool conjunction_v = conjunction<_Bn...>::value;

template <typename... _Bn>
inline constexpr bool disjunction_v = disjunction<_Bn...>::value;

template <typename _Pp>
inline constexpr bool negation_v = negation<_Pp>::value;

//=============================================================================
// Success and failure types
//=============================================================================

template <typename _T>
struct __success_type
{
    typedef _T type;
};

struct __failure_type
{
};

//=============================================================================
// std::remove_cv fwd
//=============================================================================

template <typename>
struct remove_cv;

//=============================================================================
// std::is_void
//=============================================================================

template <typename>
struct __is_void_helper
    : public false_type
{
};

template <>
struct __is_void_helper<void>
    : public true_type
{
};

/// is_void
template <typename _T>
struct is_void
    : public __is_void_helper<typename remove_cv<_T>::type>::type
{
};

//=============================================================================
// std::is_integral
//=============================================================================

template <typename>
struct __is_integral_helper
    : public false_type
{
};

template <>
struct __is_integral_helper<bool>
    : public true_type
{
};

template <>
struct __is_integral_helper<char>
    : public true_type
{
};

template <>
struct __is_integral_helper<signed char>
    : public true_type
{
};

template <>
struct __is_integral_helper<unsigned char>
    : public true_type
{
};

template <>
struct __is_integral_helper<char16_t>
    : public true_type
{
};

template <>
struct __is_integral_helper<char32_t>
    : public true_type
{
};

template <>
struct __is_integral_helper<short>
    : public true_type
{
};

template <>
struct __is_integral_helper<unsigned short>
    : public true_type
{
};

template <>
struct __is_integral_helper<int>
    : public true_type
{
};

template <>
struct __is_integral_helper<unsigned int>
    : public true_type
{
};

template <>
struct __is_integral_helper<long>
    : public true_type
{
};

template <>
struct __is_integral_helper<unsigned long>
    : public true_type
{
};

template <>
struct __is_integral_helper<long long>
    : public true_type
{
};

template <>
struct __is_integral_helper<unsigned long long>
    : public true_type
{
};

template <typename _T>
struct is_integral
    : public __is_integral_helper<typename remove_cv<_T>::type>::type
{
};

//=============================================================================
// std::is_floating_point
//=============================================================================

template <typename>
struct __is_floating_point_helper
    : public false_type
{
};

template <>
struct __is_floating_point_helper<float>
    : public true_type
{
};

template <>
struct __is_floating_point_helper<double>
    : public true_type
{
};

template <>
struct __is_floating_point_helper<long double>
    : public true_type
{
};

/// is_floating_point
template <typename _T>
struct is_floating_point
    : public __is_floating_point_helper<typename remove_cv<_T>::type>::type
{
};

//=============================================================================
// std::is_array
//=============================================================================

template <typename>
struct is_array
    : public false_type
{
};

template <typename _T, std::size_t _Size>
struct is_array<_T[_Size]>
    : public true_type
{
};

template <typename _T>
struct is_array<_T[]>
    : public true_type
{
};

//=============================================================================
// std::is_pointer
//=============================================================================

template <typename>
struct __is_pointer_helper
    : public false_type
{
};

template <typename _T>
struct __is_pointer_helper<_T *>
    : public true_type
{
};

/// is_pointer
template <typename _T>
struct is_pointer
    : public __is_pointer_helper<typename remove_cv<_T>::type>::type
{
};

//=============================================================================
// std::is_lvalue_reference
//=============================================================================

template <typename>
struct is_lvalue_reference
    : public false_type
{
};

template <typename _T>
struct is_lvalue_reference<_T &>
    : public true_type
{
};

//=============================================================================
// std::is_rvalue_reference
//=============================================================================

template <typename>
struct is_rvalue_reference
    : public false_type
{
};

template <typename _T>
struct is_rvalue_reference<_T &&>
    : public true_type
{
};

//=============================================================================
// std::is_member_object_pointer
//=============================================================================

template <typename>
struct is_function;

template <typename>
struct __is_member_object_pointer_helper
    : public false_type
{
};

template <typename _T, typename _C>
struct __is_member_object_pointer_helper<_T _C::*>
    : public __not_<is_function<_T>>::type
{
};

template <typename _T>
struct is_member_object_pointer
    : public __is_member_object_pointer_helper<
          typename remove_cv<_T>::type>::type
{
};

//=============================================================================
// std::is_member_function_pointer
//=============================================================================

template <typename>
struct __is_member_function_pointer_helper
    : public false_type
{
};

template <typename _T, typename _C>
struct __is_member_function_pointer_helper<_T _C::*>
    : public is_function<_T>::type
{
};

template <typename _T>
struct is_member_function_pointer
    : public __is_member_function_pointer_helper<
          typename remove_cv<_T>::type>::type
{
};

//=============================================================================
// std::is_enum
//=============================================================================

template <typename _T>
struct is_enum
    : public integral_constant<bool, __is_enum(_T)>
{
};

//=============================================================================
// std::is_union
//=============================================================================

template <typename _T>
struct is_union
    : public integral_constant<bool, __is_union(_T)>
{
};

//=============================================================================
// std::is_class
//=============================================================================

template <typename _T>
struct is_class
    : public integral_constant<bool, __is_class(_T)>
{
};

//=============================================================================
// std::is_function
//=============================================================================

template <typename>
struct is_function
    : public false_type
{
};

template <typename _Res, typename... _ArgTypes>
struct is_function<_Res(_ArgTypes...)>
    : public true_type
{
};

template <typename _Res, typename... _ArgTypes>
struct is_function<_Res(_ArgTypes...) &>
    : public true_type
{
};

template <typename _Res, typename... _ArgTypes>
struct is_function<_Res(_ArgTypes...) &&>
    : public true_type
{
};

template <typename _Res, typename... _ArgTypes>
struct is_function<_Res(_ArgTypes......)>
    : public true_type
{
};

template <typename _Res, typename... _ArgTypes>
struct is_function<_Res(_ArgTypes......) &>
    : public true_type
{
};

template <typename _Res, typename... _ArgTypes>
struct is_function<_Res(_ArgTypes......) &&>
    : public true_type
{
};

template <typename _Res, typename... _ArgTypes>
struct is_function<_Res(_ArgTypes...) const>
    : public true_type
{
};

template <typename _Res, typename... _ArgTypes>
struct is_function<_Res(_ArgTypes...) const &>
    : public true_type
{
};

template <typename _Res, typename... _ArgTypes>
struct is_function<_Res(_ArgTypes...) const &&>
    : public true_type
{
};

template <typename _Res, typename... _ArgTypes>
struct is_function<_Res(_ArgTypes......) const>
    : public true_type
{
};

template <typename _Res, typename... _ArgTypes>
struct is_function<_Res(_ArgTypes......) const &>
    : public true_type
{
};

template <typename _Res, typename... _ArgTypes>
struct is_function<_Res(_ArgTypes......) const &&>
    : public true_type
{
};

template <typename _Res, typename... _ArgTypes>
struct is_function<_Res(_ArgTypes...) volatile>
    : public true_type
{
};

template <typename _Res, typename... _ArgTypes>
struct is_function<_Res(_ArgTypes...) volatile &>
    : public true_type
{
};

template <typename _Res, typename... _ArgTypes>
struct is_function<_Res(_ArgTypes...) volatile &&>
    : public true_type
{
};

template <typename _Res, typename... _ArgTypes>
struct is_function<_Res(_ArgTypes......) volatile>
    : public true_type
{
};

template <typename _Res, typename... _ArgTypes>
struct is_function<_Res(_ArgTypes......) volatile &>
    : public true_type
{
};

template <typename _Res, typename... _ArgTypes>
struct is_function<_Res(_ArgTypes......) volatile &&>
    : public true_type
{
};

template <typename _Res, typename... _ArgTypes>
struct is_function<_Res(_ArgTypes...) const volatile>
    : public true_type
{
};

template <typename _Res, typename... _ArgTypes>
struct is_function<_Res(_ArgTypes...) const volatile &>
    : public true_type
{
};

template <typename _Res, typename... _ArgTypes>
struct is_function<_Res(_ArgTypes...) const volatile &&>
    : public true_type
{
};

template <typename _Res, typename... _ArgTypes>
struct is_function<_Res(_ArgTypes......) const volatile>
    : public true_type
{
};

template <typename _Res, typename... _ArgTypes>
struct is_function<_Res(_ArgTypes......) const volatile &>
    : public true_type
{
};

template <typename _Res, typename... _ArgTypes>
struct is_function<_Res(_ArgTypes......) const volatile &&>
    : public true_type
{
};

//=============================================================================
// std::is_null_pointer
//=============================================================================

template <typename>
struct __is_null_pointer_helper
    : public false_type
{
};

template <>
struct __is_null_pointer_helper<std::nullptr_t>
    : public true_type
{
};

template <typename _T>
struct is_null_pointer
    : public __is_null_pointer_helper<typename remove_cv<_T>::type>::type
{
};

template <typename _T>
struct __is_nullptr_t
    : public is_null_pointer<_T>
{
};

//=============================================================================
// std::is_reference
//=============================================================================

template <typename _T>
struct is_reference
    : public __or_<is_lvalue_reference<_T>,
                   is_rvalue_reference<_T>>::type
{
};

//=============================================================================
// std::is_arithmetic
//=============================================================================

template <typename _T>
struct is_arithmetic
    : public __or_<is_integral<_T>, is_floating_point<_T>>::type
{
};

//=============================================================================
// std::is_fundamental
//=============================================================================

template <typename _T>
struct is_fundamental
    : public __or_<is_arithmetic<_T>, is_void<_T>,
                   is_null_pointer<_T>>::type
{
};

//=============================================================================
// std::is_object
//=============================================================================

template <typename _T>
struct is_object
    : public __not_<__or_<is_function<_T>, is_reference<_T>,
                          is_void<_T>>>::type
{
};

//=============================================================================
// std::is_scalar
//=============================================================================

template <typename>
struct is_member_pointer;

template <typename _T>
struct is_scalar
    : public __or_<is_arithmetic<_T>, is_enum<_T>, is_pointer<_T>,
                   is_member_pointer<_T>, is_null_pointer<_T>>::type
{
};

//=============================================================================
// std::is_compound
//=============================================================================

template <typename _T>
struct is_compound
    : public __not_<is_fundamental<_T>>::type
{
};

//=============================================================================
// std::is_member_pointer
//=============================================================================

template <typename _T>
struct __is_member_pointer_helper
    : public false_type
{
};

template <typename _T, typename _C>
struct __is_member_pointer_helper<_T _C::*>
    : public true_type
{
};

template <typename _T>
struct is_member_pointer
    : public __is_member_pointer_helper<typename remove_cv<_T>::type>::type
{
};

//=============================================================================
// __is_referenceable
//=============================================================================

template <typename _T>
struct __is_referenceable
    : public __or_<is_object<_T>, is_reference<_T>>::type
{
};

template <typename _Res, typename... _Args>
struct __is_referenceable<_Res(_Args...)>
    : public true_type
{
};

template <typename _Res, typename... _Args>
struct __is_referenceable<_Res(_Args......)>
    : public true_type
{
};

//=============================================================================
// std::is_const
//=============================================================================

template <typename>
struct is_const
    : public false_type
{
};

template <typename _T>
struct is_const<_T const>
    : public true_type
{
};

//=============================================================================
// std::is_volatile
//=============================================================================

template <typename>
struct is_volatile
    : public false_type
{
};

template <typename _T>
struct is_volatile<_T volatile>
    : public true_type
{
};

//=============================================================================
// std::is_trivial
//=============================================================================

template <typename _T>
struct is_trivial
    : public integral_constant<bool, __is_trivial(_T)>
{
};

//=============================================================================
// std::is_trivially_copyable
//=============================================================================

template <typename _T>
struct is_trivially_copyable
    : public integral_constant<bool, __is_trivially_copyable(_T)>
{
};

//=============================================================================
// std::is_standard_layout
//=============================================================================

template <typename _T>
struct is_standard_layout
    : public integral_constant<bool, __is_standard_layout(_T)>
{
};

//=============================================================================
// std::is_pod
//=============================================================================

template <typename _T>
struct is_pod
    : public integral_constant<bool, __is_pod(_T)>
{
};

//=============================================================================
// std::is_literal_type
//=============================================================================

template <typename _T>
struct is_literal_type
    : public integral_constant<bool, __is_literal_type(_T)>
{
};

//=============================================================================
// std::is_empty
//=============================================================================

template <typename _T>
struct is_empty
    : public integral_constant<bool, __is_empty(_T)>
{
};

//=============================================================================
// std::is_polymorphic
//=============================================================================

template <typename _T>
struct is_polymorphic
    : public integral_constant<bool, __is_polymorphic(_T)>
{
};

//=============================================================================
// std::is_final
//=============================================================================

template <typename _T>
struct is_final
    : public integral_constant<bool, __is_final(_T)>
{
};

//=============================================================================
// std::is_abstract
//=============================================================================

template <typename _T>
struct is_abstract
    : public integral_constant<bool, __is_abstract(_T)>
{
};

//=============================================================================
// std::is_signed
//=============================================================================

template <typename _T,
          bool = is_arithmetic<_T>::value>
struct __is_signed_helper
    : public false_type
{
};

template <typename _T>
struct __is_signed_helper<_T, true>
    : public integral_constant<bool, _T(-1) < _T(0)>
{
};

template <typename _T>
struct is_signed
    : public __is_signed_helper<_T>::type
{
};

//=============================================================================
// std::is_unsigned
//=============================================================================

template <typename _T>
struct is_unsigned
    : public __and_<is_arithmetic<_T>, __not_<is_signed<_T>>>
{
};

//=============================================================================
// std::declval
//=============================================================================

template <typename _T, typename _U = _T &&>
_U __declval(int);

template <typename _T>
_T __declval(long);

template <typename _T>
auto declval() noexcept -> decltype(__declval<_T>(0));

template <typename, unsigned = 0>
struct extent;

template <typename>
struct remove_all_extents;

template <typename _T>
struct __is_array_known_bounds
    : public integral_constant<bool, (extent<_T>::value > 0)>
{
};

template <typename _T>
struct __is_array_unknown_bounds
    : public __and_<is_array<_T>, __not_<extent<_T>>>
{
};

//=============================================================================
// std::is_destructible
//=============================================================================

struct __do_is_destructible_impl
{
    template <typename _T, typename = decltype(declval<_T &>().~_T())>
    static true_type __test(int);

    template <typename>
    static false_type __test(...);
};

template <typename _T>
struct __is_destructible_impl
    : public __do_is_destructible_impl
{
    typedef decltype(__test<_T>(0)) type;
};

template <typename _T,
          bool = __or_<is_void<_T>,
                       __is_array_unknown_bounds<_T>,
                       is_function<_T>>::value,
          bool = __or_<is_reference<_T>, is_scalar<_T>>::value>
struct __is_destructible_safe;

template <typename _T>
struct __is_destructible_safe<_T, false, false>
    : public __is_destructible_impl<typename remove_all_extents<_T>::type>::type
{
};

template <typename _T>
struct __is_destructible_safe<_T, true, false>
    : public false_type
{
};

template <typename _T>
struct __is_destructible_safe<_T, false, true>
    : public true_type
{
};

template <typename _T>
struct is_destructible
    : public __is_destructible_safe<_T>::type
{
};

//=============================================================================
// std::is_nothrow_destructible
//=============================================================================

struct __do_is_nt_destructible_impl
{
    template <typename _T>
    static __bool_constant<noexcept(declval<_T &>().~_T())>
    __test(int);

    template <typename>
    static false_type __test(...);
};

template <typename _T>
struct __is_nt_destructible_impl
    : public __do_is_nt_destructible_impl
{
    typedef decltype(__test<_T>(0)) type;
};

template <typename _T,
          bool = __or_<is_void<_T>,
                       __is_array_unknown_bounds<_T>,
                       is_function<_T>>::value,
          bool = __or_<is_reference<_T>, is_scalar<_T>>::value>
struct __is_nt_destructible_safe;

template <typename _T>
struct __is_nt_destructible_safe<_T, false, false>
    : public __is_nt_destructible_impl<typename remove_all_extents<_T>::type>::type
{
};

template <typename _T>
struct __is_nt_destructible_safe<_T, true, false>
    : public false_type
{
};

template <typename _T>
struct __is_nt_destructible_safe<_T, false, true>
    : public true_type
{
};

template <typename _T>
struct is_nothrow_destructible
    : public __is_nt_destructible_safe<_T>::type
{
};

//=============================================================================
// std::is_default_constructible
//=============================================================================

struct __do_is_default_constructible_impl
{
    template <typename _T, typename = decltype(_T())>
    static true_type __test(int);

    template <typename>
    static false_type __test(...);
};

template <typename _T>
struct __is_default_constructible_impl
    : public __do_is_default_constructible_impl
{
    typedef decltype(__test<_T>(0)) type;
};

template <typename _T>
struct __is_default_constructible_atom
    : public __and_<__not_<is_void<_T>>,
                    __is_default_constructible_impl<_T>>
{
};

template <typename _T, bool = is_array<_T>::value>
struct __is_default_constructible_safe;

template <typename _T>
struct __is_default_constructible_safe<_T, true>
    : public __and_<__is_array_known_bounds<_T>,
                    __is_default_constructible_atom<typename remove_all_extents<_T>::type>>
{
};

template <typename _T>
struct __is_default_constructible_safe<_T, false>
    : public __is_default_constructible_atom<_T>::type
{
};

template <typename _T>
struct is_default_constructible
    : public __is_default_constructible_safe<_T>::type
{
};

//=============================================================================
// std::is_constructible
//=============================================================================

/// is_constructible
template <typename _T, typename... _Args>
struct is_constructible
    : public __bool_constant<__is_constructible(_T, _Args...)>
{
};

//=============================================================================
// std::is_copy_constructible
//=============================================================================

template <typename _T, bool = __is_referenceable<_T>::value>
struct __is_copy_constructible_impl;

template <typename _T>
struct __is_copy_constructible_impl<_T, false>
    : public false_type
{
};

template <typename _T>
struct __is_copy_constructible_impl<_T, true>
    : public is_constructible<_T, const _T &>
{
};

template <typename _T>
struct is_copy_constructible
    : public __is_copy_constructible_impl<_T>
{
};

//=============================================================================
// std::is_move_constructible
//=============================================================================

template <typename _T, bool = __is_referenceable<_T>::value>
struct __is_move_constructible_impl;

template <typename _T>
struct __is_move_constructible_impl<_T, false>
    : public false_type
{
};

template <typename _T>
struct __is_move_constructible_impl<_T, true>
    : public is_constructible<_T, _T &&>
{
};

template <typename _T>
struct is_move_constructible
    : public __is_move_constructible_impl<_T>
{
};

//=============================================================================
// std::is_nothrow_default_constructible
//=============================================================================

template <typename _T>
struct __is_nt_default_constructible_atom
    : public integral_constant<bool, noexcept(_T())>
{
};

template <typename _T, bool = is_array<_T>::value>
struct __is_nt_default_constructible_impl;

template <typename _T>
struct __is_nt_default_constructible_impl<_T, true>
    : public __and_<__is_array_known_bounds<_T>,
                    __is_nt_default_constructible_atom<typename remove_all_extents<_T>::type>>
{
};

template <typename _T>
struct __is_nt_default_constructible_impl<_T, false>
    : public __is_nt_default_constructible_atom<_T>
{
};

template <typename _T>
struct is_nothrow_default_constructible
    : public __and_<is_default_constructible<_T>,
                    __is_nt_default_constructible_impl<_T>>
{
};

//=============================================================================
// std::is_nothrow_constructible
//=============================================================================

template <typename _T, typename... _Args>
struct __is_nt_constructible_impl
    : public integral_constant<bool, noexcept(_T(declval<_Args>()...))>
{
};

template <typename _T, typename _Arg>
struct __is_nt_constructible_impl<_T, _Arg>
    : public integral_constant<bool,
                               noexcept(static_cast<_T>(declval<_Arg>()))>
{
};

template <typename _T>
struct __is_nt_constructible_impl<_T>
    : public is_nothrow_default_constructible<_T>
{
};

template <typename _T, typename... _Args>
struct is_nothrow_constructible
    : public __and_<is_constructible<_T, _Args...>,
                    __is_nt_constructible_impl<_T, _Args...>>
{
};

//=============================================================================
// std::is_nothrow_copy_constructible
//=============================================================================

template <typename _T, bool = __is_referenceable<_T>::value>
struct __is_nothrow_copy_constructible_impl;

template <typename _T>
struct __is_nothrow_copy_constructible_impl<_T, false>
    : public false_type
{
};

template <typename _T>
struct __is_nothrow_copy_constructible_impl<_T, true>
    : public is_nothrow_constructible<_T, const _T &>
{
};

template <typename _T>
struct is_nothrow_copy_constructible
    : public __is_nothrow_copy_constructible_impl<_T>
{
};

//=============================================================================
// std::is_nothrow_move_constructible
//=============================================================================

template <typename _T, bool = __is_referenceable<_T>::value>
struct __is_nothrow_move_constructible_impl;

template <typename _T>
struct __is_nothrow_move_constructible_impl<_T, false>
    : public false_type
{
};

template <typename _T>
struct __is_nothrow_move_constructible_impl<_T, true>
    : public is_nothrow_constructible<_T, _T &&>
{
};

template <typename _T>
struct is_nothrow_move_constructible
    : public __is_nothrow_move_constructible_impl<_T>
{
};

//=============================================================================
// std::is_assignable
//=============================================================================

template <typename _T, typename _U>
struct is_assignable
    : public __bool_constant<__is_assignable(_T, _U)>
{
};

//=============================================================================
// std::is_copy_assignable
//=============================================================================

template <typename _T, bool = __is_referenceable<_T>::value>
struct __is_copy_assignable_impl;

template <typename _T>
struct __is_copy_assignable_impl<_T, false>
    : public false_type
{
};

template <typename _T>
struct __is_copy_assignable_impl<_T, true>
    : public is_assignable<_T &, const _T &>
{
};

template <typename _T>
struct is_copy_assignable
    : public __is_copy_assignable_impl<_T>
{
};

//=============================================================================
// std::is_move_assignable
//=============================================================================

template <typename _T, bool = __is_referenceable<_T>::value>
struct __is_move_assignable_impl;

template <typename _T>
struct __is_move_assignable_impl<_T, false>
    : public false_type
{
};

template <typename _T>
struct __is_move_assignable_impl<_T, true>
    : public is_assignable<_T &, _T &&>
{
};

template <typename _T>
struct is_move_assignable
    : public __is_move_assignable_impl<_T>
{
};

//=============================================================================
// std::is_nothrow_assignable
//=============================================================================

template <typename _T, typename _U>
struct __is_nt_assignable_impl
    : public integral_constant<bool, noexcept(declval<_T>() = declval<_U>())>
{
};

template <typename _T, typename _U>
struct is_nothrow_assignable
    : public __and_<is_assignable<_T, _U>,
                    __is_nt_assignable_impl<_T, _U>>
{
};

//=============================================================================
// std::is_nothrow_copy_assignable
//=============================================================================

template <typename _T, bool = __is_referenceable<_T>::value>
struct __is_nt_copy_assignable_impl;

template <typename _T>
struct __is_nt_copy_assignable_impl<_T, false>
    : public false_type
{
};

template <typename _T>
struct __is_nt_copy_assignable_impl<_T, true>
    : public is_nothrow_assignable<_T &, const _T &>
{
};

template <typename _T>
struct is_nothrow_copy_assignable
    : public __is_nt_copy_assignable_impl<_T>
{
};

//=============================================================================
// std::is_nothrow_move_assignable
//=============================================================================

template <typename _T, bool = __is_referenceable<_T>::value>
struct __is_nt_move_assignable_impl;

template <typename _T>
struct __is_nt_move_assignable_impl<_T, false>
    : public false_type
{
};

template <typename _T>
struct __is_nt_move_assignable_impl<_T, true>
    : public is_nothrow_assignable<_T &, _T &&>
{
};

template <typename _T>
struct is_nothrow_move_assignable
    : public __is_nt_move_assignable_impl<_T>
{
};

//=============================================================================
// std::is_trivially_constructible
//=============================================================================

template <typename _T, typename... _Args>
struct is_trivially_constructible
    : public __bool_constant<__is_trivially_constructible(_T, _Args...)>
{
};

//=============================================================================
// std::is_trivially_default_constructible
//=============================================================================

template <typename _T>
struct is_trivially_default_constructible
    : public is_trivially_constructible<_T>::type
{
};

//=============================================================================
// std::is_implicitly_default_constructble
//=============================================================================

struct __do_is_implicitly_default_constructible_impl
{
    template <typename _T>
    static void __helper(const _T &);

    template <typename _T>
    static true_type __test(const _T &,
                            decltype(__helper<const _T &>({})) * = 0);

    static false_type __test(...);
};

template <typename _T>
struct __is_implicitly_default_constructible_impl
    : public __do_is_implicitly_default_constructible_impl
{
    typedef decltype(__test(declval<_T>())) type;
};

template <typename _T>
struct __is_implicitly_default_constructible_safe
    : public __is_implicitly_default_constructible_impl<_T>::type
{
};

template <typename _T>
struct __is_implicitly_default_constructible
    : public __and_<is_default_constructible<_T>,
                    __is_implicitly_default_constructible_safe<_T>>
{
};

//=============================================================================
// std::is_trivially_copy_constructible
//=============================================================================

template <typename _T, bool = __is_referenceable<_T>::value>
struct __is_trivially_copy_constructible_impl;

template <typename _T>
struct __is_trivially_copy_constructible_impl<_T, false>
    : public false_type
{
};

template <typename _T>
struct __is_trivially_copy_constructible_impl<_T, true>
    : public __and_<is_copy_constructible<_T>,
                    integral_constant<bool,
                                      __is_trivially_constructible(_T, const _T &)>>
{
};

template <typename _T>
struct is_trivially_copy_constructible
    : public __is_trivially_copy_constructible_impl<_T>
{
};

//=============================================================================
// std::is_trivially_move_constructible
//=============================================================================

template <typename _T, bool = __is_referenceable<_T>::value>
struct __is_trivially_move_constructible_impl;

template <typename _T>
struct __is_trivially_move_constructible_impl<_T, false>
    : public false_type
{
};

template <typename _T>
struct __is_trivially_move_constructible_impl<_T, true>
    : public __and_<is_move_constructible<_T>,
                    integral_constant<bool,
                                      __is_trivially_constructible(_T, _T &&)>>
{
};

template <typename _T>
struct is_trivially_move_constructible
    : public __is_trivially_move_constructible_impl<_T>
{
};

//=============================================================================
// std::is_trivially_assignable
//=============================================================================

template <typename _T, typename _U>
struct is_trivially_assignable
    : public __bool_constant<__is_trivially_assignable(_T, _U)>
{
};

//=============================================================================
// std::is_trivially_copy_assignable
//=============================================================================

template <typename _T, bool = __is_referenceable<_T>::value>
struct __is_trivially_copy_assignable_impl;

template <typename _T>
struct __is_trivially_copy_assignable_impl<_T, false>
    : public false_type
{
};

template <typename _T>
struct __is_trivially_copy_assignable_impl<_T, true>
    : public __bool_constant<__is_trivially_assignable(_T &, const _T &)>
{
};

template <typename _T>
struct is_trivially_copy_assignable
    : public __is_trivially_copy_assignable_impl<_T>
{
};

//=============================================================================
// std::is_trivially_move_assignable
//=============================================================================

template <typename _T, bool = __is_referenceable<_T>::value>
struct __is_trivially_move_assignable_impl;

template <typename _T>
struct __is_trivially_move_assignable_impl<_T, false>
    : public false_type
{
};

template <typename _T>
struct __is_trivially_move_assignable_impl<_T, true>
    : public __bool_constant<__is_trivially_assignable(_T &, _T &&)>
{
};

template <typename _T>
struct is_trivially_move_assignable
    : public __is_trivially_move_assignable_impl<_T>
{
};

//=============================================================================
// std::is_trivially_destructible
//=============================================================================

template <typename _T>
struct is_trivially_destructible
    : public __and_<is_destructible<_T>,
                    __bool_constant<__has_trivial_destructor(_T)>>
{
};

//=============================================================================
// std::has_virtual_destructor
//=============================================================================

template <typename _T>
struct has_virtual_destructor
    : public integral_constant<bool, __has_virtual_destructor(_T)>
{
};

//=============================================================================
// std::alignment_of
//=============================================================================

template <typename _T>
struct alignment_of
    : public integral_constant<std::size_t, __alignof__(_T)>
{
};

//=============================================================================
// std::rank
//=============================================================================

template <typename>
struct rank
    : public integral_constant<std::size_t, 0>
{
};

template <typename _T, std::size_t _Size>
struct rank<_T[_Size]>
    : public integral_constant<std::size_t, 1 + rank<_T>::value>
{
};

template <typename _T>
struct rank<_T[]>
    : public integral_constant<std::size_t, 1 + rank<_T>::value>
{
};

//=============================================================================
// std::extent
//=============================================================================

template <typename, unsigned _Uint>
struct extent
    : public integral_constant<std::size_t, 0>
{
};

template <typename _T, unsigned _Uint, std::size_t _Size>
struct extent<_T[_Size], _Uint>
    : public integral_constant<std::size_t,
                               _Uint == 0 ? _Size : extent<_T, _Uint - 1>::value>
{
};

template <typename _T, unsigned _Uint>
struct extent<_T[], _Uint>
    : public integral_constant<std::size_t,
                               _Uint == 0 ? 0 : extent<_T, _Uint - 1>::value>
{
};

//=============================================================================
// std::is_same
//=============================================================================

template <typename, typename>
struct is_same
    : public false_type
{
};

template <typename _T>
struct is_same<_T, _T>
    : public true_type
{
};

//=============================================================================
// std::is_base_of
//=============================================================================

template <typename _Base, typename _Derived>
struct is_base_of
    : public integral_constant<bool, __is_base_of(_Base, _Derived)>
{
};

//=============================================================================
// std::is_convertible
//=============================================================================

template <typename _From, typename _To,
          bool = __or_<is_void<_From>, is_function<_To>,
                       is_array<_To>>::value>
struct __is_convertible_helper
{
    typedef typename is_void<_To>::type type;
};

template <typename _From, typename _To>
class __is_convertible_helper<_From, _To, false>
{
    template <typename _To1>
    static void __test_aux(_To1) noexcept;

    template <typename _From1, typename _To1,
              typename = decltype(__test_aux<_To1>(std::declval<_From1>()))>
    static true_type
    __test(int);

    template <typename, typename>
    static false_type
    __test(...);

public:
    typedef decltype(__test<_From, _To>(0)) type;
};

template <typename _From, typename _To>
struct is_convertible
    : public __is_convertible_helper<_From, _To>::type
{
};

//=============================================================================
// std::remove_const
//=============================================================================

template <typename _T>
struct remove_const
{
    typedef _T type;
};

template <typename _T>
struct remove_const<_T const>
{
    typedef _T type;
};

template <typename _T>
using remove_const_t = typename remove_const<_T>::type;

//=============================================================================
// std::remove_volatile
//=============================================================================

template <typename _T>
struct remove_volatile
{
    typedef _T type;
};

template <typename _T>
struct remove_volatile<_T volatile>
{
    typedef _T type;
};

template <typename _T>
using remove_volatile_t = typename remove_volatile<_T>::type;

//=============================================================================
// std::remove_cv
//=============================================================================

template <typename _T>
struct remove_cv
{
    typedef typename remove_const<typename remove_volatile<_T>::type>::type type;
};

template <typename _T>
using remove_cv_t = typename remove_cv<_T>::type;

//=============================================================================
// std::add_const
//=============================================================================

template <typename _T>
struct add_const
{
    typedef _T const type;
};

template <typename _T>
using add_const_t = typename add_const<_T>::type;

//=============================================================================
// std::add_volatile
//=============================================================================

template <typename _T>
struct add_volatile
{
    typedef _T volatile type;
};

template <typename _T>
using add_volatile_t = typename add_volatile<_T>::type;

//=============================================================================
// std::add_cv
//=============================================================================

template <typename _T>
struct add_cv
{
    typedef typename add_const<typename add_volatile<_T>::type>::type type;
};

template <typename _T>
using add_cv_t = typename add_cv<_T>::type;

//=============================================================================
// std::remove_reference
//=============================================================================

template <typename _T>
struct remove_reference
{
    typedef _T type;
};

template <typename _T>
struct remove_reference<_T &>
{
    typedef _T type;
};

template <typename _T>
struct remove_reference<_T &&>
{
    typedef _T type;
};

template <typename _T>
using remove_reference_t = typename remove_reference<_T>::type;

//=============================================================================
// std::add_lvalue_reference
//=============================================================================

template <typename _T, bool = __is_referenceable<_T>::value>
struct __add_lvalue_reference_helper
{
    typedef _T type;
};

template <typename _T>
struct __add_lvalue_reference_helper<_T, true>
{
    typedef _T &type;
};

/// add_lvalue_reference
template <typename _T>
struct add_lvalue_reference
    : public __add_lvalue_reference_helper<_T>
{
};

template <typename _T>
using add_lvalue_reference_t = typename add_lvalue_reference<_T>::type;

//=============================================================================
// std::add_rvalue_reference
//=============================================================================

template <typename _T, bool = __is_referenceable<_T>::value>
struct __add_rvalue_reference_helper
{
    typedef _T type;
};

template <typename _T>
struct __add_rvalue_reference_helper<_T, true>
{
    typedef _T &&type;
};

/// add_rvalue_reference
template <typename _T>
struct add_rvalue_reference
    : public __add_rvalue_reference_helper<_T>
{
};

template <typename _T>
using add_rvalue_reference_t = typename add_rvalue_reference<_T>::type;

//=============================================================================
// std::__cv_selector
//=============================================================================

template <typename _Unqualified, bool _IsConst, bool _IsVol>
struct __cv_selector;

template <typename _Unqualified>
struct __cv_selector<_Unqualified, false, false>
{
    typedef _Unqualified __type;
};

template <typename _Unqualified>
struct __cv_selector<_Unqualified, false, true>
{
    typedef volatile _Unqualified __type;
};

template <typename _Unqualified>
struct __cv_selector<_Unqualified, true, false>
{
    typedef const _Unqualified __type;
};

template <typename _Unqualified>
struct __cv_selector<_Unqualified, true, true>
{
    typedef const volatile _Unqualified __type;
};

template <typename _Qualified, typename _Unqualified,
          bool _IsConst = is_const<_Qualified>::value,
          bool _IsVol = is_volatile<_Qualified>::value>
class __match_cv_qualifiers
{
    typedef __cv_selector<_Unqualified, _IsConst, _IsVol> __match;

public:
    typedef typename __match::__type __type;
};

//=============================================================================
// std::make_unsigned
//=============================================================================

template <typename _T>
struct __make_unsigned
{
    typedef _T __type;
};

template <>
struct __make_unsigned<char>
{
    typedef unsigned char __type;
};

template <>
struct __make_unsigned<signed char>
{
    typedef unsigned char __type;
};

template <>
struct __make_unsigned<short>
{
    typedef unsigned short __type;
};

template <>
struct __make_unsigned<int>
{
    typedef unsigned int __type;
};

template <>
struct __make_unsigned<long>
{
    typedef unsigned long __type;
};

template <>
struct __make_unsigned<long long>
{
    typedef unsigned long long __type;
};

template <typename _T,
          bool _IsInt = is_integral<_T>::value,
          bool _IsEnum = is_enum<_T>::value>
class __make_unsigned_selector;

template <typename _T>
class __make_unsigned_selector<_T, true, false>
{
    using __unsigned_type = typename __make_unsigned<typename remove_cv<_T>::type>::__type;

public:
    using __type = typename __match_cv_qualifiers<_T, __unsigned_type>::__type;
};

class __make_unsigned_selector_base
{
protected:
    template <typename...>
    struct _List
    {
    };

    template <typename _T, typename... _U>
    struct _List<_T, _U...> : _List<_U...>
    {
        static constexpr size_t __size = sizeof(_T);
    };

    template <size_t _Sz, typename _T, bool = (_Sz <= _T::__size)>
    struct __select;

    template <size_t _Sz, typename _Uint, typename... _UInts>
    struct __select<_Sz, _List<_Uint, _UInts...>, true>
    {
        using __type = _Uint;
    };

    template <size_t _Sz, typename _Uint, typename... _UInts>
    struct __select<_Sz, _List<_Uint, _UInts...>, false>
        : __select<_Sz, _List<_UInts...>>
    {
    };
};

template <typename _T>
class __make_unsigned_selector<_T, false, true>
    : __make_unsigned_selector_base
{
    // With -fshort-enums, an enum may be as small as a char.
    using _UInts = _List<unsigned char, unsigned short, unsigned int,
                         unsigned long, unsigned long long>;

    using __unsigned_type = typename __select<sizeof(_T), _UInts>::__type;

public:
    using __type = typename __match_cv_qualifiers<_T, __unsigned_type>::__type;
};

template <>
struct __make_unsigned<char16_t>
{
    using __type = typename __make_unsigned_selector<char16_t, false, true>::__type;
};

template <>
struct __make_unsigned<char32_t>
{
    using __type = typename __make_unsigned_selector<char32_t, false, true>::__type;
};

template <typename _T>
struct make_unsigned
{
    typedef typename __make_unsigned_selector<_T>::__type type;
};

template <>
struct make_unsigned<bool>;

template <typename _T>
using make_unsigned_t = typename make_unsigned<_T>::type;

//=============================================================================
// std::make_signed
//=============================================================================

template <typename _T>
struct __make_signed
{
    typedef _T __type;
};

template <>
struct __make_signed<char>
{
    typedef signed char __type;
};

template <>
struct __make_signed<unsigned char>
{
    typedef signed char __type;
};

template <>
struct __make_signed<unsigned short>
{
    typedef signed short __type;
};

template <>
struct __make_signed<unsigned int>
{
    typedef signed int __type;
};

template <>
struct __make_signed<unsigned long>
{
    typedef signed long __type;
};

template <>
struct __make_signed<unsigned long long>
{
    typedef signed long long __type;
};

template <typename _T,
          bool _IsInt = is_integral<_T>::value,
          bool _IsEnum = is_enum<_T>::value>
class __make_signed_selector;

template <typename _T>
class __make_signed_selector<_T, true, false>
{
    using __signed_type = typename __make_signed<typename remove_cv<_T>::type>::__type;

public:
    using __type = typename __match_cv_qualifiers<_T, __signed_type>::__type;
};

template <typename _T>
class __make_signed_selector<_T, false, true>
{
    typedef typename __make_unsigned_selector<_T>::__type __unsigned_type;

public:
    typedef typename __make_signed_selector<__unsigned_type>::__type __type;
};

template <>
struct __make_signed<char16_t>
{
    using __type = typename __make_signed_selector<char16_t, false, true>::__type;
};

template <>
struct __make_signed<char32_t>
{
    using __type = typename __make_signed_selector<char32_t, false, true>::__type;
};

template <typename _T>
struct make_signed
{
    typedef typename __make_signed_selector<_T>::__type type;
};

template <>
struct make_signed<bool>;

template <typename _T>
using make_signed_t = typename make_signed<_T>::type;

//=============================================================================
// std::remove_extent
//=============================================================================

template <typename _T>
struct remove_extent
{
    typedef _T type;
};

template <typename _T, std::size_t _Size>
struct remove_extent<_T[_Size]>
{
    typedef _T type;
};

template <typename _T>
struct remove_extent<_T[]>
{
    typedef _T type;
};

template <typename _T>
using remove_extent_t = typename remove_extent<_T>::type;

//=============================================================================
// std::remove_all_extents
//=============================================================================

/// remove_all_extents
template <typename _T>
struct remove_all_extents
{
    typedef _T type;
};

template <typename _T, std::size_t _Size>
struct remove_all_extents<_T[_Size]>
{
    typedef typename remove_all_extents<_T>::type type;
};

template <typename _T>
struct remove_all_extents<_T[]>
{
    typedef typename remove_all_extents<_T>::type type;
};

template <typename _T>
using remove_all_extents_t = typename remove_all_extents<_T>::type;

//=============================================================================
// std::remove_pointer
//=============================================================================

template <typename _T, typename>
struct __remove_pointer_helper
{
    typedef _T type;
};

template <typename _T, typename _U>
struct __remove_pointer_helper<_T, _U *>
{
    typedef _U type;
};

template <typename _T>
struct remove_pointer
    : public __remove_pointer_helper<_T, typename remove_cv<_T>::type>
{
};

template <typename _T>
using remove_pointer_t = typename remove_pointer<_T>::type;

//=============================================================================
// std::add_pointer
//=============================================================================

template <typename _T, bool = __or_<__is_referenceable<_T>,
                                    is_void<_T>>::value>
struct __add_pointer_helper
{
    typedef _T type;
};

template <typename _T>
struct __add_pointer_helper<_T, true>
{
    typedef typename remove_reference<_T>::type *type;
};

template <typename _T>
struct add_pointer
    : public __add_pointer_helper<_T>
{
};

template <typename _T>
using add_pointer_t = typename add_pointer<_T>::type;

//=============================================================================
// std::aligned_storage
//=============================================================================
#if 0
template <std::size_t _Len, std::size_t _Align =
                                __alignof__(typename __aligned_storage_msa<_Len>::__type)>
struct aligned_storage
{
    union type {
        unsigned char __data[_Len];
        struct __attribute__((__aligned__((_Align))))
        {
        } __align;
    };
};

template <typename... _Types>
struct __strictest_alignment
{
    static const size_t _S_alignment = 0;
    static const size_t _S_size = 0;
};

template <typename _T, typename... _Types>
struct __strictest_alignment<_T, _Types...>
{
    static const size_t _S_alignment =
        alignof(_T) > __strictest_alignment<_Types...>::_S_alignment
            ? alignof(_T)
            : __strictest_alignment<_Types...>::_S_alignment;
    static const size_t _S_size =
        sizeof(_T) > __strictest_alignment<_Types...>::_S_size
            ? sizeof(_T)
            : __strictest_alignment<_Types...>::_S_size;
};

template <size_t _Len, typename... _Types>
struct aligned_union
{
private:
    static_assert(sizeof...(_Types) != 0, "At least one type is required");

    using __strictest = __strictest_alignment<_Types...>;
    static const size_t _S_len = _Len > __strictest::_S_size
                                     ? _Len
                                     : __strictest::_S_size;

public:
    static const size_t alignment_value = __strictest::_S_alignment;
    typedef typename aligned_storage<_S_len, alignment_value>::type type;
};

template <size_t _Len, typename... _Types>
const size_t aligned_union<_Len, _Types...>::alignment_value;

#endif

//=============================================================================
// std::decay
//=============================================================================

template <typename _U,
          bool _IsArray = is_array<_U>::value,
          bool _IsFunction = is_function<_U>::value>
struct __decay_selector;

template <typename _U>
struct __decay_selector<_U, false, false>
{
    typedef typename remove_cv<_U>::type __type;
};

template <typename _U>
struct __decay_selector<_U, true, false>
{
    typedef typename remove_extent<_U>::type *__type;
};

template <typename _U>
struct __decay_selector<_U, false, true>
{
    typedef typename add_pointer<_U>::type __type;
};

template <typename _T>
class decay
{
    typedef typename remove_reference<_T>::type __remove_type;

public:
    typedef typename __decay_selector<__remove_type>::__type type;
};

template <typename _T>
class reference_wrapper;

template <typename _T>
struct __strip_reference_wrapper
{
    typedef _T __type;
};

template <typename _T>
struct __strip_reference_wrapper<reference_wrapper<_T>>
{
    typedef _T &__type;
};

template <typename _T>
struct __decay_and_strip
{
    typedef typename __strip_reference_wrapper<
        typename decay<_T>::type>::__type __type;
};

//=============================================================================
// std::enable_if
//=============================================================================

template <bool, typename _T = void>
struct enable_if
{
};

template <typename _T>
struct enable_if<true, _T>
{
    typedef _T type;
};

template <typename... _Cond>
using _Require = typename enable_if<__and_<_Cond...>::value>::type;

//=============================================================================
// std::conditional
//=============================================================================

template <bool _Cond, typename _Iftrue, typename _Iffalse>
struct conditional
{
    typedef _Iftrue type;
};

template <typename _Iftrue, typename _Iffalse>
struct conditional<false, _Iftrue, _Iffalse>
{
    typedef _Iffalse type;
};

//=============================================================================
// std::common_type
//=============================================================================

template <typename... _T>
struct common_type;

struct __do_common_type_impl
{
    template <typename _T, typename _U>
    static __success_type<typename decay<decltype(true ? std::declval<_T>()
                                                       : std::declval<_U>())>::type>
    _S_test(int);

    template <typename, typename>
    static __failure_type _S_test(...);
};

template <typename _T, typename _U>
struct __common_type_impl
    : private __do_common_type_impl
{
    typedef decltype(_S_test<_T, _U>(0)) type;
};

struct __do_member_type_wrapper
{
    template <typename _T>
    static __success_type<typename _T::type> _S_test(int);

    template <typename>
    static __failure_type _S_test(...);
};

template <typename _T>
struct __member_type_wrapper
    : private __do_member_type_wrapper
{
    typedef decltype(_S_test<_T>(0)) type;
};

template <typename _CTp, typename... _Args>
struct __expanded_common_type_wrapper
{
    typedef common_type<typename _CTp::type, _Args...> type;
};

template <typename... _Args>
struct __expanded_common_type_wrapper<__failure_type, _Args...>
{
    typedef __failure_type type;
};

template <typename _T>
struct common_type<_T>
{
    typedef typename decay<_T>::type type;
};

template <typename _T, typename _U>
struct common_type<_T, _U>
    : public __common_type_impl<_T, _U>::type
{
};

template <typename _T, typename _U, typename... _Vp>
struct common_type<_T, _U, _Vp...>
    : public __expanded_common_type_wrapper<typename __member_type_wrapper<
                                                common_type<_T, _U>>::type,
                                            _Vp...>::type
{
};

template <typename _T>
struct underlying_type
{
    typedef __underlying_type(_T) type;
};

//=============================================================================
// std::declval
//=============================================================================

template <typename _T>
struct __declval_protector
{
    static const bool __stop = false;
};

template <typename _T>
auto declval() noexcept -> decltype(__declval<_T>(0))
{
    static_assert(__declval_protector<_T>::__stop,
                  "declval() must not be used!");
    return __declval<_T>(0);
}

template <typename _T>
using __remove_cvref_t = typename remove_cv<typename remove_reference<_T>::type>::type;

//=============================================================================
// std::result_of
//=============================================================================

template <typename _Signature>
class result_of;

struct __invoke_memfun_ref
{
};
struct __invoke_memfun_deref
{
};
struct __invoke_memobj_ref
{
};
struct __invoke_memobj_deref
{
};
struct __invoke_other
{
};

template <typename _T, typename _Tag>
struct __result_of_success : __success_type<_T>
{
    using __invoke_type = _Tag;
};

struct __result_of_memfun_ref_impl
{
    template <typename _Fp, typename _T1, typename... _Args>
    static __result_of_success<decltype(
                                   (std::declval<_T1>().*std::declval<_Fp>())(std::declval<_Args>()...)),
                               __invoke_memfun_ref>
    _S_test(int);

    template <typename...>
    static __failure_type _S_test(...);
};

template <typename _MemPtr, typename _Arg, typename... _Args>
struct __result_of_memfun_ref
    : private __result_of_memfun_ref_impl
{
    typedef decltype(_S_test<_MemPtr, _Arg, _Args...>(0)) type;
};

struct __result_of_memfun_deref_impl
{
    template <typename _Fp, typename _T1, typename... _Args>
    static __result_of_success<decltype(
                                   ((*std::declval<_T1>()).*std::declval<_Fp>())(std::declval<_Args>()...)),
                               __invoke_memfun_deref>
    _S_test(int);

    template <typename...>
    static __failure_type _S_test(...);
};

template <typename _MemPtr, typename _Arg, typename... _Args>
struct __result_of_memfun_deref
    : private __result_of_memfun_deref_impl
{
    typedef decltype(_S_test<_MemPtr, _Arg, _Args...>(0)) type;
};

struct __result_of_memobj_ref_impl
{
    template <typename _Fp, typename _T1>
    static __result_of_success<decltype(
                                   std::declval<_T1>().*std::declval<_Fp>()),
                               __invoke_memobj_ref>
    _S_test(int);

    template <typename, typename>
    static __failure_type _S_test(...);
};

template <typename _MemPtr, typename _Arg>
struct __result_of_memobj_ref
    : private __result_of_memobj_ref_impl
{
    typedef decltype(_S_test<_MemPtr, _Arg>(0)) type;
};

struct __result_of_memobj_deref_impl
{
    template <typename _Fp, typename _T1>
    static __result_of_success<decltype(
                                   (*std::declval<_T1>()).*std::declval<_Fp>()),
                               __invoke_memobj_deref>
    _S_test(int);

    template <typename, typename>
    static __failure_type _S_test(...);
};

template <typename _MemPtr, typename _Arg>
struct __result_of_memobj_deref
    : private __result_of_memobj_deref_impl
{
    typedef decltype(_S_test<_MemPtr, _Arg>(0)) type;
};

template <typename _MemPtr, typename _Arg>
struct __result_of_memobj;

template <typename _Res, typename _Class, typename _Arg>
struct __result_of_memobj<_Res _Class::*, _Arg>
{
    typedef __remove_cvref_t<_Arg> _Argval;
    typedef _Res _Class::*_MemPtr;
    typedef typename conditional<__or_<is_same<_Argval, _Class>,
                                       is_base_of<_Class, _Argval>>::value,
                                 __result_of_memobj_ref<_MemPtr, _Arg>,
                                 __result_of_memobj_deref<_MemPtr, _Arg>>::type::type type;
};

template <typename _MemPtr, typename _Arg, typename... _Args>
struct __result_of_memfun;

template <typename _Res, typename _Class, typename _Arg, typename... _Args>
struct __result_of_memfun<_Res _Class::*, _Arg, _Args...>
{
    typedef __remove_cvref_t<_Arg> _Argval;
    typedef _Res _Class::*_MemPtr;
    typedef typename conditional<__or_<is_same<_Argval, _Class>,
                                       is_base_of<_Class, _Argval>>::value,
                                 __result_of_memfun_ref<_MemPtr, _Arg, _Args...>,
                                 __result_of_memfun_deref<_MemPtr, _Arg, _Args...>>::type::type type;
};

template <typename _T, typename _U = typename decay<_T>::type>
struct __inv_unwrap
{
    using type = _T;
};

template <typename _T, typename _U>
struct __inv_unwrap<_T, reference_wrapper<_U>>
{
    using type = _U &;
};

template <bool, bool, typename _Functor, typename... _ArgTypes>
struct __result_of_impl
{
    typedef __failure_type type;
};

template <typename _MemPtr, typename _Arg>
struct __result_of_impl<true, false, _MemPtr, _Arg>
    : public __result_of_memobj<typename decay<_MemPtr>::type,
                                typename __inv_unwrap<_Arg>::type>
{
};

template <typename _MemPtr, typename _Arg, typename... _Args>
struct __result_of_impl<false, true, _MemPtr, _Arg, _Args...>
    : public __result_of_memfun<typename decay<_MemPtr>::type,
                                typename __inv_unwrap<_Arg>::type, _Args...>
{
};

struct __result_of_other_impl
{
    template <typename _Fn, typename... _Args>
    static __result_of_success<decltype(
                                   std::declval<_Fn>()(std::declval<_Args>()...)),
                               __invoke_other>
    _S_test(int);

    template <typename...>
    static __failure_type _S_test(...);
};

template <typename _Functor, typename... _ArgTypes>
struct __result_of_impl<false, false, _Functor, _ArgTypes...>
    : private __result_of_other_impl
{
    typedef decltype(_S_test<_Functor, _ArgTypes...>(0)) type;
};

template <typename _Functor, typename... _ArgTypes>
struct __invoke_result
    : public __result_of_impl<
          is_member_object_pointer<
              typename remove_reference<_Functor>::type>::value,
          is_member_function_pointer<
              typename remove_reference<_Functor>::type>::value,
          _Functor, _ArgTypes...>::type
{
};

template <typename _Functor, typename... _ArgTypes>
struct result_of<_Functor(_ArgTypes...)>
    : public __invoke_result<_Functor, _ArgTypes...>
{
};

//=============================================================================
// Aliases
//=============================================================================

#if 0
/// Alias template for aligned_storage
template <size_t _Len, size_t _Align =
                           __alignof__(typename __aligned_storage_msa<_Len>::__type)>
using aligned_storage_t = typename aligned_storage<_Len, _Align>::type;

template <size_t _Len, typename... _Types>
using aligned_union_t = typename aligned_union<_Len, _Types...>::type;

#endif

/// Alias template for decay
template <typename _T>
using decay_t = typename decay<_T>::type;

/// Alias template for enable_if
template <bool _Cond, typename _T = void>
using enable_if_t = typename enable_if<_Cond, _T>::type;

/// Alias template for conditional
template <bool _Cond, typename _Iftrue, typename _Iffalse>
using conditional_t = typename conditional<_Cond, _Iftrue, _Iffalse>::type;

/// Alias template for common_type
template <typename... _T>
using common_type_t = typename common_type<_T...>::type;

/// Alias template for underlying_type
template <typename _T>
using underlying_type_t = typename underlying_type<_T>::type;

/// Alias template for result_of
template <typename _T>
using result_of_t = typename result_of<_T>::type;

// __enable_if_t (std::enable_if_t for C++11)
template <bool _Cond, typename _T = void>
using __enable_if_t = typename enable_if<_Cond, _T>::type;

// __void_t (std::void_t for C++11)
template <typename...>
using __void_t = void;

/// A metafunction that always yields void, used for detecting valid types.
template <typename...>
using void_t = void;

//=============================================================================
// detector
//=============================================================================

/// Implementation of the detection idiom (negative case).
template <typename _Default, typename _AlwaysVoid,
          template <typename...> class _Op, typename... _Args>
struct __detector
{
    using value_t = false_type;
    using type = _Default;
};

/// Implementation of the detection idiom (positive case).
template <typename _Default, template <typename...> class _Op,
          typename... _Args>
struct __detector<_Default, __void_t<_Op<_Args...>>, _Op, _Args...>
{
    using value_t = true_type;
    using type = _Op<_Args...>;
};

// Detect whether _Op<_Args...> is a valid type, use _Default if not.
template <typename _Default, template <typename...> class _Op,
          typename... _Args>
using __detected_or = __detector<_Default, void, _Op, _Args...>;

// _Op<_Args...> if that is a valid type, otherwise _Default.
template <typename _Default, template <typename...> class _Op,
          typename... _Args>
using __detected_or_t = typename __detected_or<_Default, _Op, _Args...>::type;

#define _HAS_NESTED_TYPE(_NTYPE)                             \
    template <typename _T, typename = __void_t<>>            \
    struct __has_##_NTYPE                                    \
        : false_type                                         \
    {                                                        \
    };                                                       \
    template <typename _T>                                   \
    struct __has_##_NTYPE<_T, __void_t<typename _T::_NTYPE>> \
        : true_type                                          \
    {                                                        \
    };

template <typename _T>
struct __is_swappable;

template <typename _T>
struct __is_nothrow_swappable;

template <typename... _Elements>
class tuple;

template <typename>
struct __is_tuple_like_impl : false_type
{
};

template <typename... _Ts>
struct __is_tuple_like_impl<tuple<_Ts...>> : true_type
{
};

// Internal type trait that allows us to sfinae-protect tuple_cat.
template <typename _T>
struct __is_tuple_like
    : public __is_tuple_like_impl<__remove_cvref_t<_T>>::type
{
};

//=============================================================================
// std::is_swappable
//=============================================================================

template <typename _T>
inline
    typename enable_if<__and_<__not_<__is_tuple_like<_T>>,
                              is_move_constructible<_T>,
                              is_move_assignable<_T>>::value>::type
    swap(_T &, _T &) noexcept(__and_<is_nothrow_move_constructible<_T>,
                                     is_nothrow_move_assignable<_T>>::value);

template <typename _T, size_t _Nm>
inline
    typename enable_if<__is_swappable<_T>::value>::type
        swap(_T (&__a)[_Nm], _T (&__b)[_Nm]) noexcept(__is_nothrow_swappable<_T>::value);

namespace __swappable_details
{
using std::swap;

struct __do_is_swappable_impl
{
    template <typename _T, typename = decltype(swap(std::declval<_T &>(), std::declval<_T &>()))>
    static true_type __test(int);

    template <typename>
    static false_type __test(...);
};

struct __do_is_nothrow_swappable_impl
{
    template <typename _T>
    static __bool_constant<
        noexcept(swap(std::declval<_T &>(), std::declval<_T &>()))>
    __test(int);

    template <typename>
    static false_type __test(...);
};

}  // namespace __swappable_details

template <typename _T>
struct __is_swappable_impl
    : public __swappable_details::__do_is_swappable_impl
{
    typedef decltype(__test<_T>(0)) type;
};

template <typename _T>
struct __is_nothrow_swappable_impl
    : public __swappable_details::__do_is_nothrow_swappable_impl
{
    typedef decltype(__test<_T>(0)) type;
};

template <typename _T>
struct __is_swappable
    : public __is_swappable_impl<_T>::type
{
};

template <typename _T>
struct __is_nothrow_swappable
    : public __is_nothrow_swappable_impl<_T>::type
{
};

template <typename _T>
struct is_swappable
    : public __is_swappable_impl<_T>::type
{
};

//=============================================================================
// std::is_nothrow_swappable
//=============================================================================

template <typename _T>
struct is_nothrow_swappable
    : public __is_nothrow_swappable_impl<_T>::type
{
};

//=============================================================================
// std::is_swappable_with
//=============================================================================

namespace __swappable_with_details
{
using std::swap;

struct __do_is_swappable_with_impl
{
    template <typename _T, typename _U, typename = decltype(swap(std::declval<_T>(), std::declval<_U>())),
              typename = decltype(swap(std::declval<_U>(), std::declval<_T>()))>
    static true_type __test(int);

    template <typename, typename>
    static false_type __test(...);
};

struct __do_is_nothrow_swappable_with_impl
{
    template <typename _T, typename _U>
    static __bool_constant<
        noexcept(swap(std::declval<_T>(), std::declval<_U>())) &&noexcept(swap(std::declval<_U>(), std::declval<_T>()))>
    __test(int);

    template <typename, typename>
    static false_type __test(...);
};

}  // namespace __swappable_with_details

template <typename _T, typename _U>
struct __is_swappable_with_impl
    : public __swappable_with_details::__do_is_swappable_with_impl
{
    typedef decltype(__test<_T, _U>(0)) type;
};

// Optimization for the homogenous lvalue case, not required:
template <typename _T>
struct __is_swappable_with_impl<_T &, _T &>
    : public __swappable_details::__do_is_swappable_impl
{
    typedef decltype(__test<_T &>(0)) type;
};

template <typename _T, typename _U>
struct __is_nothrow_swappable_with_impl
    : public __swappable_with_details::__do_is_nothrow_swappable_with_impl
{
    typedef decltype(__test<_T, _U>(0)) type;
};

// Optimization for the homogenous lvalue case, not required:
template <typename _T>
struct __is_nothrow_swappable_with_impl<_T &, _T &>
    : public __swappable_details::__do_is_nothrow_swappable_impl
{
    typedef decltype(__test<_T &>(0)) type;
};

template <typename _T, typename _U>
struct is_swappable_with
    : public __is_swappable_with_impl<_T, _U>::type
{
};

//=============================================================================
// std::is_nothrow_swappable_with
//=============================================================================

template <typename _T, typename _U>
struct is_nothrow_swappable_with
    : public __is_nothrow_swappable_with_impl<_T, _U>::type
{
};

//=============================================================================
// std::is_invocable
//=============================================================================

template <typename _Result, typename _Ret, typename = void>
struct __is_invocable_impl : false_type
{
};

template <typename _Result, typename _Ret>
struct __is_invocable_impl<_Result, _Ret, __void_t<typename _Result::type>>
    : __or_<is_void<_Ret>, is_convertible<typename _Result::type, _Ret>>::type
{
};

template <typename _Fn, typename... _ArgTypes>
struct __is_invocable
    : __is_invocable_impl<__invoke_result<_Fn, _ArgTypes...>, void>::type
{
};

template <typename _Fn, typename _T, typename... _Args>
constexpr bool __call_is_nt(__invoke_memfun_ref)
{
    using _U = typename __inv_unwrap<_T>::type;
    return noexcept((std::declval<_U>().*std::declval<_Fn>())(
        std::declval<_Args>()...));
}

template <typename _Fn, typename _T, typename... _Args>
constexpr bool __call_is_nt(__invoke_memfun_deref)
{
    return noexcept(((*std::declval<_T>()).*std::declval<_Fn>())(
        std::declval<_Args>()...));
}

template <typename _Fn, typename _T>
constexpr bool __call_is_nt(__invoke_memobj_ref)
{
    using _U = typename __inv_unwrap<_T>::type;
    return noexcept(std::declval<_U>().*std::declval<_Fn>());
}

template <typename _Fn, typename _T>
constexpr bool __call_is_nt(__invoke_memobj_deref)
{
    return noexcept((*std::declval<_T>()).*std::declval<_Fn>());
}

template <typename _Fn, typename... _Args>
constexpr bool __call_is_nt(__invoke_other)
{
    return noexcept(std::declval<_Fn>()(std::declval<_Args>()...));
}

template <typename _Result, typename _Fn, typename... _Args>
struct __call_is_nothrow
    : __bool_constant<
          std::__call_is_nt<_Fn, _Args...>(typename _Result::__invoke_type{})>
{
};

template <typename _Fn, typename... _Args>
using __call_is_nothrow_ = __call_is_nothrow<__invoke_result<_Fn, _Args...>, _Fn, _Args...>;

// __is_nothrow_invocable (std::is_nothrow_invocable for C++11)
template <typename _Fn, typename... _Args>
struct __is_nothrow_invocable
    : __and_<__is_invocable<_Fn, _Args...>,
             __call_is_nothrow_<_Fn, _Args...>>::type
{
};

struct __nonesuch
{
    __nonesuch() = delete;
    ~__nonesuch() = delete;
    __nonesuch(__nonesuch const &) = delete;
    void operator=(__nonesuch const &) = delete;
};

template <typename _Functor, typename... _ArgTypes>
struct invoke_result
    : public __invoke_result<_Functor, _ArgTypes...>
{
};

template <typename _Fn, typename... _Args>
using invoke_result_t = typename invoke_result<_Fn, _Args...>::type;

template <typename _Fn, typename... _ArgTypes>
struct is_invocable
    : __is_invocable_impl<__invoke_result<_Fn, _ArgTypes...>, void>::type
{
};

template <typename _Ret, typename _Fn, typename... _ArgTypes>
struct is_invocable_r
    : __is_invocable_impl<__invoke_result<_Fn, _ArgTypes...>, _Ret>::type
{
};

template <typename _Fn, typename... _ArgTypes>
struct is_nothrow_invocable
    : __and_<__is_invocable_impl<__invoke_result<_Fn, _ArgTypes...>, void>,
             __call_is_nothrow_<_Fn, _ArgTypes...>>::type
{
};

template <typename _Result, typename _Ret, typename = void>
struct __is_nt_invocable_impl : false_type
{
};

template <typename _Result, typename _Ret>
struct __is_nt_invocable_impl<_Result, _Ret,
                              __void_t<typename _Result::type>>
    : __or_<is_void<_Ret>,
            __and_<is_convertible<typename _Result::type, _Ret>,
                   is_nothrow_constructible<_Ret, typename _Result::type>>>
{
};

/// std::is_nothrow_invocable_r
template <typename _Ret, typename _Fn, typename... _ArgTypes>
struct is_nothrow_invocable_r
    : __and_<__is_nt_invocable_impl<__invoke_result<_Fn, _ArgTypes...>, _Ret>,
             __call_is_nothrow_<_Fn, _ArgTypes...>>::type
{
};

/// std::is_invocable_v
template <typename _Fn, typename... _Args>
inline constexpr bool is_invocable_v = is_invocable<_Fn, _Args...>::value;

/// std::is_nothrow_invocable_v
template <typename _Fn, typename... _Args>
inline constexpr bool is_nothrow_invocable_v = is_nothrow_invocable<_Fn, _Args...>::value;

/// std::is_invocable_r_v
template <typename _Fn, typename... _Args>
inline constexpr bool is_invocable_r_v = is_invocable_r<_Fn, _Args...>::value;

/// std::is_nothrow_invocable_r_v
template <typename _Fn, typename... _Args>
inline constexpr bool is_nothrow_invocable_r_v = is_nothrow_invocable_r<_Fn, _Args...>::value;

//=============================================================================
// Trait values
//=============================================================================

template <typename _T>
inline constexpr bool is_void_v = is_void<_T>::value;

template <typename _T>
inline constexpr bool is_null_pointer_v = is_null_pointer<_T>::value;

template <typename _T>
inline constexpr bool is_integral_v = is_integral<_T>::value;

template <typename _T>
inline constexpr bool is_floating_point_v = is_floating_point<_T>::value;

template <typename _T>
inline constexpr bool is_array_v = is_array<_T>::value;

template <typename _T>
inline constexpr bool is_pointer_v = is_pointer<_T>::value;

template <typename _T>
inline constexpr bool is_lvalue_reference_v =
    is_lvalue_reference<_T>::value;

template <typename _T>
inline constexpr bool is_rvalue_reference_v =
    is_rvalue_reference<_T>::value;

template <typename _T>
inline constexpr bool is_member_object_pointer_v =
    is_member_object_pointer<_T>::value;

template <typename _T>
inline constexpr bool is_member_function_pointer_v =
    is_member_function_pointer<_T>::value;

template <typename _T>
inline constexpr bool is_enum_v = is_enum<_T>::value;

template <typename _T>
inline constexpr bool is_union_v = is_union<_T>::value;

template <typename _T>
inline constexpr bool is_class_v = is_class<_T>::value;

template <typename _T>
inline constexpr bool is_function_v = is_function<_T>::value;

template <typename _T>
inline constexpr bool is_reference_v = is_reference<_T>::value;

template <typename _T>
inline constexpr bool is_arithmetic_v = is_arithmetic<_T>::value;

template <typename _T>
inline constexpr bool is_fundamental_v = is_fundamental<_T>::value;

template <typename _T>
inline constexpr bool is_object_v = is_object<_T>::value;

template <typename _T>
inline constexpr bool is_scalar_v = is_scalar<_T>::value;

template <typename _T>
inline constexpr bool is_compound_v = is_compound<_T>::value;

template <typename _T>
inline constexpr bool is_member_pointer_v = is_member_pointer<_T>::value;

template <typename _T>
inline constexpr bool is_const_v = is_const<_T>::value;

template <typename _T>
inline constexpr bool is_volatile_v = is_volatile<_T>::value;

template <typename _T>
inline constexpr bool is_trivial_v = is_trivial<_T>::value;

template <typename _T>
inline constexpr bool is_trivially_copyable_v =
    is_trivially_copyable<_T>::value;

template <typename _T>
inline constexpr bool is_standard_layout_v = is_standard_layout<_T>::value;

template <typename _T>
inline constexpr bool is_pod_v = is_pod<_T>::value;

template <typename _T>
inline constexpr bool is_literal_type_v = is_literal_type<_T>::value;

template <typename _T>
inline constexpr bool is_empty_v = is_empty<_T>::value;

template <typename _T>
inline constexpr bool is_polymorphic_v = is_polymorphic<_T>::value;

template <typename _T>
inline constexpr bool is_abstract_v = is_abstract<_T>::value;

template <typename _T>
inline constexpr bool is_final_v = is_final<_T>::value;

template <typename _T>
inline constexpr bool is_signed_v = is_signed<_T>::value;

template <typename _T>
inline constexpr bool is_unsigned_v = is_unsigned<_T>::value;

template <typename _T, typename... _Args>
inline constexpr bool is_constructible_v =
    is_constructible<_T, _Args...>::value;

template <typename _T>
inline constexpr bool is_default_constructible_v =
    is_default_constructible<_T>::value;

template <typename _T>
inline constexpr bool is_copy_constructible_v =
    is_copy_constructible<_T>::value;

template <typename _T>
inline constexpr bool is_move_constructible_v =
    is_move_constructible<_T>::value;

template <typename _T, typename _U>
inline constexpr bool is_assignable_v = is_assignable<_T, _U>::value;

template <typename _T>
inline constexpr bool is_copy_assignable_v = is_copy_assignable<_T>::value;

template <typename _T>
inline constexpr bool is_move_assignable_v = is_move_assignable<_T>::value;

template <typename _T>
inline constexpr bool is_destructible_v = is_destructible<_T>::value;

template <typename _T, typename... _Args>
inline constexpr bool is_trivially_constructible_v =
    is_trivially_constructible<_T, _Args...>::value;

template <typename _T>
inline constexpr bool is_trivially_default_constructible_v =
    is_trivially_default_constructible<_T>::value;

template <typename _T>
inline constexpr bool is_trivially_copy_constructible_v =
    is_trivially_copy_constructible<_T>::value;

template <typename _T>
inline constexpr bool is_trivially_move_constructible_v =
    is_trivially_move_constructible<_T>::value;

template <typename _T, typename _U>
inline constexpr bool is_trivially_assignable_v =
    is_trivially_assignable<_T, _U>::value;

template <typename _T>
inline constexpr bool is_trivially_copy_assignable_v =
    is_trivially_copy_assignable<_T>::value;

template <typename _T>
inline constexpr bool is_trivially_move_assignable_v =
    is_trivially_move_assignable<_T>::value;

template <typename _T>
inline constexpr bool is_trivially_destructible_v =
    is_trivially_destructible<_T>::value;

template <typename _T, typename... _Args>
inline constexpr bool is_nothrow_constructible_v =
    is_nothrow_constructible<_T, _Args...>::value;

template <typename _T>
inline constexpr bool is_nothrow_default_constructible_v =
    is_nothrow_default_constructible<_T>::value;

template <typename _T>
inline constexpr bool is_nothrow_copy_constructible_v =
    is_nothrow_copy_constructible<_T>::value;

template <typename _T>
inline constexpr bool is_nothrow_move_constructible_v =
    is_nothrow_move_constructible<_T>::value;

template <typename _T, typename _U>
inline constexpr bool is_nothrow_assignable_v =
    is_nothrow_assignable<_T, _U>::value;

template <typename _T>
inline constexpr bool is_nothrow_copy_assignable_v =
    is_nothrow_copy_assignable<_T>::value;

template <typename _T>
inline constexpr bool is_nothrow_move_assignable_v =
    is_nothrow_move_assignable<_T>::value;

template <typename _T>
inline constexpr bool is_nothrow_destructible_v =
    is_nothrow_destructible<_T>::value;

template <typename _T>
inline constexpr bool has_virtual_destructor_v =
    has_virtual_destructor<_T>::value;

template <typename _T>
inline constexpr size_t alignment_of_v = alignment_of<_T>::value;

template <typename _T>
inline constexpr size_t rank_v = rank<_T>::value;

template <typename _T, unsigned _Idx = 0>
inline constexpr size_t extent_v = extent<_T, _Idx>::value;

template <typename _T, typename _U>
inline constexpr bool is_same_v = is_same<_T, _U>::value;

template <typename _Base, typename _Derived>
inline constexpr bool is_base_of_v = is_base_of<_Base, _Derived>::value;

template <typename _From, typename _To>
inline constexpr bool is_convertible_v = is_convertible<_From, _To>::value;

}  // namespace OS6STD

OS6STD_END_SYSTEM_HEADER

#endif

//=============================================================================
// End of file
//=============================================================================
