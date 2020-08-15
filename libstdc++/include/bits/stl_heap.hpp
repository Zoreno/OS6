/**
 * @file stl_heap.hpp
 * @author Joakim Bertils
 * @version 0.1
 * @date 2020-08-15
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

#ifndef __STL_HEAP_HPP
#define __STL_HEAP_HPP

#pragma GCC system_header

#include <bits/c++config.hpp>
#include <bits/move.hpp>
#include <bits/predefined_ops.hpp>
#include <bits/stl_iterator_base_types.hpp>
#include <bits/stl_iterator_base_funcs.hpp>

OS6STD_BEGIN_SYSTEM_HEADER

namespace OS6STD
{
//=============================================================================
// std::is_heap, std::is_heap_until
//=============================================================================

/**
 * @brief Returns the distance to the last element of a valid heap.
 * 
 * @tparam _RandomAccessIterator Type of the iterator.
 * @tparam _Distance Type used to measure distance.
 * @tparam _Compare Type for the functor doing the comparison.
 * @param __first Iterator to first element in the range.
 * @param __n Maximum extent of the range.
 * @param __comp Comparison functor to use
 * 
 * @return Distance to last heap element of the range.
 */
template <typename _RandomAccessIterator,
          typename _Distance,
          typename _Compare>
_Distance __is_heap_until(_RandomAccessIterator __first,
                          _Distance __n,
                          _Compare& __comp)
{
    _Distance __parent = 0;

    for (_Distance __child = 1; __child < __n; ++__child)
    {
        if (__comp(__first + __parent, __first + __child))
        {
            return __child;
        }

        if ((__child & 1) == 0)
        {
            ++__parent;
        }
    }

    return __n;
}

/**
 * @brief Checks if range is a valid heap with default compare functor.
 * 
 * @tparam _RandomAccessIterator Type of the iterator.
 * @tparam _Distance Type used to measure distance.
 * @param __first Iterator to the first element in the range.
 * @param __n Maximum extent of the range.
 * 
 * @return true if the whole range is a heap
 */
template <typename _RandomAccessIterator,
          typename _Distance>
inline bool __is_heap(_RandomAccessIterator __first,
                      _Distance __n)
{
    OS6STD::__ops::_Iter_less_iter __comp;
    return std::__is_heap_until(__first, __n, __comp) == __n;
}

/**
 * @brief Checks if range is a valid heap with default compare.
 * 
 * @tparam _RandomAccessIterator Type of the iterator.
 * @tparam _Compare Type of the compare function
 * @tparam _Distance Type used to measure distance.
 * @param __first Iterator to the first element in the range. 
 * @param __comp Comparison function pointer.
 * @param __n Maximum extent of the range.
 * 
 * @return true if the whole range is a heap
 */
template <typename _RandomAccessIterator,
          typename _Compare,
          typename _Distance>
inline bool __is_heap(_RandomAccessIterator __first,
                      _Compare __comp,
                      _Distance __n)
{
    typedef __decltype(__comp) _Cmp;
    OS6STD::__ops::_Iter_comp_iter<_Cmp> __cmp(std::move(__comp));
    return std::__is_heap_until(__first, __n, __cmp) == __n;
}

/**
 * @brief Checks if a given range is a valid heap.
 * 
 * @tparam _RandomAccessIterator Type of the iterators
 * @param __first Iterator to start of range.
 * @param __last Iterator to end of range.
 * 
 * @return true if heap.
 */
template <typename _RandomAccessIterator>
inline bool __is_heap(_RandomAccessIterator __first,
                      _RandomAccessIterator __last)
{
    return std::__is_heap(__first, std::distance(__first, __last));
}

/**
 * @brief Checks if a given range is a valid heap with custom compare.
 * 
 * @tparam _RandomAccessIterator Type of the iterators.
 * @tparam _Compare Type of the compare functor.
 * @param __first Iterator to start of range.
 * @param __last Iterator to end of range.
 * @param __comp Compare functor.
 * 
 * @return true if heap given custom compare.
 */
template <typename _RandomAccessIterator,
          typename _Compare>
inline bool __is_heap(_RandomAccessIterator __first,
                      _RandomAccessIterator __last,
                      _Compare __comp)
{
    return std::__is_heap(__first, std::move(__comp,
                                             std::distance(__first, __last)));
}

/**
 * @brief Checks if the given range is a valid heap
 * 
 * @tparam _RandomAccessIterator Type of the iterators.
 * @param __first Iterator to the beginning of the range.
 * @param __last Iterator to the end of the range.
 * 
 * @return Iterator to one past the last element in the valid heap.
 */
template <typename _RandomAccessIterator>
inline _RandomAccessIterator is_heap_until(_RandomAccessIterator __first,
                                           _RandomAccessIterator __last)
{
    OS6STD::__ops::_Iter_less_iter __comp;
    return __first + std::__is_heap_until(__first,
                                          std::distance(__first, __last),
                                          __comp);
}

