/**
 * @file utility.hpp
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

#ifndef __UTILITY_HPP
#define __UTILITY_HPP

#pragma GCC system_header

#include <bits/c++config.hpp>

OS6STD_BEGIN_SYSTEM_HEADER

namespace OS6STD
{
template <typename _T>
struct tuple_size;

template <typename _T,
          typename _U = typename remove_cv<_T>::type,
          typename = typename enable_if<is_same<_T, _U>::value>::type,
          size_t = tuple_size<_T>::value>
using __enable_if_has_tuple_size = _T;

template <typename _T>
struct tuple_size<const __enable_if_has_tuple_size<_T>>
    : public tuple_size<_T>
{
};

template <typename _T>
struct tuple_size<volatile __enable_if_has_tuple_size<_T>>
    : public tuple_size<_T>
{
};

template <typename _T>
struct tuple_size<const volatile __enable_if_has_tuple_size<_T>>
    : public tuple_size<_T>
{
};

template <std::size_t __i, typename _T>
struct tuple_element;

template <std::size_t __i, typename _T>
using __tuple_element = typename tuple_element<__i, _T>::type;

template <std::size_t __i, typename _T>
struct tuple_element<__i, const _T>
{
    typedef typename add_const<__tuple_element<__i, _T>>::type type;
};

template <std::size_t __i, typename _T>
struct tuple_element<__i, volatile _T>
{
    typedef typename add_volatile<__tuple_element<__i, _T>>::type type;
};

template <std::size_t __i, typename _T>
struct tuple_element<__i, const volatile _T>
{
    typedef typename add_cv<__tuple_element<__i, _T>>::type type;
};

template <std::size_t __i, typename _T>
using tuple_element_t = typename tuple_element<__i, _T>::type;

template <typename _T1, typename _T2>
struct __is_tuple_like_impl<pair<_T1, _T2>>
    : true_type
{
};

template <class _T1, class _T2>
struct tuple_size<pair<_T1, _T2>>
    : public integral_constant<std::size_t, 2>
{
};

template <class _T1, class _T2>
struct tuple_element<0, pair<_T1, _T2>>
{
    using type = _T1;
};

template <class _T1, class _T2>
struct tuple_element<1, pair<_T1, _T2>>
{
    using type = _T2;
};

template <std::size_t _Int>
struct __pair_get;

template <>
struct __pair_get<0>
{
    template <typename _T1, typename _T2>
    static constexpr _T1 &
    __get(std::pair<_T1, _T2> &__pair) noexcept
    {
        return __pair.first;
    }

    template <typename _T1, typename _T2>
    static constexpr _T1 &&
    __move_get(std::pair<_T1, _T2> &&__pair) noexcept
    {
        return std::forward<_T1>(__pair.first);
    }

    template <typename _T1, typename _T2>
    static constexpr const _T1 &
    __const_get(const std::pair<_T1, _T2> &__pair) noexcept
    {
        return __pair.first;
    }

    template <typename _T1, typename _T2>
    static constexpr const _T1 &&
    __const_move_get(const std::pair<_T1, _T2> &&__pair) noexcept
    {
        return std::forward<const _T1>(__pair.first);
    }
};

template <>
struct __pair_get<1>
{
    template <typename _T1, typename _T2>
    static constexpr _T2 &
    __get(std::pair<_T1, _T2> &__pair) noexcept
    {
        return __pair.second;
    }

    template <typename _T1, typename _T2>
    static constexpr _T2 &&
    __move_get(std::pair<_T1, _T2> &&__pair) noexcept
    {
        return std::forward<_T2>(__pair.second);
    }

    template <typename _T1, typename _T2>
    static constexpr const _T2 &
    __const_get(const std::pair<_T1, _T2> &__pair) noexcept
    {
        return __pair.second;
    }

    template <typename _T1, typename _T2>
    static constexpr const _T2 &&
    __const_move_get(const std::pair<_T1, _T2> &&__pair) noexcept
    {
        return std::forward<const _T2>(__pair.second);
    }
};

template <std::size_t _Int, class _T1, class _T2>
constexpr typename tuple_element<_Int, std::pair<_T1, _T2>>::type &
get(std::pair<_T1, _T2> &__in) noexcept
{
    return __pair_get<_Int>::__get(__in);
}

template <std::size_t _Int, class _T1, class _T2>
constexpr typename tuple_element<_Int, std::pair<_T1, _T2>>::type &&
get(std::pair<_T1, _T2> &&__in) noexcept
{
    return __pair_get<_Int>::__move_get(std::move(__in));
}

template <std::size_t _Int, class _T1, class _T2>
constexpr const typename tuple_element<_Int, std::pair<_T1, _T2>>::type &
get(const std::pair<_T1, _T2> &__in) noexcept
{
    return __pair_get<_Int>::__const_get(__in);
}

template <std::size_t _Int, class _T1, class _T2>
constexpr const typename tuple_element<_Int, std::pair<_T1, _T2>>::type &&
get(const std::pair<_T1, _T2> &&__in) noexcept
{
    return __pair_get<_Int>::__const_move_get(std::move(__in));
}

template <typename _T, typename _U>
constexpr _T &
get(pair<_T, _U> &__p) noexcept
{
    return __p.first;
}

template <typename _T, typename _U>
constexpr const _T &
get(const pair<_T, _U> &__p) noexcept
{
    return __p.first;
}

template <typename _T, typename _U>
constexpr _T &&
get(pair<_T, _U> &&__p) noexcept
{
    return std::move(__p.first);
}

template <typename _T, typename _U>
constexpr const _T &&
get(const pair<_T, _U> &&__p) noexcept
{
    return std::move(__p.first);
}

template <typename _T, typename _U>
constexpr _T &
get(pair<_U, _T> &__p) noexcept
{
    return __p.second;
}

template <typename _T, typename _U>
constexpr const _T &
get(const pair<_U, _T> &__p) noexcept
{
    return __p.second;
}

template <typename _T, typename _U>
constexpr _T &&
get(pair<_U, _T> &&__p) noexcept
{
    return std::move(__p.second);
}

template <typename _T, typename _U>
constexpr const _T &&
get(const pair<_U, _T> &&__p) noexcept
{
    return std::move(__p.second);
}

template <typename _T, typename _U = _T>
inline _T
exchange(_T &__obj, _U &&__new_val)
{
    return std::__exchange(__obj, std::forward<_U>(__new_val));
}

template <size_t... _Indexes>
struct _Index_tuple
{
};

template <size_t _Num>
struct _Build_index_tuple
{
    using __type = _Index_tuple<__integer_pack(_Num)...>;
};

template <typename _T, _T... _Idx>
struct integer_sequence
{
    typedef _T value_type;
    static constexpr size_t size() noexcept { return sizeof...(_Idx); }
};

template <typename _T, _T _Num>
using make_integer_sequence = integer_sequence<_T, __integer_pack(_Num)...>;

template <size_t... _Idx>
using index_sequence = integer_sequence<size_t, _Idx...>;

template <size_t _Num>
using make_index_sequence = make_integer_sequence<size_t, _Num>;

template <typename... _Types>
using index_sequence_for = make_index_sequence<sizeof...(_Types)>;

struct in_place_t
{
    explicit in_place_t() = default;
};

inline constexpr in_place_t in_place{};

template <typename _T>
struct in_place_type_t
{
    explicit in_place_type_t() = default;
};

template <typename _T>
inline constexpr in_place_type_t<_T> in_place_type{};

template <size_t _Idx>
struct in_place_index_t
{
    explicit in_place_index_t() = default;
};

template <size_t _Idx>
inline constexpr in_place_index_t<_Idx> in_place_index{};

template <typename>
struct __is_in_place_type_impl : false_type
{
};

template <typename _T>
struct __is_in_place_type_impl<in_place_type_t<_T>> : true_type
{
};

template <typename _T>
struct __is_in_place_type
    : public __is_in_place_type_impl<_T>
{
};

#define __cpp_lib_as_const 201510
template <typename _T>
constexpr add_const_t<_T> &as_const(_T &__t) noexcept
{
    return __t;
}

template <typename _T>
void as_const(const _T &&) = delete;

}  // namespace OS6STD

OS6STD_END_SYSTEM_HEADER

#endif

//=============================================================================
// End of file
//=============================================================================
