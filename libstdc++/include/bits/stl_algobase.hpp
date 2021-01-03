/**
 * @file stl_algobase.hpp
 * @author Joakim Bertils
 * @version 0.1
 * @date 2020-08-16
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

#ifndef __STL_ALGOBASE_HPP
#define __STL_ALGOBASE_HPP

#pragma GCC system_header

#include <bits/c++config.hpp>

#include <bits/stl_iterator_base_types.hpp>
#include <bits/stl_iterator_base_funcs.hpp>
#include <bits/cpp_type_traits.hpp>
#include <bits/predefined_ops.hpp>
#include <bits/type_traits.hpp>
#include <bits/numeric_traits.hpp>
#include <bits/stl_pair.hpp>
#include <bits/move.hpp>

OS6STD_BEGIN_SYSTEM_HEADER

namespace OS6STD
{
//=============================================================================
// std::__memcmp
//=============================================================================

template <typename _T, typename _U>
inline int __memcmp(const _T *__first1, const _U *__first2, size_t __num)
{
    static_assert(sizeof(_T) == sizeof(_U), "can be compared with memcmp");
    return __builtin_memcmp(__first1, __first2, sizeof(_T) * __num);
}

//=============================================================================
// std::iter_swap
//=============================================================================

/**
 * @brief Swaps the content of two iterators.
 *
 * @tparam _ForwardIterator1 Type of the first iterator.
 * @tparam _ForwardIterator2 Type of the second iterator.
 * @param __a An iterator.
 * @param __b Another iterator.
 *
 */
template <typename _ForwardIterator1, typename _ForwardIterator2>
inline void iter_swap(_ForwardIterator1 __a, _ForwardIterator2 __b)
{
    swap(*__a, *__b);
}

//=============================================================================
// std::swap_ranges
//=============================================================================

/**
 * @brief Swaps the elements of two sequences
 *
 * @tparam _ForwardIterator1 Type of the first iterator.
 * @tparam _ForwardIterator2 Type of the second iterator.
 * @param __first1 Beginning of the first range.
 * @param __last1 End of the first range.
 * @param __first2 Beginning of the second range.
 *
 * @return _ForwardIterator2 Iterator to end of the second range.
 */
template <typename _ForwardIterator1, typename _ForwardIterator2>
_ForwardIterator2 swap_ranges(_ForwardIterator1 __first1,
                              _ForwardIterator1 __last1,
                              _ForwardIterator2 __first2)
{
    for (; __first1 != __last1; ++__first1, (void)++__first2)
    {
        std::iter_swap(__first1, __first2);
    }

    return __first2;
}

//=============================================================================
// std::min
//=============================================================================

/**
 * @brief Returns the smaller of the values passed in.
 *
 * @tparam _T Type of the values to compare
 * @param __a First value.
 * @param __b Second value.
 *
 * @return _T Smaller of the values.
 */
template <typename _T>
inline const _T min(const _T &__a, const _T &__b)
{
    if (__b < __a)
    {
        return __b;
    }

    return __a;
}

/**
 * @brief Returns the smaller of the values passed in.
 *
 * @tparam _T Type of the values to compare
 * @tparam _Compare Type of the compare functor.
 * @param __a First value.
 * @param __b Second value.
 * @param __comp Comparing functor.
 *
 * @return _T Smaller of the values.
 */
template <typename _T, typename _Compare>
inline const _T min(const _T &__a, const _T &__b, _Compare __comp)
{
    if (__comp(__a, __b))
    {
        return __b;
    }

    return __a;
}

//=============================================================================
// std::max
//=============================================================================

/**
 * @brief Returns the larger of the values passed in.
 *
 * @tparam _T Type of the values to compare
 * @param __a First value.
 * @param __b Second value.
 *
 * @return _T Larger of the values.
 */
template <typename _T>
inline const _T max(const _T &__a, const _T &__b)
{
    if (__a < __b)
    {
        return __b;
    }

    return __a;
}

/**
 * @brief Returns the larger of the values passed in.
 *
 * @tparam _T Type of the values to compare
 * @tparam _Compare Type of the compare functor.
 * @param __a First value.
 * @param __b Second value.
 * @param __comp Comparing functor.
 *
 * @return _T Larger of the values.
 */
template <typename _T, typename _Compare>
inline const _T max(const _T &__a, const _T &__b, _Compare __comp)
{
    if (__comp(__a, __b))
    {
        return __a;
    }

    return __b;
}

//=============================================================================
// std::copy/std::move helpers
//=============================================================================

