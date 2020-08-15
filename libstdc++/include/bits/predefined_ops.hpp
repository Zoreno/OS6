/**
 * @file predefined_ops.hpp
 * @author Joakim Bertils
 * @version 0.1
 * @date 2020-07-29
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

#ifndef __PREDEFINED_OPS_HPP
#define __PREDEFINED_OPS_HPP

#pragma GCC system_header

OS6STD_BEGIN_SYSTEM_HEADER

namespace OS6STD
{
namespace __ops
{
/**
 * @brief Construct for comparing two iterators.
 * 
 * 
 */
struct _Iter_less_iter
{
    /**
     * @brief Compares two iterators, returning whether the first is less than the second.
     * 
     * @tparam _Iterator1 Type of the first iterator.
     * @tparam _Iterator2 Type of the second iterator.
     * @param __it1 The first iterator.
     * @param __it2 The second iterator.
     * 
     * @return true if first is less than the second.
     */
    template <typename _Iterator1,
              typename _Iterator2>
    constexpr bool operator()(_Iterator1 __it1,
                              _Iterator2 __it2) const
    {
        return *__it1 < *__it2;
    }
};

/**
 * @brief Constructs and returns a iterator less iterator compare object
 * 
 * 
 * @return _Iter_less_iter Iterator less iterator compare object
 */
inline _Iter_less_iter __iter_less_iter()
{
    return _Iter_less_iter();
}

/**
 * @brief Iterator less than value functor.
 * 
 * 
 */
struct _Iter_less_val
{
    /**
     * @brief Default constructor.
     * 
     * 
     */
    constexpr _Iter_less_val() = default;

    /**
     * @brief Construct a new iter less val object from a iter less iter object.
     * 
     * 
     */
    explicit _Iter_less_val(_Iter_less_iter) {}

    /**
     * @brief Comparing the dereferenced iterator and the value.
     * 
     * @tparam _Iterator Type of the iterator.
     * @tparam _Value Type of the value.
     * @param __it Iterator to dereference.
     * @param __val Value to compare to.
     * 
     * @return true if Iterator value is less than value.
     */
    template <typename _Iterator,
              typename _Value>
    bool operator()(_Iterator __it, _Value& __val) const
    {
        return *__it < __val;
    }
};

/**
 * @brief Returns an iterator less val functor.
 * 
 * 
 * @return Newly constructed functor.
 */
inline _Iter_less_val __iter_less_val()
{
    return _Iter_less_val();
}

/**
 * @brief Returns an iterator less iter functor.
 * 
 * 
 * @return Newly constructed functor.
 */
inline _Iter_less_iter __iter_less_val(_Iter_less_iter)
{
    return _Iter_less_iter();
}

struct _Val_less_iter
{
    // TODO
};

struct _Iter_equal_to_val
{
    // TODO
};

/**
 * @brief Iterator comparison functor.
 * 
 * @tparam _Compare Compare functor type.
 * 
 */
template <typename _Compare>
struct _Iter_comp_iter
{
    /**
     * @brief Compare functor
     * 
     * 
     */
    _Compare __comp;

    /**
     * @brief Constructor taking another functor.
     * 
     * 
     */
    explicit constexpr _Iter_comp_iter(_Compare __comp)
        : __comp(std::move(__comp))
    {
    }

    /**
     * @brief Perform the comparison
     * 
     * @tparam _Iterator1 Type of the first iterator.
     * @tparam _Iterator2 Type of the second iterator.
     * @param __it1 First iterator
     * @param __it2 Second iterator
     * 
     * @return true if compare iterator returns true for the comparison.
     */
    template <typename _Iterator1,
              typename _Iterator2>
    constexpr bool operator()(_Iterator1 __it1,
                              _Iterator2 __it2)
    {
        return bool(__comp(*__it1, *__it2));
    }
};

/**
 * @brief Construct a new iterator functor.
 * 
 * @tparam _Compare Comparison functor type.
 * @param __comp Comparison functor.
 * 
 * @return Newly constructer functor.
 */
template <typename _Compare>
constexpr inline _Iter_comp_iter<_Compare> __iter_comp_iter(_Compare __comp)
{
    return _Iter_comp_iter<_Compare>(std::move(__comp));
}

template <typename _Compare>
struct _Iter_comp_val
{
    _Compare __comp;

    explicit _Iter_comp_val(_Compare __comp)
        : __comp(std::move(__comp))
    {
    }

    explicit _Iter_comp_val(const _Iter_comp_iter<_Compare>& __comp)
        : __comp(__comp.__comp)
    {
    }

    template <typename _Iterator,
              typename _Value>
    bool operator()(_Iterator __it, _Value& __val)
    {
        return bool(__comp(*__it, __val));
    }
};

template <typename _Compare>
inline _Iter_comp_val<_Compare> __iter_comp_val(_Compare __comp)
{
    return _Iter_comp_val<_Compare>(std::move(__comp));
}

template <typename _Compare>
inline _Iter_comp_val<_Compare> __iter_comp_val(_Iter_comp_iter<_Compare> __comp)
{
    return _Iter_comp_val<_Compare>(std::move(__comp));
}

template <typename _Compare>
struct _Val_comp_iter
{
    // TODO
};

template <typename _Value>
struct _Iter_equals_val
{
    // TODO
};

template <typename _Iterator>
struct _Iter_equals_iter
{
    // TODO
};

template <typename _Compare,
          typename _Value>
struct _Iter_comp_to_value
{
    // TODO
};

template <typename _Compare,
          typename _Iterator>
struct _Iter_comp_to_iter
{
    // TODO
};

template <typename _Predicate>
struct _Iter_negate
{
    // TODO
};

}  // namespace __ops
}  // namespace OS6STD

OS6STD_END_SYSTEM_HEADER

#endif
