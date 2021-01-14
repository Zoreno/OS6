/**
 * @file stl_iterator.hpp
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

#ifndef __STL_ITERATOR_HPP
#define __STL_ITERATOR_HPP

#pragma GCC system_header

#include <bits/c++config.hpp>
#include <bits/cpp_type_traits.hpp>
#include <bits/type_traits.hpp>
#include <bits/move.hpp>
#include <bits/ptr_traits.hpp>

#include <type_traits>

OS6STD_BEGIN_SYSTEM_HEADER

namespace OS6STD
{
//=============================================================================
// std::reverse_iterator
//=============================================================================

template <typename _Iterator>
class reverse_iterator
    : public iterator<typename iterator_traits<_Iterator>::iterator_category,
                      typename iterator_traits<_Iterator>::value_type,
                      typename iterator_traits<_Iterator>::difference_type,
                      typename iterator_traits<_Iterator>::pointer,
                      typename iterator_traits<_Iterator>::reference>
{
    template <typename _Iter>
    friend class reverse_iterator;

protected:
    _Iterator current;
    using __traits_type = iterator_traits<_Iterator>;

public:
    using iterator_type = _Iterator;
    using difference_type = typename __traits_type::difference_type;
    using pointer = typename __traits_type::pointer;
    using reference = typename __traits_type::reference;

    reverse_iterator()
        : current()
    {
    }

    explicit reverse_iterator(const reverse_iterator &__x)
        : current(__x.current)
    {
    }

    reverse_iterator &operator=(const reverse_iterator &) = default;

    template <typename _Iter>
    reverse_iterator &operator=(const reverse_iterator &__x)
    {
        current = __x.current;

        return *this;
    }

    iterator_type base() const
    {
        return current;
    }

    reference operator*() const
    {
        _Iterator __tmp = current;
        return *--__tmp;
    }

    pointer operator->() const
    {
        _Iterator __tmp = current;
        --__tmp;
        return _S_to_pointer(__tmp);
    }

    reverse_iterator &operator++()
    {
        --current;

        return *this;
    }

    reverse_iterator operator++(int)
    {
        reverse_iterator __tmp = *this;
        --current;

        return __tmp;
    }

    reverse_iterator &operator--()
    {
        ++current;

        return *this;
    }

    reverse_iterator operator--(int)
    {
        reverse_iterator __tmp = *this;
        ++current;

        return __tmp;
    }

    reverse_iterator operator+(difference_type __n) const
    {
        return reverse_iterator(current - __n);
    }

    reverse_iterator &operator+=(difference_type __n)
    {
        current -= __n;

        return *this;
    }

    reverse_iterator operator-(difference_type __n) const
    {
        return reverse_iterator(current + __n);
    }

    reverse_iterator &operator-=(difference_type __n)
    {
        current += __n;

        return *this;
    }

    reference operator[](difference_type __n) const
    {
        return *(*this + __n);
    }

private:
    template <typename _T>
    static _T _S_to_pointer(_T *__p)
    {
        return __p;
    }

    template <typename _T>
    static _T _S_to_pointer(_T __t)
    {
        return __t.operator->();
    }
};

template <typename _Iterator>
inline bool
operator==(const reverse_iterator<_Iterator> &__x,
           const reverse_iterator<_Iterator> &__y)
{
    return __x.base() == __y.base();
}

template <typename _Iterator>
inline bool
operator<(const reverse_iterator<_Iterator> &__x,
          const reverse_iterator<_Iterator> &__y)
{
    return __y.base() < __x.base();
}

template <typename _Iterator>
inline bool
operator!=(const reverse_iterator<_Iterator> &__x,
           const reverse_iterator<_Iterator> &__y)
{
    return !(__x == __y);
}

template <typename _Iterator>
inline bool
operator>(const reverse_iterator<_Iterator> &__x,
          const reverse_iterator<_Iterator> &__y)
{
    return __y < __x;
}

template <typename _Iterator>
inline bool
operator<=(const reverse_iterator<_Iterator> &__x,
           const reverse_iterator<_Iterator> &__y)
{
    return !(__y < __x);
}

template <typename _Iterator>
inline bool
operator>=(const reverse_iterator<_Iterator> &__x,
           const reverse_iterator<_Iterator> &__y)
{
    return !(__x < __y);
}

template <typename _IteratorL, typename _IteratorR>
inline bool
operator==(const reverse_iterator<_IteratorL> &__x,
           const reverse_iterator<_IteratorR> &__y)
{
    return __x.base() == __y.base();
}

template <typename _IteratorL, typename _IteratorR>
inline bool
operator<(const reverse_iterator<_IteratorL> &__x,
          const reverse_iterator<_IteratorR> &__y)
{
    return __x.base() > __y.base();
}

template <typename _IteratorL, typename _IteratorR>
inline bool
operator!=(const reverse_iterator<_IteratorL> &__x,
           const reverse_iterator<_IteratorR> &__y)
{
    return __x.base() != __y.base();
}

template <typename _IteratorL, typename _IteratorR>
inline bool
operator>(const reverse_iterator<_IteratorL> &__x,
          const reverse_iterator<_IteratorR> &__y)
{
    return __x.base() < __y.base();
}

template <typename _IteratorL, typename _IteratorR>
inline bool
operator<=(const reverse_iterator<_IteratorL> &__x,
           const reverse_iterator<_IteratorR> &__y)
{
    return __x.base() >= __y.base();
}

template <typename _IteratorL, typename _IteratorR>
inline bool
operator>=(const reverse_iterator<_IteratorL> &__x,
           const reverse_iterator<_IteratorR> &__y)
{
    return __x.base() <= __y.base();
}

template <typename _IteratorL, typename _IteratorR>
inline auto
operator-(const reverse_iterator<_IteratorL> &__x,
          const reverse_iterator<_IteratorR> &__y)
    -> decltype(__y.base() - __x.base())
{
    return __y.base() - __x.base();
}

template <typename _Iterator>
inline reverse_iterator<_Iterator>
operator+(typename reverse_iterator<_Iterator>::difference_type __n,
          const reverse_iterator<_Iterator> &__x)
{
    return reverse_iterator<_Iterator>(__x.base() - __n);
}

template <typename _Iterator>
inline reverse_iterator<_Iterator> make_reverse_iterator(_Iterator __i)
{
    return reverse_iterator<_Iterator>(__i);
}

//=============================================================================
// std::back_insert_iterator
//=============================================================================

template <typename _Container>
class back_insert_iterator
    : public iterator<output_iterator_tag,
                      void,
                      void,
                      void,
                      void>
{
protected:
    _Container *container;

public:
    using container_type = _Container;

    explicit back_insert_iterator(_Container &__x)
        : container(std::__addressof(__x))
    {
    }

    back_insert_iterator &operator=(const typename _Container::value_type &__value)
    {
        container->push_back(__value);

        return *this;
    }

    back_insert_iterator &operator=(typename _Container::value_typ &&__value)
    {
        container->push_back(std::move(__value));

        return *this;
    }

    back_insert_iterator &operator*()
    {
        return *this;
    }

    back_insert_iterator &operator++()
    {
        return *this;
    }

    back_insert_iterator operator++(int)
    {
        return *this;
    }
};

template <typename _Container>
inline back_insert_iterator<_Container> back_inserter(_Container &__x)
{
    return back_insert_iterator<_Container>(__x);
}

//=============================================================================
// std::front_insert_iterator
//=============================================================================

template <typename _Container>
class front_insert_iterator
    : public iterator<output_iterator_tag,
                      void,
                      void,
                      void,
                      void>
{
protected:
    _Container *container;

public:
    using container_type = _Container;

    explicit front_insert_iterator(_Container &__x)
        : container(std::__addressof(__x))
    {
    }

    front_insert_iterator &operator=(const typename _Container::value_type &__value)
    {
        container->push_front(__value);

        return *this;
    }

    front_insert_iterator &operator=(typename _Container::value_typ &&__value)
    {
        container->push_front(std::move(__value));

        return *this;
    }

    front_insert_iterator &operator*()
    {
        return *this;
    }

    front_insert_iterator &operator++()
    {
        return *this;
    }

    front_insert_iterator operator++(int)
    {
        return *this;
    }
};

template <typename _Container>
inline front_insert_iterator<_Container> front_inserter(_Container &__x)
{
    return front_insert_iterator<_Container>(__x);
}

//=============================================================================
// std::insert_iterator
//=============================================================================

template <typename _Container>
class insert_iterator
    : public iterator<output_iterator_tag,
                      void,
                      void,
                      void,
                      void>
{
    using _Iter = typename _Container::iterator;

protected:
    _Container *container;
    _Iter iter;

public:
    using container_type = _Container;

    explicit insert_iterator(_Container &__x, _Iter __i)
        : container(std::__addressof(__x), iter(__i))
    {
    }

    insert_iterator &operator=(const typename _Container::value_type &__value)
    {
        iter = container->insert(iter, __value);
        ++iter;
        return *this;
    }

    insert_iterator &operator=(typename _Container::value_typ &&__value)
    {
        iter = container->insert(iter, std::move(__value));
        ++iter;
        return *this;
    }

    insert_iterator &operator*()
    {
        return *this;
    }

    insert_iterator &operator++()
    {
        return *this;
    }

    insert_iterator operator++(int)
    {
        return *this;
    }
};

template <typename _Container>
inline insert_iterator<_Container>
inserter(_Container &__x,
         typename _Container::iterator __i)
{
    return insert_iterator<_Container>(__x, __i);
}

//=============================================================================
// std::__normal_iterator
//=============================================================================

template <typename _Iterator, typename _Container>
class __normal_iterator
{
protected:
    _Iterator _current;

    using __traits_type = std::iterator_traits<_Iterator>;

public:
    using iterator_type = _Iterator;
    using iterator_category = typename __traits_type::iterator_category;
    using value_type = typename __traits_type::value_type;
    using difference_type = typename __traits_type::difference_type;
    using reference = typename __traits_type::reference;
    using pointer = typename __traits_type::pointer;

    // Constructors
    __normal_iterator()
        : _current(_Iterator())
    {
    }

    __normal_iterator(const _Iterator &__i)
        : _current(__i)
    {
    }

    template <typename _Iter>
    __normal_iterator(const __normal_iterator<_Iter,
                                              typename __enable_if<(std::__are_same<_Iter,
                                                                                    typename _Container::pointer>::__value),
                                                                   _Container>::__type> &__i)
        : _current(__i.base())
    {
    }

    reference operator*() const
    {
        return *_current;
    }

    pointer operator->() const
    {
        return _current;
    }

    __normal_iterator &operator++()
    {
        ++_current;
        return *this;
    }

    __normal_iterator operator++(int)
    {
        return __normal_iterator(_current++);
    }

    __normal_iterator &operator--()
    {
        --_current;
        return *this;
    }

    __normal_iterator operator--(int)
    {
        return __normal_iterator(_current--);
    }

    reference operator[](difference_type __n) const
    {
        return _current[__n];
    }

    __normal_iterator &operator+=(difference_type __n)
    {
        _current += __n;
        return *this;
    }

    __normal_iterator operator+(difference_type __n) const
    {
        return __normal_iterator(_current + __n);
    }

    __normal_iterator &operator-=(difference_type __n)
    {
        _current -= __n;
        return *this;
    }

    __normal_iterator operator-(difference_type __n) const
    {
        return __normal_iterator(_current - __n);
    }

    const _Iterator &base() const
    {
        return _current;
    }
};

template <typename _IteratorL, typename _IteratorR, typename _Container>
inline bool operator==(const __normal_iterator<_IteratorL, _Container> &__lhs,
                       const __normal_iterator<_IteratorR, _Container> &__rhs)

{
    return __lhs.base() == __rhs.base();
}

template <typename _Iterator, typename _Container>
inline bool operator==(const __normal_iterator<_Iterator, _Container> &__lhs,
                       const __normal_iterator<_Iterator, _Container> &__rhs)

{
    return __lhs.base() == __rhs.base();
}

template <typename _IteratorL, typename _IteratorR, typename _Container>
inline bool operator!=(const __normal_iterator<_IteratorL, _Container> &__lhs,
                       const __normal_iterator<_IteratorR, _Container> &__rhs)

{
    return __lhs.base() != __rhs.base();
}

template <typename _Iterator, typename _Container>
inline bool operator!=(const __normal_iterator<_Iterator, _Container> &__lhs,
                       const __normal_iterator<_Iterator, _Container> &__rhs)

{
    return __lhs.base() != __rhs.base();
}

// Random access iterator requirements
template <typename _IteratorL, typename _IteratorR, typename _Container>
inline bool operator<(const __normal_iterator<_IteratorL, _Container> &__lhs,
                      const __normal_iterator<_IteratorR, _Container> &__rhs)

{
    return __lhs.base() < __rhs.base();
}

template <typename _Iterator, typename _Container>
inline bool operator<(const __normal_iterator<_Iterator, _Container> &__lhs,
                      const __normal_iterator<_Iterator, _Container> &__rhs)

{
    return __lhs.base() < __rhs.base();
}

template <typename _IteratorL, typename _IteratorR, typename _Container>
inline bool operator>(const __normal_iterator<_IteratorL, _Container> &__lhs,
                      const __normal_iterator<_IteratorR, _Container> &__rhs)

{
    return __lhs.base() > __rhs.base();
}

template <typename _Iterator, typename _Container>
inline bool operator>(const __normal_iterator<_Iterator, _Container> &__lhs,
                      const __normal_iterator<_Iterator, _Container> &__rhs)

{
    return __lhs.base() > __rhs.base();
}

template <typename _IteratorL, typename _IteratorR, typename _Container>
inline bool operator<=(const __normal_iterator<_IteratorL, _Container> &__lhs,
                       const __normal_iterator<_IteratorR, _Container> &__rhs)

{
    return __lhs.base() <= __rhs.base();
}

template <typename _Iterator, typename _Container>
inline bool operator<=(const __normal_iterator<_Iterator, _Container> &__lhs,
                       const __normal_iterator<_Iterator, _Container> &__rhs)

{
    return __lhs.base() <= __rhs.base();
}

template <typename _IteratorL, typename _IteratorR, typename _Container>
inline bool operator>=(const __normal_iterator<_IteratorL, _Container> &__lhs,
                       const __normal_iterator<_IteratorR, _Container> &__rhs)

{
    return __lhs.base() >= __rhs.base();
}

template <typename _Iterator, typename _Container>
inline bool operator>=(const __normal_iterator<_Iterator, _Container> &__lhs,
                       const __normal_iterator<_Iterator, _Container> &__rhs)

{
    return __lhs.base() >= __rhs.base();
}

//=============================================================================
// std::move_iterator
//=============================================================================

// TODO: Implement the move iterator

}  // namespace OS6STD

OS6STD_END_SYSTEM_HEADER

#endif

//=============================================================================
// End of file
//=============================================================================