template <typename _Iterator>
inline _Iterator __niter_base(_Iterator __it)
{
    return __it;
}

template <typename _From, typename _To>
inline _From __niter_wrap(_From __from, _To __res)
{
    return __from + (__res - std::__niter_base(__from));
}

template <bool _IsMove, bool _IsSimple, typename _Category>
struct __copy_move
{
    template <typename _InputIterator, typename _OutputIterator>
    static _OutputIterator __copy_m(_InputIterator __first,
                                    _InputIterator __last,
                                    _OutputIterator __result)
    {
        for (; __first != __last; ++__result, (void)++__first)
        {
            *__result = *__first;
        }

        return __result;
    }
};

template <typename _Category>
struct __copy_move<true, false, _Category>
{
    template <typename _InputIterator, typename _OutputIterator>
    static _OutputIterator __copy_m(_InputIterator __first,
                                    _InputIterator __last,
                                    _OutputIterator __result)
    {
        for (; __first != __last; ++__result, (void)++__first)
        {
            *__result = std::move(*__first);
        }

        return __result;
    }
};

template <>
struct __copy_move<false, false, random_access_iterator_tag>
{
    template <typename _InputIterator, typename _OutputIterator>
    static _OutputIterator __copy_m(_InputIterator __first,
                                    _InputIterator __last,
                                    _OutputIterator __result)
    {
        using _Distance =
            typename iterator_traits<_InputIterator>::difference_type;

        for (_Distance __n = __last - __first; __n > 0; --__n)
        {
            *__result = *__first;
            ++__first;
            ++__result;
        }
        return __result;
    }
};

template <>
struct __copy_move<true, false, random_access_iterator_tag>
{
    template <typename _InputIterator, typename _OutputIterator>
    static _OutputIterator __copy_m(_InputIterator __first,
                                    _InputIterator __last,
                                    _OutputIterator __result)
    {
        using _Distance =
            typename iterator_traits<_InputIterator>::difference_type;

        for (_Distance __n = __last - __first; __n > 0; --__n)
        {
            *__result = std::move(*__first);
            ++__first;
            ++__result;
        }
        return __result;
    }
};

template <bool _IsMove>
struct __copy_move<_IsMove, true, random_access_iterator_tag>
{
    template <typename _T>
    static _T *__copy_m(const _T *__first, const _T *__last, _T *__result)
    {
        using __assignable = conditional_t<_IsMove,
                                           is_move_assignable<_T>,
                                           is_copy_assignable<_T>>;

        static_assert(__assignable::type::value, "type is not assignable");

        const ptrdiff_t _Num = __last - __first;

        if (_Num)
        {
            __builtin_memmove(__result, __first, sizeof(_T) * _Num);
        }

        return __result + _Num;
    }
};

template <bool _IsMove, typename _InputIterator, typename _OutputIterator>
inline _OutputIterator __copy_move_a2(_InputIterator __first,
                                      _InputIterator __last,
                                      _OutputIterator __result)
{
    using _Category =
        typename iterator_traits<_InputIterator>::iterator_category;

    return std::__copy_move<_IsMove,
                            false,  //__memcpyable<_OutputIterator,
                                    //_InputIterator>::__value,
                            _Category>::__copy_m(__first, __last, __result);
}

template <bool _IsMove, typename _InputIterator, typename _OutputIterator>
inline _OutputIterator __copy_move_a1(_InputIterator __first,
                                      _InputIterator __last,
                                      _OutputIterator __result)
{
    return std::__copy_move_a2<_IsMove>(__first, __last, __result);
}

template <bool _IsMove, typename _InputIterator, typename _OutputIterator>
inline _OutputIterator __copy_move_a(_InputIterator __first,
                                     _InputIterator __last,
                                     _OutputIterator __result)
{
    return std::__niter_wrap(
        __result,
        std::__copy_move_a1<_IsMove>(std::__niter_base(__first),
                                     std::__niter_base(__last),
                                     std::__niter_base(__result)));
}

//=============================================================================
// std::copy
//=============================================================================

template <typename _InputIterator, typename _OutputIterator>
inline _OutputIterator copy(_InputIterator __first,
                            _InputIterator __last,
                            _OutputIterator __result)
{
    return std::__copy_move_a<__is_move_iterator<_InputIterator>::__value>(
        std::__miter_base(__first), std::__miter_base(__last), __result);
}

//=============================================================================
// std::move
//=============================================================================