/**
 * @brief Checks if the given range is a valid heap
 * 
 * @tparam _RandomAccessIterator Type of the iterators.
 * @tparam _Compare Type of the compare functor.
 * @param __first Iterator to the beginning of the range.
 * @param __last Iterator to the end of the range.
 * @param __comp Compare functor.
 * 
 * @return Iterator to one past the last element in the valid heap.
 */
template <typename _RandomAccessIterator,
          typename _Compare>
inline _RandomAccessIterator is_heap_until(_RandomAccessIterator __first,
                                           _RandomAccessIterator __last,
                                           _Compare __comp)
{
    typedef __decltype(__comp) _Cmp;
    OS6STD::__ops::_Iter_comp_iter<_Cmp> __cmp(std::move(__comp));
    return __first + std::__is_heap_until(__first,
                                          std::distance(__first, __last),
                                          __cmp);
}

/**
 * @brief Check if the full range is a valid heap
 * 
 * @tparam _RandomAccessIterator Type of the iterators.
 * @param __first Iterator to the beginning of the range.
 * @param __last Iterator to the end of the range.
 * 
 * @return true if full range is a valid heap.
 */
template <typename _RandomAccessIterator>
inline bool is_heap(_RandomAccessIterator __first,
                    _RandomAccessIterator __last)
{
    return std::is_heap_until(__first, __last) == __last;
}

/**
 * @brief Check if the full range is a valid heap given custom compare.
 * 
 * @tparam _RandomAccessIterator Type of the iterators.
 * @tparam _Compare Type of the compare functor.
 * @param __first Iterator to beginning of heap.
 * @param __last Iterator to bed of heap.
 * @param __comp Compare functor.
 * 
 * @return true if full range is a valid heap.
 */
template <typename _RandomAccessIterator,
          typename _Compare>
inline bool is_heap(_RandomAccessIterator __first,
                    _RandomAccessIterator __last,
                    _Compare __comp)
{
    const auto __dist = std::distance(__first, __last);
    typedef __decltype(__comp) _Cmp;
    OS6STD::__ops::_Iter_comp_iter<_Cmp> __cmp(std::move(__comp));
    return std::is_heap_until(__first, __last, __cmp) == __dist;
}

//=============================================================================
// std::push_heap
//=============================================================================

/**
 * @brief Helper functionm to push a value to an existing heap.
 * 
 * @tparam _RandomAccessIterator Type of the iterator.
 * @tparam _Distance Type used to measure distance.
 * @tparam _T Type of the value to push
 * @tparam _Compare Type of the compare functor.
 * @param __first Iterator to first element in range.
 * @param __holeIndex Hole to place the value into.
 * @param __topIndex Index of the top value of the heap.
 * @param __value Value to add to the heap.
 * @param __comp Compare functor.
 * 
 */
template <typename _RandomAccessIterator,
          typename _Distance,
          typename _T,
          typename _Compare>
void __push_heap(_RandomAccessIterator __first,
                 _Distance __holeIndex,
                 _Distance __topIndex,
                 _T __value,
                 _Compare& __comp)
{
    _Distance __parent = (__holeIndex - 1) / 2;

    while (__holeIndex > __topIndex && __comp(__first + __parent, __value))
    {
        *(__first + __holeIndex) = std::move(*(__first + __parent));
        __holeIndex = __parent;
        __parent = (__holeIndex - 1) / 2;
    }

    *(__first + __holeIndex) = std::move(__value);
}

/**
 * @brief Pushes a value to the heap.
 * 
 * @tparam _RandomAccessIterator Type of the iterators.
 * @param __first Iterator to the beginning of the range.
 * @param __last Iterator to the end of the range.
 * 
 */
template <typename _RandomAccessIterator>
inline void push_heap(_RandomAccessIterator __first,
                      _RandomAccessIterator __last)
{
    using _Value = typename iterator_traits<_RandomAccessIterator>::value_type;
    using _Distance = typename iterator_traits<_RandomAccessIterator>::difference_type;

    OS6STD::__ops::_Iter_less_val __comp;
    _Value __value = std::move(*(__last - 1));
    std::__push_heap(__first,
                     _Distance((__last - __first) - 1),
                     _Distance(0),
                     std::move(__value),
                     __comp);
}

/**
 * @brief Pushes a value to the heap with custom compare.
 * 
 * @tparam _RandomAccessIterator Type of the iterators.
 * @tparam _Compare Type of the compare functor.
 * @param __first Iterator to the beginning of the range.
 * @param __last Iterator to the end of the range.
 * @param __comp Compare functor.
 * 
 */
