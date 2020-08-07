/**
 * @file allocator.hpp
 * @author Joakim Bertils
 * @version 0.1
 * @date 2020-07-29
 * 
 * @brief Implements the standard allocator interface.
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

#ifndef __ALLOCATOR_HPP
#define __ALLOCATOR_HPP

#pragma GCC system_header

#include <bits/allocator_base.hpp>
#include <bits/memoryfwd.hpp>

#include <type_traits>

OS6STD_BEGIN_SYSTEM_HEADER

namespace OS6STD
{

template <>
class allocator<void>
{
public:
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using pointer = void *;
    using const_pointer = const void *;
    using value_type = void;

    template <typename _U>
    struct rebind
    {
        using other = allocator<_U>;
    };

    using propagate_on_container_move_assignment = true_type;
    using is_always_equal = true_type;

    template <typename _U, typename... _Args>
    void construct(_U *__p, _Args &&... __args)
    {
        ::new ((void *)__p) _U(std::forward<_Args>(__args)...);
    }

    template <typename _U>
    void destroy(_U *__p)
    {
        __p->~_U();
    }
};

template <typename _T>
class allocator : public __allocator_base<_T>
{
public:
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using pointer = _T *;
    using const_pointer = const _T *;
    using reference = _T &;
    using const_reference = const _T &;
    using value_type = _T;

    template <typename _U>
    struct rebind
    {
        using other = allocator<_U>;
    };

    using propagate_on_container_move_assignment = true_type;
    using is_always_equal = true_type;

    constexpr allocator() noexcept
    {
    }

    constexpr allocator(const allocator &__a) noexcept
        : __allocator_base<_T>(__a)
    {
    }

    allocator &operator=(const allocator &) = default;

    template <typename _U>
    constexpr allocator(const allocator<_U> &) noexcept
    {
    }

    ~allocator() noexcept
    {
    }
};

template <typename _T1, typename _T2>
inline bool operator==(const allocator<_T1> &, const allocator<_T2> &) noexcept
{
    return true;
}

template <typename _T>
inline bool operator==(const allocator<_T> &, const allocator<_T> &) noexcept
{
    return true;
}

template <typename _T1, typename _T2>
inline bool operator!=(const allocator<_T1> &, const allocator<_T2> &) noexcept
{
    return false;
}

template <typename _T>
inline bool operator!=(const allocator<_T> &, const allocator<_T> &) noexcept
{
    return false;
}

template <typename _T>
class allocator<const _T>
{
public:
    typedef _T value_type;
    template <typename _U>
    allocator(const allocator<_U> &) {}
};

template <typename _T>
class allocator<volatile _T>
{
public:
    typedef _T value_type;
    template <typename _U>
    allocator(const allocator<_U> &) {}
};

template <typename _T>
class allocator<const volatile _T>
{
public:
    typedef _T value_type;
    template <typename _U>
    allocator(const allocator<_U> &) {}
};

template <typename _Alloc, bool = __is_empty(_Alloc)>
struct __alloc_swap
{
    static void _S_do_it(_Alloc &, _Alloc &) noexcept
    {
    }
};

template <typename _Alloc>
struct __alloc_swap<_Alloc, false>
{
    static void _S_do_it(_Alloc &__one, _Alloc &__two) noexcept
    {
        if (__one != __two)
        {
            swap(__one, __two);
        }
    }
};

template <typename _Alloc, bool = __is_empty(_Alloc)>
struct __alloc_neq
{
    static bool _S_do_it(const _Alloc &, const _Alloc &)
    {
        return false;
    }
};

template <typename _Alloc>
struct __alloc_neq<_Alloc, false>
{
    static bool _S_do_it(const _Alloc &__one, const _Alloc &__two)
    {
        return __one != __two;
    }
};

template <typename _Tp, bool = __or_<is_copy_constructible<typename _Tp::value_type>,
                                     is_nothrow_move_constructible<typename _Tp::value_type>>::value>
struct __shrink_to_fit_aux
{
    static bool _S_do_it(_Tp &) noexcept { return false; }
};

template <typename _Tp>
struct __shrink_to_fit_aux<_Tp, true>
{
    static bool _S_do_it(_Tp &__c) noexcept
    {
        return false;
    }
};

} // namespace OS6STD

OS6STD_END_SYSTEM_HEADER

#endif