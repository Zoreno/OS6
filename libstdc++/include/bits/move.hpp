/**
 * @file move.hpp
 * @author Joakim Bertils
 * @version 0.1
 * @date 2020-07-28
 * 
 * @brief Move and forward semantics
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

#ifndef __MOVE_HPP
#define __MOVE_HPP

#pragma GCC system_header

#include <bits/c++config.hpp>

OS6STD_BEGIN_SYSTEM_HEADER

namespace OS6STD
{
template <typename _T>
inline constexpr _T *__addressof(_T &__r) noexcept
{
    return __builtin_addressof(__r);
}

}  // namespace OS6STD

#include <type_traits>

namespace OS6STD
{
template <typename _T>
constexpr _T &&forward(typename std::remove_reference<_T>::type &__t) noexcept
{
    return static_cast<_T &&>(__t);
}

template <typename _T>
constexpr _T &&forward(typename std::remove_reference<_T>::type &&__t) noexcept
{
    static_assert(!std::is_lvalue_reference<_T>::value,
                  "template argument substituting _T is an lvalue reference type");

    return static_cast<_T &&>(__t);
}

template <typename _T>
constexpr typename std::remove_reference<_T>::type &&move(_T &&__t) noexcept
{
    return static_cast<typename std::remove_reference<_T>::type &&>(__t);
}

template <typename _T>
struct __move_if_noexcept_cond
    : public __and_<__not_<is_nothrow_move_constructible<_T>>,
                    is_copy_constructible<_T>>::type
{
};

template <typename _T>
constexpr typename conditional<__move_if_noexcept_cond<_T>::value, const _T &, _T &&>::type
move_if_noexcept(_T &__x) noexcept
{
    return std::move(__x);
}

template <typename _T>
inline constexpr _T *addressof(_T &__r) noexcept
{
    return std::__addressof(__r);
}

template <typename _T>
const _T *addressof(const _T &&) = delete;

template <typename _Tp, typename _Up = _Tp>
inline _Tp
__exchange(_Tp &__obj, _Up &&__new_val)
{
    _Tp __old_val = std::move(__obj);
    __obj = std::forward<_Up>(__new_val);
    return __old_val;
}

template <typename _Tp>
inline typename enable_if<__and_<__not_<__is_tuple_like<_Tp>>,
                                 is_move_constructible<_Tp>,
                                 is_move_assignable<_Tp>>::value>::type
swap(_Tp &__a, _Tp &__b) noexcept(__and_<is_nothrow_move_constructible<_Tp>,
                                         is_nothrow_move_assignable<_Tp>>::value)

{
    _Tp __tmp = std::move(__a);
    __a = std::move(__b);
    __b = std::move(__tmp);
}

template <typename _Tp, size_t _Nm>
inline
    typename enable_if<__is_swappable<_Tp>::value>::type
        swap(_Tp (&__a)[_Nm], _Tp (&__b)[_Nm]) noexcept(__is_nothrow_swappable<_Tp>::value)
{
    for (size_t __n = 0; __n < _Nm; ++__n)
        swap(__a[__n], __b[__n]);
}

}  // namespace OS6STD

OS6STD_END_SYSTEM_HEADER

#endif

//=============================================================================
// End of file
//=============================================================================
