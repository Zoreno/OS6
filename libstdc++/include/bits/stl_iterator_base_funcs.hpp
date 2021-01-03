/**
 * @file stl_iterator_base_funcs.hpp
 * @author Joakim Bertils
 * @version 0.1
 * @date 2020-08-12
 *
 * @brief
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

#ifndef __STL_ITERATOR_BASE_FUNCS_HPP
#define __STL_ITERATOR_BASE_FUNCS_HPP

#pragma GCC system_header

#include <bits/c++config.hpp>

OS6STD_BEGIN_SYSTEM_HEADER

namespace OS6STD
{
template <typename>
struct _List_iterator;

template <typename>
struct _List_const_iterator;

//=============================================================================
// std::distance
//=============================================================================

template <typename _InputIterator>
inline constexpr typename iterator_traits<_InputIterator>::difference_type
__distance(_InputIterator __first, _InputIterator __last, input_iterator_tag)
{
    typename iterator_traits<_InputIterator>::difference_type __n = 0;

    while (__first != __last)
    {
        ++__first;
        ++__n;
    }

    return __n;
}

template <typename _RandomAccessIterator>
inline constexpr
    typename iterator_traits<_RandomAccessIterator>::difference_type
    __distance(_RandomAccessIterator __first,
               _RandomAccessIterator __last,
               random_access_iterator_tag)
{
    typename iterator_traits<_RandomAccessIterator>::difference_type __n = 0;

    __n = __last - __first;

    return __n;
}

template <typename _InputIterator>
inline typename iterator_traits<_InputIterator>::difference_type distance(
    _InputIterator __first, _InputIterator __last)
{
    return std::__distance(__first, __last, std::__iterator_category(__first));
}

//=============================================================================
// std::advance
//=============================================================================

template <typename _InputIterator, typename _Distance>
inline constexpr void _advance(_InputIterator &__i,
                               _Distance __n,
                               input_iterator_tag)
{
    while (__n--)
    {
        ++__i;
    }
}

template <typename _BidirectionalIterator, typename _Distance>
inline constexpr void __advance(_BidirectionalIterator &__i,
                                _Distance __n,
                                bidirectional_iterator_tag)
{
    if (__n > 0)
    {
        while (__n--)
        {
            ++__i;
        }
    }
    else
    {
        while (__n++)
        {
            --__i;
        }
    }
}

template <typename _RandomAccessIterator, typename _Distance>
inline constexpr void __advance(_RandomAccessIterator &__i,
                                _Distance __n,
                                random_access_iterator_tag)
{
    if (__builtin_constant_p(__n) && __n == 1)
    {
        ++__i;
    }
    else if (__builtin_constant_p(__n) && __n == -1)
    {
        --__i;
    }
    else
    {
        __i += __n;
    }
}

template <typename _InputIterator, typename _Distance>
inline void advance(_InputIterator &__i, _Distance __n)
{
    typename iterator_traits<_InputIterator>::difference_type __d = __n;

    std::__advance(__i, __d, std::__iterator_category(__i));
}

//=============================================================================
// std::next
//=============================================================================

template <typename _InputIterator>
inline _InputIterator next(
    _InputIterator __x,
    typename iterator_traits<_InputIterator>::difference_types __n = 1)
{
    std::advance(__x, __n);
    return __x;
}

//=============================================================================
// std::prev
//=============================================================================

template <typename _BidirectionalIterator>
inline _BidirectionalIterator prev(
    _BidirectionalIterator __x,
    typename iterator_traits<_BidirectionalIterator>::difference_types __n = 1)
{
    std::advance(__x, -__n);
    return __x;
}

}  // namespace OS6STD

OS6STD_END_SYSTEM_HEADER

#endif

//=============================================================================
// End of file
//=============================================================================
