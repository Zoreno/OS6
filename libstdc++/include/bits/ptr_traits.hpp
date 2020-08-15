/**
 * @file ptr_traits.hpp
 * @author Joakim Bertils
 * @version 0.1
 * @date 2020-07-28
 * 
 * @brief Pointer traits.
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

#ifndef __PTR_TRAITS_HPP
#define __PTR_TRAITS_HPP

#pragma GCC system_header

#include <bits/c++config.hpp>

OS6STD_BEGIN_SYSTEM_HEADER

#include <bits/move.hpp>

namespace OS6STD
{
class __undefined;

template <typename _T>
struct __get_first_arg
{
    using type = __undefined;
};

template <template <typename, typename...> class _Template,
          typename _T,
          typename... _Types>
struct __get_first_arg<_Template<_T, _Types...>>
{
    using type = _T;
};

template <typename _T>
using __get_first_arg_t = typename __get_first_arg<_T>::type;

template <typename _T, typename _U>
struct __replace_first_arg
{
};

template <template <typename, typename...> class _Template,
          typename _U,
          typename _T,
          typename... _Types>
struct __replace_first_arg<_Template<_T, _Types...>, _U>
{
    using type = _Template<_U, _Types...>;
};

template <typename _T, typename _U>
using __replace_first_arg_t = typename __replace_first_arg<_T, _U>::type;

template <typename _T>
using __make_not_void = typename conditional<is_void<_T>::value, __undefined, _T>::type;

template <typename _Ptr>
struct pointer_traits
{
private:
    template <typename _T>
    using __element_type = typename _T::element_type;

    template <typename _T>
    using __difference_type = typename _T::difference_type;

    template <typename _T, typename _U, typename = void>
    struct __rebind : __replace_first_arg<_T, _U>
    {
    };

    template <typename _T, typename _U>
    struct __rebind<_T, _U, __void_t<typename _T::template rebind<_U>>>
    {
        using type = typename _T::template rebind<_U>;
    };

public:
    using pointer = _Ptr;
    using element_type = __detected_or_t<__get_first_arg_t<_Ptr>, __element_type, _Ptr>;
    using difference_type = __detected_or_t<ptrdiff_t, __difference_type, _Ptr>;
    template <typename _U>
    using rebind = typename __rebind<_Ptr, _U>::type;

    static _Ptr pointer_to(__make_not_void<element_type> &__e)
    {
        return _Ptr::pointer_to(__e);
    }

    static_assert(!is_same<element_type, __undefined>::value,
                  "pointer type defines element_type or is like SomePointer<T, Args>");
};

template <typename _T>
struct pointer_traits<_T *>
{
    typedef _T *pointer;
    typedef _T element_type;
    typedef ptrdiff_t difference_type;

    template <typename _U>
    using rebind = _U *;

    static pointer pointer_to(__make_not_void<element_type> &__r) noexcept
    {
        return std::addressof(__r);
    }
};

template <typename _Ptr, typename _T>
using __ptr_rebind = typename pointer_traits<_Ptr>::template rebind<_T>;

template <typename _T>
constexpr _T *__to_address(_T *__ptr) noexcept
{
    static_assert(!std::is_function<_T>::value, "not a function pointer");
    return __ptr;
}

template <typename _Ptr>
constexpr typename std::pointer_traits<_Ptr>::element_type *
__to_address(const _Ptr &__ptr)
{
    return std::__to_address(__ptr.operator->());
}

}  // namespace OS6STD

OS6STD_END_SYSTEM_HEADER

#endif

//=============================================================================
// End of file
//=============================================================================
