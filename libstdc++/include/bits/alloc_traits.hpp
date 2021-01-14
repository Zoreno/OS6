/**
 * @file alloc_traits.hpp
 * @author Joakim Bertils
 * @version 0.1
 * @date 2021-01-06
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

#ifndef _STL_CONSTRUCT_HPP
#define _STL_CONSTRUCT_HPP

#pragma GCC system_header

#include <bits/stl_construct.hpp>
#include <bits/memoryfwd.hpp>
#include <bits/allocator.hpp>
#include <bits/ptr_traits.hpp>
#include <bits/numeric_traits.hpp>

OS6STD_BEGIN_SYSTEM_HEADER

namespace OS6STD
{
//=============================================================================
// Allocator traits base
//=============================================================================

struct __allocator_traits_base
{
    template <typename _T, typename _U, typename = void>
    struct __rebind : __replace_first_arg<_T, _U>
    {
    };

    template <typename _T, typename _U>
    struct __rebind<_T, _U, __void_t<typename _T::template rebind<_U>::other>>
    {
        using type = typename _T::template rebind<_U>::other;
    };

protected:
    template <typename _T>
    using __pointer = typename _T::pointer;

    template <typename _T>
    using __c_pointer = typename _T::const_pointer;

    template <typename _T>
    using __v_pointer = typename _T::void_pointer;

    template <typename _T>
    using __cv_pointer = typename _T::const_void_pointer;

    template <typename _T>
    using __pocca = typename _T::propagate_on_container_copy_assignment;

    template <typename _T>
    using __pocma = typename _T::propagate_on_container_move_assignment;

    template <typename _T>
    using __pocs = typename _T::propagate_on_container_swap;

    template <typename _T>
    using __equal = typename _T::is_always_equal;
};

template <typename _Alloc, typename _U>
using __alloc_rebind = typename __allocator_traits_base::template __rebind<_Alloc, _U>::type;

//=============================================================================
// Allocator Traits
//=============================================================================

template <typename _Alloc>
struct allocator_traits : __allocator_traits_base
{
    using allocator_type = _Alloc;
    using value_type = typename _Alloc::value_type;

    using pointer = __detected_or_t<value_type *, __pointer, _Alloc>;

private:
    template <template <typename> class _Func, typename _T, typename = void>
    struct _Ptr
    {
        using type = typename pointer_traits<pointer>::template rebind<_T>;
    };

    template <template <typename> class _Func, typename _T>
    struct _Ptr<_Func, _T, __void_t<_Func<_Alloc>>>
    {
        using type = _Func<_Alloc>;
    };

    template <typename _T, typename _PtrT, typename = void>
    struct _Diff
    {
        using type = typename pointer_traits<_PtrT>::difference_type;
    };

    template <typename _T, typename _PtrT>
    struct _Diff<_T, _PtrT, __void_t<typename _T::difference_type>>
    {
        using type = typename _T::difference_type;
    };

    template <typename _T, typename _DiffT, typename = void>
    struct _Size : make_unsigned<_DiffT>
    {
    };

    template <typename _T, typename _DiffT>
    struct _Size<_T, _DiffT, __void_t<typename _T::size_type>>
    {
        using type = typename _T::size_type;
    };

public:
    using const_pointer = typename _Ptr<__c_pointer, const value_type>::type;
    using void_pointer = typename _Ptr<__v_pointer, void>::type;
    using const_void_pointer = typename _Ptr<__cv_pointer, const void>::type;
    using difference_type = typename _Diff<_Alloc, pointer>::type;
    using size_type = typename _Size<_Alloc, difference_type>::type;
    using propagate_on_container_copy_assignment = __detected_or_t<false_type, __pocca, _Alloc>;
    using propagate_on_container_move_assignment = __detected_or_t<false_type, __pocma, _Alloc>;
    using propagate_on_container_swap = __detected_or_t<false_type, __pocs, _Alloc>;
    using is_always_equal = __detected_or_t<typename is_empty<_Alloc>::type, __equal, _Alloc>;

    template <typename _T>
    using rebind_alloc = __alloc_rebind<_Alloc, _T>;

    template <typename _T>
    using rebind_traits = allocator_traits<rebind_alloc<_T>>;

private:
    template <typename _Alloc2>
    static constexpr auto _S_allocate(_Alloc2 &__a, size_type __n, const_void_pointer __hint, int)
        -> decltype(__a.allocate(__n, __hint))
    {
        return __a.allocate(__n, __hint);
    }

    template <typename _Alloc2>
    static constexpr pointer _S_allocate(_Alloc2 &__a, size_type __n, const_void_pointer, ...)
    {
        return __a.allocate(__n);
    }

    template <typename _T, typename... _Args>
    struct __construct_helper
    {
        template <typename _Alloc2,
                  typename = decltype(std::declval<_Alloc2 *>()->construct(std::declval<_T *>(), std::declval<_Args>()...))>
        static true_type __test(int);

        template <typename>
        static false_type __test(...);

        using type = decltype(__test<_Alloc>(0));
    };

    template <typename _T, typename... _Args>
    using __has_construct = typename __construct_helper<_T, _Args...>::type;

    template <typename _T, typename... _Args>
    static _Require<__and_<__not_<__has_construct<_T, _Args...>>,
                           is_constructible<_T, _Args...>>>
    _S_construct(_Alloc &, _T *__p, _Args &&... __args) noexcept(std::is_nothrow_constructible<_T, _Args...>::value)
    {
        ::new ((void *)__p) _T(std::forward<_Args>(__args)...);
    }

    template <typename _Alloc2, typename _T>
    static auto _S_destroy(_Alloc2 &__a, _T *__p, int) noexcept(noexcept(__a.destroy(__p))) -> decltype(__a.destroy(__p))
    {
        __a.destroy(__p);
    }

    template <typename _Alloc2, typename _T>
    static void _S_destroy(_Alloc2 &, _T *__p, ...) noexcept(std::is_nothrow_destructible<_T>::value)
    {
        _destroy(__p);
    }

    template <typename _Alloc2>
    static constexpr auto _S_max_size(_Alloc2 &__a, int)
        -> decltype(__a.max_size())
    {
        return __a.max_size();
    }

    template <typename _Alloc2>
    static constexpr size_type _S_max_size(_Alloc2 &, ...)
    {
        return __numeric_traits<size_type>::__max / sizeof(value_type);
    }

    template <typename _Alloc2>
    static constexpr auto _S_select(_Alloc2 &__a, int) -> decltype(__a.select_on_container_copy_construction())
    {
        return __a.select_on_container_copy_construction();
    }

    template <typename _Alloc2>
    static constexpr _Alloc2 _S_select(_Alloc2 &__a, ...)
    {
        return __a;
    }

public:
    static pointer allocate(_Alloc &__a, size_type __n)
    {
        return __a.allocate(__n);
    }

    static pointer allocate(_Alloc &__a, size_type __n, const_void_pointer __hint)
    {
        return _S_allocate(__a, __n, __hint, 0);
    }

    static void deallocate(_Alloc &__a, pointer __p, size_type __n)
    {
        __a.deallocate(__p, __n);
    }

    template <typename _T, typename... _Args>
    static auto construct(_Alloc &__a, _T *__p, _Args &&... __args) noexcept(noexcept(_S_construct(__a, __p, std::forward<_Args>(__args)...)))
        -> decltype(_S_construct(__a, __p, std::forward<_Args>(__args)...))
    {
        _S_construct(__a, __p, std::forward<_Args>(__args)...);
    }

    template <typename _T>
    static void destroy(_Alloc &__a, _T *__p) noexcept(noexcept(_S_destroy(__a, __p, 0)))
    {
        _S_destroy(__a, __p, 0);
    }

    static size_type max_size(const _Alloc &__a) noexcept
    {
        return _S_max_size(__a, 0);
    }

    static _Alloc
    select_on_container_copy_construction(const _Alloc &__rhs)
    {
        return _S_select(__rhs, 0);
    }
};

template <typename _T>
struct allocator_traits<allocator<_T>>
{
    using allocator_type = allocator<_T>;
    using value_type = _T;
    using pointer = _T *;
    using const_pointer = const _T *;
    using void_pointer = void *;
    using const_void_pointer = const void *;
    using difference_type = std::ptrdiff_t;
    using size_type = std::size_t;
    using propagate_on_container_copy_assignment = false_type;
    using propagate_on_container_move_assignment = true_type;
    using is_always_equal = true_type;
    template <typename _U>
    using rebind_alloc = allocator<_U>;
    template <typename _U>
    using rebind_traits = allocator_traits<allocator<_U>>;

    static pointer allocate(allocator_type &__a, size_type __n)
    {
        return __a.allocate(__n);
    }

    static pointer allocate(allocator_type &__a, size_type __n, const_void_pointer __hint)
    {
        return __a.allocate(__n, __hint);
    }

    static void deallocate(allocator_type &__a, pointer __p, size_type __n)
    {
        __a.deallocate(__p, __n);
    }

    template <typename _U, typename... _Args>
    static void construct(allocator_type &__a __attribute__((__unused__)),
                          _U *__p,
                          _Args &&... __args) noexcept(std::is_nothrow_constructible<_U, _Args...>::value)
    {
        __a.construct(__p, std::forward<_Args>(__args)...);
    }

    template <typename _U>
    static void destroy(allocator_type &__a __attribute__((__unused__)),
                        _U *__p) noexcept(is_nothrow_destructible<_U>::value)
    {
        __a.destroy(__p);
    }

    static size_type max_size(const allocator_type &__a __attribute__((__unused__))) noexcept
    {
        return __a.max_size();
    }

    static allocator_type select_on_container_copy_construction(const allocator_type &__rhs)
    {
        return __rhs;
    }
};

template <typename _Alloc, typename = typename _Alloc::value_type>
struct __alloc_traits : allocator_traits<_Alloc>
{
    using allocator_type = _Alloc;
    using _base_type = allocator_traits<_Alloc>;
    using value_type = typename _base_type::value_type;
    using pointer = typename _base_type::pointer;
    using const_pointer = typename _base_type::const_pointer;
    using size_type = typename _base_type::size_type;
    using difference_type = typename _base_type::difference_type;
    using reference = value_type &;
    using const_reference = const value_type &;

    using _base_type::allocate;
    using _base_type::construct;
    using _base_type::deallocate;
    using _base_type::destroy;
    using _base_type::max_size;

private:
    template <typename _Ptr>
    using __is_custom_pointer = std::__and_<std::is_same<pointer, _Ptr>,
                                            std::__not_<std::is_pointer<_Ptr>>>;

public:
    // overload construct for non-standard pointer types
    template <typename _Ptr, typename... _Args>
    static std::__enable_if_t<__is_custom_pointer<_Ptr>::value>
    construct(_Alloc &__a, _Ptr __p, _Args &&... __args) noexcept(noexcept(_base_type::construct(__a, std::__to_address(__p), std::forward<_Args>(__args)...)))
    {
        _base_type::construct(__a, std::__to_address(__p), std::forward<_Args>(__args)...);
    }

    // overload destroy for non-standard pointer types
    template <typename _Ptr>
    static std::__enable_if_t<__is_custom_pointer<_Ptr>::value>
    destroy(_Alloc &__a, _Ptr __p) noexcept(noexcept(_base_type::destroy(__a, std::__to_address(__p))))
    {
        _base_type::destroy(__a, std::__to_address(__p));
    }

    static constexpr _Alloc _select_on_copy(const _Alloc &__a)
    {
        return _base_type::select_on_container_copy_construction(__a);
    }

    /*
    static void _S_on_swap(_Alloc &__a, _Alloc &__b)
    {
        std::__alloc_on_swap(__a, __b);
    }
    */

    static constexpr bool _propagate_on_copy_assign()
    {
        return _base_type::propagate_on_container_copy_assignment::value;
    }

    static constexpr bool _propagate_on_move_assign()
    {
        return _base_type::propagate_on_container_move_assignment::value;
    }

    static constexpr bool _propagate_on_swap()
    {
        return _base_type::propagate_on_container_swap::value;
    }

    static constexpr bool _always_equal()
    {
        return _base_type::is_always_equal::value;
    }

    static constexpr bool _nothrow_move()
    {
        return _propagate_on_move_assign() || _always_equal();
    }

    template <typename _Tp>
    struct rebind
    {
        typedef typename _base_type::template rebind_alloc<_Tp> other;
    };
};

}  // namespace OS6STD

OS6STD_END_SYSTEM_HEADER

#endif

//=============================================================================
// End of file
//=============================================================================