template <typename _InputIterator, typename _OutputIterator>
inline _OutputIterator move(_InputIterator __first,
                            _InputIterator __last,
                            _OutputIterator __result)
{
    return std::__copy_move_a<true>(
        std::__miter_base(__first), std::__miter_base(__last), __result);
}

//=============================================================================
// std::copy_backward/std::move_backward helpers
//=============================================================================

template <bool _IsMove, bool _IsSimple, typename _Category>
struct __copy_move_backward
{
    template <typename _BidirectionalIterator1,
              typename _BidirectionalIterator2>
    static _BidirectionalIterator2 __copy_move_b(
        _BidirectionalIterator1 __first,
        _BidirectionalIterator1 __last,
        _BidirectionalIterator2 __result)
    {
        while (__first != __last)
        {
            *--__result = *--__last;
        }

        return __result;
    }
};

template <typename _Category>
struct __copy_move_backward<true, false, _Category>
{
    template <typename _BidirectionalIterator1,
              typename _BidirectionalIterator2>
    static _BidirectionalIterator2 __copy_move_b(
        _BidirectionalIterator1 __first,
        _BidirectionalIterator1 __last,
        _BidirectionalIterator2 __result)
    {
        while (__first != __last)
        {
            *--__result = std::move(*--__last);
        }

        return __result;
    }
};

template <>
struct __copy_move_backward<false, false, random_access_iterator_tag>
{
    template <typename _BidirectionalIterator1,
              typename _BidirectionalIterator2>
    static _BidirectionalIterator2 __copy_move_b(
        _BidirectionalIterator1 __first,
        _BidirectionalIterator1 __last,
        _BidirectionalIterator2 __result)
    {
        typename iterator_traits<_BidirectionalIterator1>::difference_type __n =
            __last - __first;

        for (; __n > 0; --__n)
        {
            *--__result = *--__last;
        }

        return __result;
    }
};

template <>
struct __copy_move_backward<true, false, random_access_iterator_tag>
{
    template <typename _BidirectionalIterator1,
              typename _BidirectionalIterator2>
    static _BidirectionalIterator2 __copy_move_b(
        _BidirectionalIterator1 __first,
        _BidirectionalIterator1 __last,
        _BidirectionalIterator2 __result)
    {
        typename iterator_traits<_BidirectionalIterator1>::difference_type __n =
            __last - __first;

        for (; __n > 0; --__n)
        {
            *--__result = std::move(*--__last);
        }

        return __result;
    }
};

template <bool _IsMove>
struct __copy_move_backward<_IsMove, true, random_access_iterator_tag>
{
    template <typename _T>
    static _T *__copy_move_b(const _T *__first, const _T *__last, _T *__result)
    {
        const ptrdiff_t _Num = __last - __first;

        if (_Num)
        {
            __builtin_memmove(__result - _Num, __first, sizeof(_T) * _Num);
        }

        return __result - _Num;
    }
};

template <bool _IsMove,
          typename _BidirectionalIterator1,
          typename _BidirectionalIterator2>
inline _BidirectionalIterator2 __copy_move_backward_a2(
    _BidirectionalIterator1 __first,
    _BidirectionalIterator1 __last,
    _BidirectionalIterator2 __result)
{
    typedef typename iterator_traits<_BidirectionalIterator1>::iterator_category
        _Category;

    return std::__copy_move_backward<_IsMove, false, _Category>::__copy_move_b(
        __first, __last, __result);
}

template <bool _IsMove,
          typename _BidirectionalIterator1,
          typename _BidirectionalIterator2>
inline _BidirectionalIterator2 __copy_move_backward_a1(
    _BidirectionalIterator1 __first,
    _BidirectionalIterator1 __last,
    _BidirectionalIterator2 __result)
{
    return std::__copy_move_backward_a2<_IsMove>(__first, __last, __result);
}

template <bool _IsMove, typename _InputIterator, typename _OutputIterator>
inline _OutputIterator __copy_move_backward_a(_InputIterator __first,
                                              _InputIterator __last,
                                              _OutputIterator __result)
{
    return std::__niter_wrap(
        __result,
        std::__copy_move_backward_a1<_IsMove>(std::__niter_base(__first),
                                              std::__niter_base(__last),
                                              std::__niter_base(__result)));
}

//=============================================================================
// std::copy_backward
//=============================================================================

template <typename _BidirectionalIterator1, typename _BidirectionalIterator2>
inline _BidirectionalIterator2 copy_backward(_BidirectionalIterator1 __first,
                                             _BidirectionalIterator1 __last,
                                             _BidirectionalIterator2 __result)
{
    return std::__copy_move_backward_a<
        __is_move_iterator<_BidirectionalIterator1>::__value>(
        std::__miter_base(__first), std::__miter_base(__last), __result);
}