template <typename _RandomAccessIterator,
          typename _Compare>
inline void push_heap(_RandomAccessIterator __first,
                      _RandomAccessIterator __last,
                      _Compare __comp)
{
    using _Value = typename iterator_traits<_RandomAccessIterator>::value_type;
    using _Distance = typename iterator_traits<_RandomAccessIterator>::difference_type;

    __decltype(OS6STD::__ops::__iter_comp_val(std::move(__comp)))
        __cmp(std::move(__comp));
    _Value __value = std::move(*(__last - 1));
    std::__push_heap(__first,
                     _Distance((__last - __first) - 1),
                     _Distance(0),
                     std::move(__value),
                     __cmp);
}

//=============================================================================
// std::__adjust_heap
//=============================================================================

/**
 * @brief Adjusts the heap.
 * 
 * @tparam _RandomAccessIterator Type of the iterators.
 * @tparam _Distance Type used to measure distance.
 * @tparam _T Type of the value.
 * @tparam _Compare Type of the comparison functor.
 * @param __first Iterator to beginning of the range.
 * @param __holeIndex Index of the hole.
 * @param __len Length of the range.
 * @param __value Value to adjust for.
 * @param __comp Compare functor.
 * 
 */
template <typename _RandomAccessIterator,
          typename _Distance,
          typename _T,
          typename _Compare>
void __adjust_heap(_RandomAccessIterator __first,
                   _Distance __holeIndex,
                   _Distance __len,
                   _T __value,
                   _Compare __comp)
{
    const _Distance __topIndex = __holeIndex;
    _Distance __secondChild = __holeIndex;

    while (__secondChild < (__len - 1) / 2)
    {
        __secondChild = 2 * (__secondChild + 1);

        if (__comp(__first + __secondChild, __first + (__secondChild - 1)))
        {
            __secondChild--;
        }

        *(__first + __holeIndex) = std::move(*(__first + __secondChild));
        __holeIndex = __secondChild;
    }

    if ((__len & 1) == 0 && __secondChild == (__len - 2) / 2)
    {
        __secondChild = 2 * (__secondChild + 1);
        *(__first + __holeIndex) = std::move(*(__first + (__secondChild - 1)));
        __holeIndex = __secondChild - 1;
    }

    __decltype(OS6STD::__ops::__iter_comp_val(std::move(__comp))) __cmp(std::move(__comp));
    std::__push_heap(__first,
                     __holeIndex,
                     __topIndex,
                     std::move(__value),
                     __cmp);
}

//=============================================================================
// std::pop_heap
//=============================================================================

/**
 * @brief Removes an element from the heap
 * 
 * @tparam _RandomAccessIterator Type of the iterators.
 * @tparam _Compare Compare functor type.
 * @param __first Iterator to the beginning of the range.
 * @param __last Iterator to the end of the heap.
 * @param __result Iterator to place to store the popped element.
 * @param __comp Compare functor.
 * 
 */
template <typename _RandomAccessIterator,
          typename _Compare>
inline void __pop_heap(_RandomAccessIterator __first,
                       _RandomAccessIterator __last,
                       _RandomAccessIterator __result,
                       _Compare& __comp)
{
    using _Value = typename iterator_traits<_RandomAccessIterator>::value_type;
    using _Distance = typename iterator_traits<_RandomAccessIterator>::difference_type;

    _Value __value = std::move(*__result);
    *__result = std::move(*__first);

    std::__adjust_heap(__first,
                       _Distance(0),
                       _Distance(__last - __first),
                       std::move(__value),
                       __comp);
}

/**
 * @brief Removes an element from the heap.
 * 
 * @tparam _RandomAccessIterator Type of the iterators
 * @param __first Iterator to the beginning of the range.
 * @param __last Iterator to the end of the range.
 * 
 */
template <typename _RandomAccessIterator>
inline void pop_heap(_RandomAccessIterator __first,
                     _RandomAccessIterator __last)
{
    if (__last - __first > 1)
    {
        --__last;
        OS6STD::__ops::_Iter_less_iter __comp;
        std::__pop_heap(__first,
                        __last,
                        __last,
                        __comp);
    }
}

/**
 * @brief Removes an element from the heap with custom compare.
 * 
 * @tparam _RandomAccessIterator Type of the iterators.
 * @tparam _Compare Compare functor type.
 * @param __first Iterator to the beginning of the range.
 * @param __last Iterator to the end of the range.
 * @param __comp Compare functor.
 * 
 */
template <typename _RandomAccessIterator,
          typename _Compare>
inline void pop_heap(_RandomAccessIterator __first,
                     _RandomAccessIterator __last,
                     _Compare __comp)
{
    if (__last - __first > 1)
    {
        typedef __decltype(__comp) _Cmp;
        OS6STD::__ops::_Iter_comp_iter<_Cmp> __cmp(std::move(__comp));
        --__last;
        std::__pop_heap(__first,
                        __last,
                        __last,
                        __cmp);
    }
}

