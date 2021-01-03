/**
 * @file stl_iterator_base_types.hpp
 * @author Joakim Bertils
 * @version 0.1
 * @date 2020-07-28
 *
 * @brief General iterator-related utility types.
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

#ifndef __STL_ITERATOR_BASE_TYPES_HPP
#define __STL_ITERATOR_BASE_TYPES_HPP

#pragma GCC system_header

#include <bits/c++config.hpp>

#include <type_traits>

OS6STD_BEGIN_SYSTEM_HEADER

namespace OS6STD
{
/**
 * @brief Marking for input iterators
 *
 *
 */
struct input_iterator_tag
{
};

/**
 * @brief Marking for output iterators
 *
 *
 */
struct output_iterator_tag
{
};

/**
 * @brief Marking for forward iterators
 *
 *
 */
struct forward_iterator_tag : public input_iterator_tag
{
};

/**
 * @brief Marking for bidirectional iterators
 *
 *
 */
struct bidirectional_iterator_tag : public forward_iterator_tag
{
};

/**
 * @brief Marking for random access iterators
 *
 *
 */
struct random_access_iterator_tag : public bidirectional_iterator_tag
{
};

/**
 * @brief Common iterator class
 *
 * @tparam _Category Iterator category
 * @tparam _T The type that the iterator points to.
 * @tparam _Distance Distance between iterators.
 * @tparam _Pointer Pointer type.
 * @tparam _Reference Reference type.
 *
 */
template <typename _Category,
          typename _T,
          typename _Distance = ptrdiff_t,
          typename _Pointer = _T *,
          typename _Reference = _T &>
struct iterator
{
    using iterator_category = _Category;
    using value_type = _T;
    using difference_type = _Distance;
    using pointer = _Pointer;
    using reference = _Reference;
};

template <typename _Iterator, typename = __void_t<>>
struct __iterator_traits
{
};

template <typename _Iterator>
struct __iterator_traits<_Iterator,
                         __void_t<typename _Iterator::iterator_category,
                                  typename _Iterator::value_type,
                                  typename _Iterator::difference_type,
                                  typename _Iterator::pointer,
                                  typename _Iterator::reference>>
{
    using iterator_category = typename _Iterator::iterator_category;
    using value_type = typename _Iterator::value_type;
    using difference_type = typename _Iterator::difference_type;
    using pointer = typename _Iterator::pointer;
    using reference = typename _Iterator::reference;
};

template <typename _Iterator>
struct iterator_traits : public __iterator_traits<_Iterator>
{
};

template <typename _T>
struct iterator_traits<_T *>
{
    using iterator_category = random_access_iterator_tag;
    using value_type = _T;
    using difference_type = ptrdiff_t;
    using pointer = _T *;
    using reference = _T &;
};

template <typename _T>
struct iterator_traits<const _T *>
{
    using iterator_category = random_access_iterator_tag;
    using value_type = _T;
    using difference_type = ptrdiff_t;
    using pointer = const _T *;
    using reference = const _T &;
};

template <typename _Iter>
inline constexpr typename iterator_traits<_Iter>::iterator_category
__iterator_category(const _Iter &)
{
    return typename iterator_traits<_Iter>::iterator_category();
}

template <typename _InIter>
using _RequireInputIter = typename enable_if<
    is_convertible<typename iterator_traits<_InIter>::iterator_category,
                   input_iterator_tag>::value>::type;

}  // namespace OS6STD

OS6STD_END_SYSTEM_HEADER

#endif

//=============================================================================
// End of file
//=============================================================================