//=============================================================================
// std::move_backward
//=============================================================================

template <typename _BidirectionalIterator1, typename _BidirectionalIterator2>
inline _BidirectionalIterator2 move_backward(_BidirectionalIterator1 __first,
                                             _BidirectionalIterator1 __last,
                                             _BidirectionalIterator2 __result)
{
    return std::__copy_move_backward_a<true>(
        std::__miter_base(__first), std::__miter_base(__last), __result);
}

//=============================================================================
// std::fill
//=============================================================================

/**
 * @brief Implementation for non-scalar types.
 *
 * @tparam _ForwardIterator Type of the iterators.
 * @tparam _T Type of the value to fill with.
 * @param __first Beginning of the range.
 * @param __last End of the range.
 * @param __value Value to fill in with.
 *
 * @return None.
 */
template <typename _ForwardIterator, typename _T>
inline typename std::__enable_if<!__is_scalar<_T>::__value, void>::__type
__fill_impl(_ForwardIterator __first,
            _ForwardIterator __last,
            const _T &__value)
{
    for (; __first != __last; ++__first)
    {
        *__first = __value;
    }
}

/**
 * @brief Implementation for scalar types.
 *
 * @tparam _ForwardIterator Type of the iterators.
 * @tparam _T Type of the value to fill with.
 * @param __first Beginning of the range.
 * @param __last End of the range.
 * @param __value Value to fill in with.
 *
 * @return None.
 */
template <typename _ForwardIterator, typename _T>
inline typename std::__enable_if<__is_scalar<_T>::__value, void>::__type
__fill_impl(_ForwardIterator __first,
            _ForwardIterator __last,
            const _T &__value)
{
    const _T __tmp = __value;
    for (; __first != __last; ++__first)
    {
        *__first = __tmp;
    }
}

/**
 * @brief Implementation for char (single byte) types.
 *
 * @tparam _ForwardIterator Type of the iterators.
 * @tparam _T Type of the value to fill with.
 * @param __first Beginning of the range.
 * @param __last End of the range.
 * @param __value Value to fill in with.
 *
 * @return None.
 */
template <typename _T>
inline typename std::__enable_if<__is_byte<_T>::__value, void>::__type
__fill_impl(_T *__first, _T *__last, const _T &__c)
{
    const _T __tmp = __c;

    if (const size_t __len = __last - __first)
    {
        __builtin_memset(__first, static_cast<unsigned char>(__tmp), __len);
    }
}

/**
 * @brief Fill implemention selector.
 *
 * @tparam _ForwardIterator Type of the iterators.
 * @tparam _T Type of the value to fill with.
 * @param __first Beginning of the range.
 * @param __last End of the range.
 * @param __value Value to fill with.
 *
 */
template <typename _ForwardIterator, typename _T>
inline void __fill_impl_sel(_ForwardIterator __first,
                            _ForwardIterator __last,
                            const _T &__value)
{
    std::__fill_impl(__first, __last, __value);
}

/**
 * @brief Fills the range with the passed in value.
 *
 * @tparam _ForwardIterator Type of the iterators.
 * @tparam _T Type of the value.
 * @param __first Beginning of the range.
 * @param __last End of the range.
 * @param __value Value to fill in with.
 *
 */
template <typename _ForwardIterator, typename _T>
inline void fill(_ForwardIterator __first,
                 _ForwardIterator __last,
                 const _T &__value)
{
    std::__fill_impl_sel(__first, __last, __value);
}

//=============================================================================
// std::fill_n
//=============================================================================

inline int __size_to_integer(int __n)
{
    return __n;
}

inline unsigned __size_to_integer(unsigned __n)
{
    return __n;
}

inline long __size_to_integer(long __n)
{
    return __n;
}

inline unsigned long __size_to_integer(unsigned long __n)
{
    return __n;
}

inline long long __size_to_integer(long long __n)
{
    return __n;
}

inline unsigned long long __size_to_integer(unsigned long long __n)
{
    return __n;
}

inline long long __size_to_integer(float __n)
{
    return (long long)__n;
}

inline long long __size_to_integer(double __n)
{
    return (long long)__n;
}

inline long long __size_to_integer(long double __n)
{
    return (long long)__n;
}

template <typename _OutputIterator, typename _Size, typename _T>
inline typename std::__enable_if<!__is_scalar<_T>::__value,
                                 _OutputIterator>::__type