//=============================================================================
// std::make_heap
//=============================================================================

/**
 * @brief Turns a range to a heap
 * 
 * @tparam _RandomAccessIterator Type of the iterators.
 * @tparam _Compare Compare functor type.
 * @param __first Iterator to the beginning of the range.
 * @param __last Iterator to the end of the range.
 * @param __comp Compare functor.
 * 
 */
template <typename _RandomAccessIterator,
          typename _Compare>
void __make_heap(_RandomAccessIterator __first,
                 _RandomAccessIterator __last,
                 _Compare& __comp)
{
    using _Value = typename iterator_traits<_RandomAccessIterator>::value_type;
    using _Distance = typename iterator_traits<_RandomAccessIterator>::difference_type;

    if (__last - __first < 2)
    {
        return;
    }

    const _Distance __len = __last - __first;
    _Distance __parent = (__len - 2) / 2;

    while (true)
    {
        _Value __value = std::move(*(__first + __parent));

        std::__adjust_heap(__first,
                           __parent,
                           __len,
                           std::move(__value),
                           __comp);

        if (__parent == 0)
        {
            return;
        }

        __parent--;
    }
}

/**
 * @brief Turns a range into a heap.
 * 
 * @tparam _RandomAccessIterator Type of the iterators.
 * @param __first Iterator to beginning of the range.
 * @param __last Iterator to end of the range.
 * 
 */
template <typename _RandomAccessIterator>
inline void make_heap(_RandomAccessIterator __first,
                      _RandomAccessIterator __last)
{
    OS6STD::__ops::_Iter_less_iter __comp;
    std::__make_heap(__first,
                     __last,
                     __comp);
}

/**
 * @brief Turns a range into a heap with custom compare.
 * 
 * @tparam _RandomAccessIterator Type of the iterators.
 * @tparam _Compare Compare functor type.
 * @param __first Iterator to beginning of the range.
 * @param __last Iterator to end of the range.
 * @param __comp Compare functor.
 * 
 */
template <typename _RandomAccessIterator,
          typename _Compare>
inline void make_heap(_RandomAccessIterator __first,
                      _RandomAccessIterator __last,
                      _Compare __comp)
{
    typedef __decltype(__comp) _Cmp;
    OS6STD::__ops::_Iter_comp_iter<_Cmp> __cmp(std::move(__comp));
    std::__make_heap(__first,
                     __last,
                     __cmp);
}

//=============================================================================
// std::sort_heap
//=============================================================================

/**
 * @brief Sorts a heap, turning a heap into a range of ascending values.
 * 
 * @tparam _RandomAccessIterator Type of the iterators.
 * @tparam _Compare Compare functor type.
 * @param __first Iterator to the beginning of the range.
 * @param __last Iterator to the end of the range.
 * @param __comp Compare functor.
 * 
 */
template <typename _RandomAccessIterator,
          typename _Compare>
void __sort_heap(_RandomAccessIterator __first,
                 _RandomAccessIterator __last,
                 _Compare& __comp)
{
    while (__last - __first > 1)
    {
        --__last;
        std::__pop_heap(__first,
                        __last,
                        __last,
                        __comp);
    }
}

/**
 * @brief Sorts a heap, turning a heap into a range of ascending values.
 * 
 * @tparam _RandomAccessIterator Type of the iterators.
 * @param __first Iterator to the beginning of the range.
 * @param __last Iterator to the end of the range.
 * 
 */
template <typename _RandomAccessIterator>
inline void sort_heap(_RandomAccessIterator __first,
                      _RandomAccessIterator __last)
{
    OS6STD::__ops::_Iter_less_iter __comp;
    std::__sort_heap(__first,
                     __last,
                     __comp);
}

/**
 * @brief Sorts a heap, turning a heap into a range of ascending values with custom compare.
 * 
 * @tparam _RandomAccessIterator Type of the iterators.
 * @tparam _Compare Compare functor type.
 * @param __first Iterator to the beginning of the range.
 * @param __last Iterator to the end of the range.
 * @param __comp Compare functor.
 * 
 */
template <typename _RandomAccessIterator,
          typename _Compare>
inline void sort_heap(_RandomAccessIterator __first,
                      _RandomAccessIterator __last,
                      _Compare __comp)
{
    typedef __decltype(__comp) _Cmp;
    OS6STD::__ops::_Iter_comp_iter<_Cmp> __cmp(std::move(__comp));
    std::__sort_heap(__first,
                     __last,
                     __cmp);
}

}  // namespace OS6STD

OS6STD_END_SYSTEM_HEADER

#endif

//=============================================================================
// End of file
//=============================================================================
