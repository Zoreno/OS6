/**
 * @file stl_construct.hpp
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

#include <new>
#include <bits/move.hpp>
#include <bits/stl_iterator_base_types.hpp>
#include <bits/stl_iterator_base_funcs.hpp>

OS6STD_BEGIN_SYSTEM_HEADER

namespace OS6STD
{
template <typename _T, typename... _Args>
inline void _construct(_T *__p, _Args &&... __args)
{
    ::new (static_cast<void *>(__p)) _T(std::forward<_Args>(__args)...);
}

template <typename _T>
inline void _construct_novalue(_T *__p)
{
    ::new (static_cast<void *>(__p)) _T;
}

template <typename _ForwardIterator>
void _destroy(_ForwardIterator __first, _ForwardIterator __last);

template <typename _T>
inline void _destroy(_T *__p)
{
    __p->~_T();
}

template <bool>
struct _destroy_aux
{
    template <typename _ForwardIterator>
    static void __destroy(_ForwardIterator __first, _ForwardIterator __last)
    {
        for (; __first != __last; ++__first)
        {
            std::_destroy(std::__addressof(*__first));
        }
    }
};

template <>
struct _destroy_aux<true>
{
    template <typename _ForwardIterator>
    static void __destroy(_ForwardIterator __first, _ForwardIterator __last)
    {
    }
};

template <typename _ForwardIterator>
void _destroy(_ForwardIterator __first, _ForwardIterator __last)
{
    using _Value_type = typename iterator_traits<_ForwardIterator>::value_type;

    static_assert(is_destructible<_Value_type>::value, "Value is destructable");

    std::_destroy_aux<__has_trivial_destructor(_Value_type)>::__destroy(__first, __last);
}

}  // namespace OS6STD

OS6STD_END_SYSTEM_HEADER

#endif

//=============================================================================
// End of file
//=============================================================================