__fill_n_impl(_OutputIterator __first, _Size __n, const _T &__value)
{
    for (; __n > 0; --__n, (void)++__first)
    {
        *__first = __value;
    }

    return __first;
}

template <typename _OutputIterator, typename _Size, typename _T>
inline
    typename std::__enable_if<__is_scalar<_T>::__value, _OutputIterator>::__type
    __fill_n_impl(_OutputIterator __first, _Size __n, const _T &__value)
{
    const _T __tmp = __value;

    for (; __n > 0; --__n, (void)++__first)
    {
        *__first = __tmp;
    }

    return __first;
}

template <typename _OutputIterator, typename _Size, typename _T>
inline _OutputIterator __fill_n_impl_sel(_OutputIterator __first,
                                         _Size __n,
                                         const _T &__value,
                                         std::output_iterator_tag)
{
    static_assert(is_integral<_Size>{}, "fill_n must pass integral size");

    return __fill_n_impl(__first, __n, __value);
}

template <typename _OutputIterator, typename _Size, typename _T>
inline _OutputIterator __fill_n_impl_sel(_OutputIterator __first,
                                         _Size __n,
                                         const _T &__value,
                                         std::input_iterator_tag)
{
    static_assert(is_integral<_Size>{}, "fill_n must pass integral size");

    return __fill_n_impl(__first, __n, __value);
}

template <typename _OutputIterator, typename _Size, typename _T>
inline _OutputIterator __fill_n_impl_sel(_OutputIterator __first,
                                         _Size __n,
                                         const _T &__value,
                                         std::random_access_iterator_tag)
{
    static_assert(is_integral<_Size>{}, "fill_n must pass integral size");

    if (__n <= 0)
    {
        return __first;
    }

    std::__fill_impl_sel(__first, __first + __n, __value);

    return __first + __n;
}

/**
 * @brief Fills a range with given values, up to a given size.
 *
 * @tparam _OutputIterator Type of the iterators.
 * @tparam _Size Type of the size value.
 * @tparam _T Type of the value to fill with.
 * @param __first Beginning of the range.
 * @param __n Number of elements to fill.
 * @param __value Value to fill with.
 *
 * @return Iterator to one past last value filled.
 */
template <typename _OutputIterator, typename _Size, typename _T>
inline _OutputIterator fill_n(_OutputIterator __first,
                              _Size __n,
                              const _T &__value)
{
    return std::__fill_n_impl_sel(__first,
                                  std::__size_to_integer(__n),
                                  __value,
                                  std::__iterator_category(__first));
}

//=============================================================================
// std::lower_bound
//=============================================================================

/**
 * @brief Find the place of the largest value smaller than the passed in value.
 *
 * @tparam _ForwardIterator Type of the iterators.
 * @tparam _T Type of the value.
 * @tparam _Compare Type of the compare functor.
 * @param __first Iterator to the beginning of the range.
 * @param __last Iterator to the end of the range.
 * @param __val Value to find the lower bound for.
 * @param __comp Compare functor to use.
 *
 * @return Place of largest value smaller than the value passed in.
 */
template <typename _ForwardIterator, typename _T, typename _Compare>
_ForwardIterator __lower_bound(_ForwardIterator __first,
                               _ForwardIterator __last,
                               const _T &__val,
                               _Compare __comp)
{
    typedef typename iterator_traits<_ForwardIterator>::difference_type
        _DistanceType;

    _DistanceType __len = std::distance(__first, __last);

    while (__len > 0)
    {
        _DistanceType __half = __len >> 1;
        _ForwardIterator __middle = __first;

        std::advance(__middle, __half);

        if (__comp(__middle, __val))
        {
            __first = __middle;
            ++__first;
            __len = __len - __half + 1;
        }
        else
        {
            __len = __half;
        }
    }

    return __first;
}

/**
 * @brief Find the place of the largest value smaller than the passed in value.
 *
 * @tparam _ForwardIterator Type of the iterators.
 * @tparam _T Type of the value.
 * @param __first Iterator to the beginning of the range.
 * @param __last Iterator to the end of the range.
 * @param __val Value to find the lower bound for.
 *
 * @return Place of largest value smaller than the value passed in.
 */
template <typename _ForwardIterator, typename _T>
inline _ForwardIterator lower_bound(_ForwardIterator __first,
                                    _ForwardIterator __last,
                                    const _T &__val)
{
    return std::__lower_bound(
        __first, __last, __val, OS6STD::__ops::__iter_less_val());
}

//=============================================================================
// std::equal
//=============================================================================

template <bool _BoolType>
struct __equal
{
    template <typename _InputIterator1, typename _InputIterator2>
    static bool equal(_InputIterator1 __first1,
                      _InputIterator1 __last1,
                      _InputIterator2 __first2)
    {
        for (; __first1 != __last1; ++__first1, (void)++__first2)
        {
            if (!(*__first1 == *__first2))
            {
                return false;
            }
        }
        return true;
    }
};

template <>
struct __equal<true>
{
    template <typename _T>
    static bool equal(const _T *__first1, const _T *__last1, const _T *__first2)
    {
        if (const size_t __len = (__last1 - __first1))
        {
            return !std::__memcmp(__first1, __first2, __len);
        }

        return true;
    }
};

template <typename _InputIterator1, typename _InputIterator2>
inline bool __equal_aux1(_InputIterator1 __first1,
                         _InputIterator1 __last1,
                         _InputIterator2 __first2)
{
    typedef typename iterator_traits<_InputIterator1>::value_type _ValueType1;
    const bool __simple = ((__is_integer<_ValueType1>::__value ||
                            __is_pointer<_ValueType1>::__value));

    return std::__equal<__simple>::equal(__first1, __last1, __first2);
}

template <typename _InputIterator1, typename _InputIterator2>
inline bool __equal_aux(_InputIterator1 __first1,
                        _InputIterator1 __last1,
                        _InputIterator2 __first2)
{
    return std::__equal_aux1(std::__niter_base(__first1),
                             std::__niter_base(__last1),
                             std::__niter_base(__first2));
}

template <typename _InputIterator1, typename _InputIterator2>
inline bool equal(_InputIterator1 __first1,
                  _InputIterator1 __last1,
                  _InputIterator2 __first2)
{
    return std::__equal_aux(__first1, __last1, __first2);
}

template <typename _InputIterator1,
          typename _InputIterator2,
          typename _BinaryPredicate>
inline bool equal(_InputIterator1 __first1,
                  _InputIterator1 __last1,
                  _InputIterator2 __first2,
                  _BinaryPredicate __binary_pred)
{
    for (; __first1 != __last1; ++__first1, (void)++__first2)
    {
        if (!bool(__binary_pred(*__first1, *__first2)))
        {
            return false;
        }
    }

    return true;
}

template <typename _InputIterator1, typename _InputIterator2>
inline bool __equal4(_InputIterator1 __first1,
                     _InputIterator1 __last1,
                     _InputIterator2 __first2,
                     _InputIterator2 __last2)
{
    using _RATag = random_access_iterator_tag;
    using _Cat1 = typename iterator_traits<_InputIterator1>::iterator_category;
    using _Cat2 = typename iterator_traits<_InputIterator2>::iterator_category;
    using _RAIters = __and_<is_same<_Cat1, _RATag>, is_same<_Cat2, _RATag>>;

    if (_RAIters())
    {
        auto __d1 = std::distance(__first1, __last1);
        auto __d2 = std::distance(__first2, __last2);
        if (__d1 != __d2)
        {
            return false;
        }

        return std::equal(__first1, __last1, __first2);
    }

    for (; __first1 != __last1 && __first2 != __last2;
         ++__first1, (void)++__first2)
    {
        if (!(*__first1 == *__first2))
        {
            return false;
        }
    }

    return __first1 == __last1 && __first2 == __last2;
}

template <typename _InputIterator1,
          typename _InputIterator2,
          typename _BinaryPredicate>
inline bool __equal4(_InputIterator1 __first1,
                     _InputIterator1 __last1,
                     _InputIterator2 __first2,
                     _InputIterator2 __last2,
                     _BinaryPredicate __binary_pred)
{
    using _RATag = random_access_iterator_tag;
    using _Cat1 = typename iterator_traits<_InputIterator1>::iterator_category;
    using _Cat2 = typename iterator_traits<_InputIterator2>::iterator_category;
    using _RAIters = __and_<is_same<_Cat1, _RATag>, is_same<_Cat2, _RATag>>;

    if (_RAIters())
    {
        auto __d1 = std::distance(__first1, __last1);
        auto __d2 = std::distance(__first2, __last2);

        if (__d1 != __d2)
        {
            return false;
        }

        return std::equal(__first1, __last1, __first2, __binary_pred);
    }

    for (; __first1 != __last1 && __first2 != __last2;
         ++__first1, (void)++__first2)
    {
        if (!bool(__binary_pred(*__first1, *__first2)))
        {
            return false;
        }
    }

    return __first1 == __last1 && __first2 == __last2;
}

template <typename _InputIterator1, typename _InputIterator2>
inline bool equal(_InputIterator1 __first1,
                  _InputIterator1 __last1,
                  _InputIterator2 __first2,
                  _InputIterator2 __last2)
{
    return std::__equal4(__first1, __last1, __first2, __last2);
}

template <typename _InputIterator1,
          typename _InputIterator2,
          typename _BinaryPredicate>
inline bool equal(_InputIterator1 __first1,
                  _InputIterator1 __last1,
                  _InputIterator2 __first2,
                  _InputIterator2 __last2,
                  _BinaryPredicate __binary_pred)
{
    return std::__equal4(__first1, __last1, __first2, __last2, __binary_pred);
}

//=============================================================================
// std::lexiocographical_compare
//=============================================================================

//=============================================================================
// std::lexiocographical_compare_three_way
//=============================================================================

//=============================================================================
// std::mismatch
//=============================================================================

template <typename _InputIterator1,
          typename _InputIterator2,
          typename _BinaryPredicate>
pair<_InputIterator1, _InputIterator2> __mismatch(
    _InputIterator1 __first1,
    _InputIterator1 __last1,
    _InputIterator2 __first2,
    _InputIterator2 __last2,
    _BinaryPredicate __binary_pred)
{
    while (__first1 != __last1 && __first2 != __last2 &&
           __binary_pred(__first1, __first2))
    {
        ++__first1;
        ++__first2;
    }

    return pair<_InputIterator1, _InputIterator2>(__first1, __first2);
}

/**
 * @brief Find the place in a range that do not match.
 *
 * @tparam _InputIterator1 Type of the first iterators.
 * @tparam _InputIterator2 Type of the second iterators.
 * @param __first1 Beginning of the first range.
 * @param __last1 End of the first range.
 * @param __first2 Beginning of the second range.
 * @param __last2 End of the second range.
 *
 * @return Pair of iterators, one in the first and one in the second range.
 */
template <typename _InputIterator1, typename _InputIterator2>
inline pair<_InputIterator1, _InputIterator2> mismatch(_InputIterator1 __first1,
                                                       _InputIterator1 __last1,
                                                       _InputIterator2 __first2,
                                                       _InputIterator2 __last2)
{
    return std::__mismatch(__first1,
                           __last1,
                           __first2,
                           __last2,
                           OS6STD::__ops::__iter_equal_to_iter());
}

/**
 * @brief Find the place in a range that do not match.
 *
 * @tparam _InputIterator1 Type of the first iterators.
 * @tparam _InputIterator2 Type of the second iterators.
 * @tparam _BinaryPredicate Type of the compare functor.
 * @param __first1 Beginning of the first range.
 * @param __last1 End of the first range.
 * @param __first2 Beginning of the second range.
 * @param __last2 End of the second range.
 * @param __binary_pred Compare functor.
 *
 * @return Pair of iterators, one in the first and one in the second range.
 */
template <typename _InputIterator1,
          typename _InputIterator2,
          typename _BinaryPredicate>
inline pair<_InputIterator1, _InputIterator2> mismatch(
    _InputIterator1 __first1,
    _InputIterator1 __last1,
    _InputIterator2 __first2,
    _InputIterator2 __last2,
    _BinaryPredicate __binary_pred)
{
    return std::__mismatch(__first1,
                           __last1,
                           __first2,
                           __last2,
                           OS6STD::__ops::__iter_comp_iter(__binary_pred));
}

//=============================================================================
// std::is_permutation
//=============================================================================

/**
 * @brief Find the first element in the range that fulfills the prediate.
 *
 * @tparam _InputIterator Type of the iterators.
 * @tparam _Predicate Type of the predicate.
 * @param __first Start of the range.
 * @param __last End of the range.
 * @param __pred Predicate functor.
 *
 * @return Iterator to first element that fulfills the predicate.
 */
template <typename _InputIterator, typename _Predicate>
inline _InputIterator __find_if(_InputIterator __first,
                                _InputIterator __last,
                                _Predicate __pred,
                                input_iterator_tag)
{
    while (__first != __last && !__pred(__first))
    {
        ++__first;
    }

    return __first;
}

/**
 * @brief Find the first element in the range that fulfills the prediate.
 *
 * Special case for a random access iterator.
 *
 * @tparam _RandomAccessIterator Type of the iterators.
 * @tparam _Predicate Type of the predicate.
 * @param __first Start of the range.
 * @param __last End of the range.
 * @param __pred Predicate functor.
 *
 * @return Iterator to first element that fulfills the predicate.
 */
template <typename _RandomAccessIterator, typename _Predicate>
inline _RandomAccessIterator __find_if(_RandomAccessIterator __first,
                                       _RandomAccessIterator __last,
                                       _Predicate __pred,
                                       random_access_iterator_tag)
{
    using _Distance =
        typename iterator_traits<_RandomAccessIterator>::difference_type;
    _Distance __trip_count = (__last - __first) >> 2;

    for (; __trip_count > 0; --__trip_count)
    {
        if (__pred(__first))
        {
            return __first;
        }

        ++__first;

        if (__pred(__first))
        {
            return __first;
        }

        ++__first;

        if (__pred(__first))
        {
            return __first;
        }

        ++__first;

        if (__pred(__first))
        {
            return __first;
        }

        ++__first;
    }

    switch (__last - __first)
    {
    case 3:
        if (__pred(__first))
        {
            return __first;
        }

        ++__first;
    case 2:
        if (__pred(__first))
        {
            return __first;
        }

        ++__first;
    case 1:
        if (__pred(__first))
        {
            return __first;
        }

        ++__first;
    case 0:
    default:
        return __last;
    }
}

/**
 * @brief Count how many elements in the sequence that the predicate holds for.
 *
 * @tparam _InputIterator Type of the iterator.
 * @tparam _Predicate Type of the compare functor.
 * @param __first Start of the range.
 * @param __last End of the range.
 * @param __pred Binary predicate.
 *
 * @return Number of matches.
 */
template <typename _InputIterator, typename _Predicate>
typename iterator_traits<_InputIterator>::difference_type __count_if(
    _InputIterator __first, _InputIterator __last, _Predicate __pred)
{
    typename iterator_traits<_InputIterator>::difference_type __n = 0;

    for (; __first != __last; ++__first)
    {
        if (__pred(__first))
        {
            ++__n;
        }
    }

    return __n;
}

/**
 * @brief Checks if the first sequence can be permuted to the second sequence.
 *
 * @tparam _ForwardIterator1 Type of the iterators of the first range.
 * @tparam _ForwardIterator2 Type of the iterators of the second range.
 * @tparam _BinaryPrediacte Type of the compare functor.
 * @param __first1 Start of the first range.
 * @param __last1 End of the first range.
 * @param __first2 Start of the second range.
 * @param __pred Compare functor.
 *
 * @return True if permutation.
 */
template <typename _ForwardIterator1,
          typename _ForwardIterator2,
          typename _BinaryPrediacate>
bool __is_permuation(_ForwardIterator1 __first1,
                     _ForwardIterator1 __last1,
                     _ForwardIterator2 __first2,
                     _BinaryPrediacate __pred)
{
    for (; __first1 != __last1; ++__first1, (void)++__first2)
    {
        if (!__pred(__first1, __first2))
        {
            break;
        }
    }

    if (__first1 == __last1)
    {
        return true;
    }

    _ForwardIterator2 __last2 = __first2;

    std::advance(__last2, std::distance(__first1, __last1));

    for (_ForwardIterator1 __scan = __first1; __scan != __last1; ++__scan)
    {
        if (__scan !=
            std::__find_if(__first1,
                           __scan,
                           OS6STD::__ops::__iter_comp_iter(__pred, __scan)))
        {
            continue;
        }

        auto __matches = std::__count_if(
            __first2, __last2, OS6STD::__ops::__iter_comp_iter(__pred, __scan));

        if (__matches == 0 || std::__count_if(__scan,
                                              __last1,
                                              OS6STD::__ops::__iter_comp_iter(
                                                  __pred, __scan)) != __matches)
        {
            return false;
        }
    }

    return true;
}

/**
 * @brief Checks if the first sequence can be permuted to the second sequence.
 *
 * @tparam _ForwardIterator1 Type of the iterators of the first range.
 * @tparam _ForwardIterator2 Type of the iterators of the second range.
 * @param __first1 Start of the first range.
 * @param __last1 End of the first range.
 * @param __first2 Start of the second range.
 *
 * @return True if permutation.
 */
template <typename _ForwardIterator1, typename _ForwardIterator2>
inline bool is_permutation(_ForwardIterator1 __first1,
                           _ForwardIterator1 __last1,
                           _ForwardIterator2 __first2)
{
    return std::__is_permuation(
        __first1, __last1, __first2, OS6STD::__ops::__iter_equal_to_iter());
}

}  // namespace OS6STD

OS6STD_END_SYSTEM_HEADER

#endif

//=============================================================================
// End of file
//=============================================================================
