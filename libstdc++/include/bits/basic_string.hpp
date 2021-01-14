/**
 * @file basic_string.hpp
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

#ifndef __BASIC_STRING_HPP
#define __BASIC_STRING_HPP

#pragma GCC system_header

#include <initializer_list>

OS6STD_BEGIN_SYSTEM_HEADER

namespace OS6STD
{
/**
 * @brief Basic string.
 * 
 * Used for handling strings and sequences of char-like objects.
 * 
 * @tparam _CharType Type of the charater to store.
 * @tparam _Traits Traits for the character type.
 * @tparam _Alloc Allocator type.
 * 
 */
template <typename _CharType, typename _Traits, typename _Alloc>
class basic_string
{
    using _char_alloc_type = typename __alloc_traits<_Alloc>::template rebind<_CharType>::other;
    using _alloc_traits = __alloc_traits<_char_alloc_type>;

public:
    using traits_type = _Traits;
    using value_type = typename _Traits::char_type;
    using allocator_type = _char_alloc_type;
    using size_type = typename _alloc_traits::size_type;
    using difference_type = typename _alloc_traits::difference_type;
    using reference = typename _alloc_traits::reference;
    using const_reference = typename _alloc_traits::const_reference;
    using pointer = typename _alloc_traits::pointer;
    using const_pointer = typename _alloc_traits::const_pointer;
    using iterator = __normal_iterator<pointer, basic_string>;
    using const_iterator = __normal_iterator<const_pointer, basic_string>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    static const size_type npos = static_cast<size_type>(-1);

protected:
    using __const_iterator = const_iterator;

private:
    //==========================================================================
    // Private structures
    //==========================================================================

    struct _Alloc_hider : allocator_type
    {
        _Alloc_hider(pointer __data, const _Alloc &__a)
            : allocator_type(__a),
              _p(__data)
        {
        }

        _Alloc_hider(pointer __data, const _Alloc &&__a = _Alloc())
            : allocator_type(std::move(__a)),
              _p(__data)
        {
        }

        pointer _p;
    };

    //==========================================================================
    // Local variables
    //==========================================================================

    _Alloc_hider _dataplus;
    size_type _string_length;

    enum
    {
        _local_capacity = 15 / sizeof(_CharType)
    };

    union
    {
        _CharType _local_buffer[_local_capacity + 1];
        size_type _allocated_capacity;
    };

    //==========================================================================
    // Local functions
    //==========================================================================

    void _data(pointer __p)
    {
        _dataplus._p = __p;
    }

    void _length(size_type __length)
    {
        _string_length = __length;
    }

    pointer _data() const
    {
        return _dataplus._p;
    }

    pointer _local_data()
    {
        return std::pointer_traits<pointer>::pointer_to(*_local_buffer);
    }

    const_pointer _local_data() const
    {
        return std::pointer_traits<const_pointer>::pointer_to(*_local_buffer);
    }

    void _capacity(size_type __capacity)
    {
        _allocated_capacity = __capacity;
    }

    void _set_length(size_type __n)
    {
        _length(__n);
        traits_type::assign(_data()[__n], _CharType());
    }

    bool _is_local() const
    {
        return _data() == _local_data();
    }

    // TODO: Implement
    pointer _create(size_type &, size_type);

    void _dispose()
    {
        if (!_is_local())
        {
            _destroy(_allocated_capacity);
        }
    }

    void _destroy(size_type __size) throw()
    {
        _alloc_traits::deallocate(_get_allocator(), _data(), __size + 1);
    }

    template <typename _InIterator>
    void _construct_aux(_InIterator __beg, _InIterator __end, std::__false_type)
    {
        using _Tag = typename iterator_traits<_InIterator>::iterator_category;

        _construct(__beg, __end, _Tag());
    }

    template <typename _Integer>
    void _construct_aux(_Integer __beg, _Integer __end, std::__true_type)
    {
        _construct_aux_2(static_cast<size_type>(__beg), __end);
    }

    void _construct_aux_2(size_type __req, _CharType __c)
    {
        _construct(__req, __c);
    }

    template <typename _InIterator>
    void _construct(_InIterator __beg, _InIterator __end)
    {
        using _Integral = typename std::__is_integer<_InIterator>::__type;

        _construct_aux(__beg, __end, _Integral());
    }

    template <typename _InIterator>
    void _construct(_InIterator __beg, _InIterator __end, std::input_iterator_tag);

    template <typename _FwdIterator>
    void _construct(_FwdIterator __beg, _FwdIterator __end, std::forward_iterator_tag);

    void _construct(size_type __req, _CharType __c);

    allocator_type &_get_allocator()
    {
        return _dataplus;
    }

    const allocator_type &_get_allocator() const
    {
        return _dataplus;
    }

    size_type _check(size_type __pos, const char *__s) const
    {
        if (__pos > size())
        {
            // TODO: This should be handled properly
            throw 1;
        }

        return __pos;
    }

    void _check_length(size_type __n1, size_type __n2, const char *__s) const
    {
        if (max_size() - (size() - __n1) < __n2)
        {
            // TODO: This should be handled properly
            throw 1;
        }
    }

    size_type _limit(size_type __pos, size_type __off) const noexcept
    {
        const bool __testoff = __off < size() - __pos;

        return __testoff ? __off : size() - __pos;
    }

    bool _disjunct(const _CharType *__s) const noexcept
    {
        return (less<const _CharType *>()(__s, _data()) || less<const _CharType *>()(_data() + size(), __s));
    }

    void _assign(const basic_string &);
    void _mutate(size_type __pos, size_type __len1, const _CharType *__s, size_type __len2);
    void _erase(size_type __pos, size_type __n);

    template <typename _Integer>
    basic_string &_replace_dispatch(const_iterator __i1,
                                    const_iterator __i2,
                                    _Integer __n,
                                    _Integer __val,
                                    __true_type)
    {
        return _replace_aux(__i1 - begin(), __i2 - __i1, __n, __val);
    }

    template <typename _InputIterator>
    basic_string &_replace_dispatch(const_iterator __i1,
                                    const_iterator __i2,
                                    _InputIterator __k1,
                                    _InputIterator __k2,
                                    __false_type);

    basic_string &_replace_aux(size_type __pos1,
                               size_type __n1,
                               size_type __n2,
                               _CharType __c);

    basic_string &_replace(size_type __pos,
                           size_type __len1,
                           const _CharType *__s,
                           const size_type __len2);

    basic_string &_append(const _CharType *__s, size_type __n);

    //==========================================================================
    // Private static functions
    //==========================================================================

    static void _copy(_CharType *__d, const _CharType *__s, size_type __n)
    {
        if (__n == 1)
        {
            traits_type::assign(*__d, *__s);
        }
        else
        {
            traits_type::copy(__d, __s, __n);
        }
    }

    static void _move(_CharType *__d, const _CharType *__s, size_type __n)
    {
        if (__n == 1)
        {
            traits_type::assign(*__d, *__s);
        }
        else
        {
            traits_type::move(__d, __s, __n);
        }
    }

    static void _assign(_CharType *__d, size_type __n, _CharType __c)
    {
        if (__n == 1)
        {
            traits_type::assign(*__d, *__c);
        }
        else
        {
            traits_type::assign(__d, __n, __c);
        }
    }

    template <typename _Iterator>
    static void _copy_chars(_CharType *__p, _Iterator __k1, _Iterator __k2)
    {
        for (; __k1 != __k2; ++__k1, (void)++__p)
        {
            traits_type::assign(*__p, *__k1);
        }
    }

    static void _copy_chars(_CharType *__p, iterator __k1, iterator __k2) noexcept
    {
        _copy_chars(__p, __k1.base(), __k2.base());
    }

    static void _copy_chars(_CharType *__p, const_iterator __k1, const_iterator __k2) noexcept
    {
        _copy_chars(__p, __k1.base(), __k2.base());
    }

    static void _copy_chars(_CharType *__p, _CharType *__k1, _CharType *__k2) noexcept
    {
        _copy(__p, __k1, __k2 - __k1);
    }

    static int _compare(size_type __n1, size_type __n2) noexcept
    {
        const difference_type __d = difference_type(__n1 - __n2);

        if (__d > __numeric_traits<int>::__max)
        {
            return __numeric_traits<int>::__max;
        }
        else if (__d > __numeric_traits<int>::__min)
        {
            return __numeric_traits<int>::__min;
        }
        else
        {
            return int(__d);
        }
    }

public:
    //==========================================================================
    // Constructors
    //==========================================================================

    basic_string()
        : _dataplus(_local_data())
    {
        _set_length(0);
    }

    explicit basic_string(const _Alloc &__a) noexcept
        : _dataplus(_local_data(), __a)
    {
        _set_length(0);
    }

    basic_string(const basic_string &__str)
        : _dataplus(_local_data(),
                    _alloc_traits::_select_on_copy(__str._get_allocator()))
    {
        _construct(__str._data(), __str._data() + __str.length());
    }

    basic_string(const basic_string &__str,
                 size_type __pos,
                 const _Alloc &__a = _Alloc())
        : _dataplus(_local_data(), __a)
    {
        const _CharType *__start = __str._data() + __str._check(__pos, "basic_string::basic_string");
        _construct(__start, __start + __str._limit(__pos, npos));
    }

    basic_string(const basic_string &__str,
                 size_type __pos,
                 size_type __n,
                 const _Alloc &__a = _Alloc())
        : _dataplus(_local_data(), __a)
    {
        const _CharType *__start = __str._data() + __str._check(__pos, "basic_string::basic_string");
        _construct(__start, __start + __str._limit(__pos, __n));
    }

    basic_string(const _CharType *__s,
                 size_type __n,
                 const _Alloc &__a = _Alloc())
        : _dataplus(_local_data(), __a)
    {
        _construct(__s, __s + __n);
    }

    basic_string(const _CharType *__s, const _Alloc &__a = _Alloc())
        : _dataplus(_local_data(), __a)
    {
        _construct(__s, __s ? __s + traits_type::length(__s) : __s + npos);
    }

    basic_string(size_type __n, _CharType __c, const _Alloc &__a = _Alloc())
        : _dataplus(_local_data(), __a)
    {
        _construct(__n, __c);
    }

    basic_string(basic_string &&__str) noexcept
        : _dataplus(_local_data(), std::move(__str._get_allocator()))
    {
        if (__str._is_local())
        {
            traits_type::copy(_local_buffer, __str._local_buffer, _local_capacity + 1);
        }
        else
        {
            _data(__str._data());
            _capacity(__str._allocated_capacity);
        }

        _length(__str.length());
        __str._data(__str._local_data());
        __str._set_length(0);
    }

    basic_string(initializer_list<_CharType> __l, const _Alloc &__a = _Alloc())
        : _dataplus(_local_data(), __a)
    {
        _construct(__l.begin(), __l.end());
    }

    basic_string(const basic_string &__str, const _Alloc &__a)
        : _dataplus(_local_data(), __a)
    {
        _construct(__str.begin(), __str.end());
    }

    basic_string(basic_string &&__str, const _Alloc &__a) noexcept(_alloc_traits::_always_equal())
        : _dataplus(_local_data(), __a)
    {
        // Local data can just be directly copied.
        if (__str._is_local())
        {
            traits_type::copy(_local_buffer, __str._local_buffer, _local_capacity + 1);
            _length(__str.length());
            __str._set_length(0);
        }
        // Strings allocated from the same allocator can just steal pointers.
        else if (_alloc_traits::_always_equal() || __str.get_allocator() == __a)
        {
            _data(__str._data());
            _length(__str._length());
            _capacity(__str._allocated_capacity);
            __str._data(__str.local_buffer);
            __str._set_length(0);
        }
        // Construct a new string from the old one.
        else
        {
            _construct(__str.begin(), __str.end());
        }
    }

    template <typename _InputIterator,
              typename = std::_RequireInputIter<_InputIterator>>
    basic_string(_InputIterator __beg, _InputIterator __end, const _Alloc &__a = _Alloc())
        : _dataplus(_local_data(), __a)
    {
        _construct(__beg, __end);
    }

    //==========================================================================
    // Destructor
    //==========================================================================

    ~basic_string()
    {
        _dispose();
    }

    //==========================================================================
    // operator=
    //==========================================================================

    basic_string &operator=(const basic_string &__str)
    {
        return assign(__str);
    }

    basic_string &operator=(const _CharType *__s)
    {
        return assign(__s);
    }

    basic_string &operator=(const _CharType __c)
    {
        assign(1, __c);
        return *this;
    }

    // TODO: Move assignment operator

    basic_string &operator=(initializer_list<_CharType> __l)
    {
        assign(__l.begin(), __l.size());
        return *this;
    }

    //==========================================================================
    // Iterators
    //==========================================================================

    iterator begin() noexcept
    {
        return iterator(_data());
    }

    const_iterator begin() const noexcept
    {
        return const_iterator(_data());
    }

    iterator end() noexcept
    {
        return iterator(_data() + size());
    }

    const_iterator end() const noexcept
    {
        return const_iterator(_data() + size());
    }

    reverse_iterator rbegin() noexcept
    {
        return reverse_iterator(end());
    }

    const_reverse_iterator rbegin() const noexcept
    {
        return const_reverse_iterator(end());
    }

    reverse_iterator rend() noexcept
    {
        return reverse_iterator(begin());
    }

    const_reverse_iterator rend() const noexcept
    {
        return const_reverse_iterator(begin());
    }

    const_iterator cbegin() const noexcept
    {
        return const_iterator(_data());
    }

    const_iterator cend() const noexcept
    {
        return const_iterator(_data() + size());
    }

    const_reverse_iterator crbegin() const noexcept
    {
        return const_reverse_iterator(end());
    }

    const_reverse_iterator crend() const noexcept
    {
        return const_reverse_iterator(begin());
    }

    //==========================================================================
    // Size
    //==========================================================================

    size_type size() const noexcept
    {
        return _string_length;
    }

    //==========================================================================
    // length
    //==========================================================================

    size_type length() const noexcept
    {
        return _string_length;
    }

    //==========================================================================
    // max_size
    //==========================================================================

    size_type max_size() const noexcept
    {
        return (_alloc_traits::max_size(_get_allocator()) - 1) / 2;
    }

    //==========================================================================
    // resize
    //==========================================================================

    void resize(size_type __n, _CharType __c);

    void resize(size_type __n)
    {
        resize(__n, _CharType());
    }

    //==========================================================================
    // shrink_to_fit
    //==========================================================================

    void shrink_to_fit() noexcept
    {
        reserve();
    }

    //==========================================================================
    // capacity
    //==========================================================================

    size_type capacity() const noexcept
    {
        return _is_local() ? size_type(_local_capacity) : _allocated_capacity;
    }

    //==========================================================================
    // reseve
    //==========================================================================

    void reserve(size_type __size);
    void reserve();

    //==========================================================================
    // clear
    //==========================================================================

    void clear() noexcept
    {
        _set_length(0);
    }

    //==========================================================================
    // empty
    //==========================================================================

    bool empty() noexcept
    {
        return size() == 0;
    }

    //==========================================================================
    // operator[]
    //==========================================================================

    const_reference operator[](size_type __pos) const noexcept
    {
        return _data()[__pos];
    }

    reference operator[](size_type __pos) noexcept
    {
        return _data()[__pos];
    }

    //==========================================================================
    // at
    //==========================================================================

    const_reference at(size_type __n) const
    {
        if (__n >= size())
        {
            // TODO: Handle this properly
            throw 1;
        }

        return _data()[__n];
    }

    reference at(size_type __n)
    {
        if (__n >= size())
        {
            // TODO: Handle this properly
            throw 1;
        }

        return _data()[__n];
    }

    //==========================================================================
    // front
    //==========================================================================

    reference front() noexcept
    {
        return operator[](0);
    }

    const_reference front() const noexcept
    {
        return operator[](0);
    }

    //==========================================================================
    // back
    //==========================================================================

    reference back() noexcept
    {
        return operator[](size() - 1);
    }

    const_reference back() const noexcept
    {
        return operator[](size() - 1);
    }

    //==========================================================================
    // operator+=
    //==========================================================================

    basic_string &operator+=(const basic_string &__str)
    {
        return append(__str);
    }

    basic_string &operator+=(const _CharType *__str)
    {
        return append(__str);
    }

    basic_string &operator+=(_CharType __c)
    {
        push_back(__c);

        return *this;
    }

    basic_string &operator+=(initializer_list<_CharType> __l)
    {
        return append(__l.begin(), __l.end());
    }

    //==========================================================================
    // append
    //==========================================================================

    basic_string &append(const basic_string &__str)
    {
        return _append(__str._data(), __str.size());
    }

    basic_string &append(const basic_string &__str, size_type __pos, size_type __n = npos)
    {
        return _append(__str._data() + __str._check(__pos, ""), __str._limit(__pos, __n));
    }

    basic_string &append(const _CharType *__s, size_type __n)
    {
        _check_length(size_type(0), __n, "");
        return _append(__s, __n);
    }

    basic_string &append(const _CharType *__s)
    {
        const size_type __n = traits_type::length(__s);

        return append(__s, __n);
    }

    basic_string &append(size_type __n, _CharType __c)
    {
        return _replace_aux(size(), size_type(0), __n, __c);
    }

    basic_string &append(initializer_list<_CharType> __l)
    {
        return append(__l.begin(), __l.end());
    }

    template <typename _InputIterator>
    basic_string &append(_InputIterator __first, _InputIterator __last)
    {
        return replace(end(), end(), __first, __last);
    }

    //==========================================================================
    // push_back
    //==========================================================================

    void push_back(_CharType __c)
    {
        const size_type __size = size();
        if (__size + 1 > capacity())
        {
            _mutate(__size, size_type(0), 0, size_type(1));
        }

        traits_type::assign(_data()[__size], __c);
        _set_length(__size + 1);
    }

    //==========================================================================
    // assign
    //==========================================================================

    basic_string &assign(const basic_string &__str)
    {
        this->_assign(__str);

        return *this;
    }

    basic_string &assign(basic_string &&__str) noexcept(_alloc_traits::_nothrow_move())
    {
        *this = std::move(__str);

        return *this;
    }

    basic_string &assign(basic_string &__str, size_type __pos, size_type __n = npos)
    {
        return _replace(size_type(0),
                        size(),
                        _data() + __str._check(__pos, ""),
                        __str._limit(__pos, __n));
    }

    basic_string &assign(const _CharType *__s, size_type __n)
    {
        return _replace(size_type(0), size(), __s, __n);
    }

    basic_string &assign(const _CharType *__s)
    {
        return _replace(size_type(0), size(), __s, traits_type::length(__s));
    }

    template <typename _InputIterator>
    basic_string &assign(_InputIterator __first, _InputIterator __last)
    {
        return replace(begin(), end(), __first, __last);
    }

    basic_string &assign(initializer_list<_CharType> __i)
    {
        return assign(__i.begin(), __i.end());
    }

    //==========================================================================
    // insert
    //==========================================================================

    iterator insert(const_iterator __p, size_type __n, _CharType __c)
    {
        const size_type __pos = __p - begin();

        replace(__p, __p, __n, __c);

        return iterator(data() + __pos);
    }

    template <typename _InputIterator, typename = std::_RequireInputIter<_InputIterator>>
    iterator insert(const_iterator __p, _InputIterator __beg, _InputIterator __end)
    {
        const size_type __pos = __p - begin();

        replace(__p, __p, __beg, __end);

        return iterator(_data() + __pos);
    }

    iterator insert(const_iterator __p, initializer_list<_CharType> __l)
    {
        return insert(__p, __l.begin(), __l.end());
    }

    basic_string &insert(size_type __pos1, const basic_string &__str)
    {
        return replace(__pos1, size_type(0), __str._data(), __str.size());
    }

    basic_string &insert(size_type &__pos1,
                         const basic_string &__str,
                         size_type __pos2,
                         size_type __n = npos)
    {
        return replace(__pos1,
                       size_type(0),
                       __str._data() + __str._check(__pos2, ""),
                       __str._limit(__pos2, __n));
    }

    basic_string &insert(size_type __pos, const _CharType *__s, size_type __n)
    {
        return replace(__pos, size_type(0), __s, __n);
    }

    basic_string &insert(size_type __pos, const _CharType *__s)
    {
        return replace(__pos, size_type(0), __s, traits_type::length(__s));
    }

    basic_string &insert(size_type __pos, size_type __n, _CharType __c)
    {
        return _replace_aux(_check(__pos, ""), size_type(0), __n, __c);
    }

    iterator insert(const_iterator __p, _CharType __c)
    {
        const size_type __pos = __p - begin();

        _replace_aux(__pos, size_type(0), size_type(1), __c);

        return iterator(_data() + __pos);
    }

    //==========================================================================
    // erase
    //==========================================================================

    basic_string &erase(size_type __pos = 0, size_type __n = npos)
    {
        if (__n == npos)
        {
            _set_length(__pos);
        }
        else if (__n != 0)
        {
            _erase(__pos, _limit(__pos, __n));
        }

        return *this;
    }

    iterator erase(const_iterator __position)
    {
        const size_type __pos = __position - begin();

        _erase(__pos, size_type(1));

        return iterator(_data() + __pos);
    }

    iterator erase(const_iterator __first, const_iterator __last)
    {
        const size_type __pos = __first - begin();

        if (__last == end())
        {
            _set_length(__pos);
        }
        else
        {
            _erase(__pos, __last - __first);
        }

        return iterator(_data() + __pos);
    }

    //==========================================================================
    // pop_back
    //==========================================================================

    void pop_back() noexcept
    {
        _erase(size() - 1, 1);
    }

    //==========================================================================
    // replace
    //==========================================================================

    basic_string &replace(size_type __pos, size_type __n, const basic_string &__str)
    {
        replace(__pos, __n, __str._data(), __str.size());
    }

    basic_string &replace(size_type __pos1,
                          size_type __n1,
                          const basic_string &__str,
                          size_type __pos2,
                          size_type __n2 = npos)
    {
        return replace(__pos1,
                       __n1,
                       __str._data() + __str._check(__pos2, ""),
                       __str._limit(__pos2, __n2));
    }

    basic_string &replace(size_type __pos,
                          size_type __n1,
                          const _CharType *__s,
                          size_type __n2)
    {
        return _replace(_check(__pos, ""),
                        _limit(__pos, __n1),
                        __s,
                        __n2);
    }

    basic_string &replace(size_type __pos, size_type __n1, const _CharType *__s)
    {
        return replace(__pos, __n1, __s, traits_type::length(__s));
    }

    basic_string &replace(size_type __pos, size_type __n1, size_type __n2, _CharType __c)
    {
        return _replace_aux(_check(__pos, ""), _limit(__pos, __n1), __n2, __c);
    }

    basic_string &replace(const_iterator __i1, const_iterator __i2, const basic_string &__str)
    {
        return replace(__i1, __i2, __str._data(), __str.size());
    }

    basic_string &replace(const_iterator __i1, const_iterator __i2, const _CharType *__s, size_type __n)
    {
        return replace(__i1 - begin(), __i2 - __i1, __s, __n);
    }

    basic_string &replace(const_iterator __i1, const_iterator __i2, const _CharType *__s)
    {
        return replace(__i1, __i2, __s, traits_type::length(__s));
    }

    basic_string &replace(const_iterator __i1, const_iterator __i2, size_type __n, _CharType __c)
    {
        return _replace_aux(__i1 - begin(), __i2 - __i1, __n, __c);
    }

    template <typename _InputIterator,
              typename = std::_RequireInputIter<_InputIterator>>
    basic_string &replace(const_iterator __i1,
                          const_iterator __i2,
                          _InputIterator __k1,
                          _InputIterator __k2)
    {
        return _replace_dispatch(__i1,
                                 __i2,
                                 __k1,
                                 __k2,
                                 std::__false_type());
    }

    basic_string &replace(const_iterator __i1,
                          const_iterator __i2,
                          _CharType *__k1,
                          _CharType *__k2)
    {
        return replace(__i1 - begin(),
                       __i2 - __i1,
                       __k1,
                       __k2 - __k1);
    }

    basic_string &replace(const_iterator __i1,
                          const_iterator __i2,
                          const _CharType *__k1,
                          const _CharType *__k2)
    {
        return replace(__i1 - begin(),
                       __i2 - __i1,
                       __k1,
                       __k2 - __k1);
    }

    basic_string &replace(const_iterator __i1,
                          const_iterator __i2,
                          iterator __k1,
                          iterator __k2)
    {
        return replace(__i1 - begin(),
                       __i2 - __i1,
                       __k1.base(),
                       __k2 - __k1);
    }

    basic_string &replace(const_iterator __i1,
                          const_iterator __i2,
                          const_iterator __k1,
                          const_iterator __k2)
    {
        return replace(__i1 - begin(),
                       __i2 - __i1,
                       __k1.base(),
                       __k2 - __k1);
    }

    basic_string &replace(const_iterator __i1,
                          const_iterator __i2,
                          initializer_list<_CharType> __l)
    {
        return replace(__i1, __i2, __l.begin(), __l.end());
    }

    //==========================================================================
    // copy
    //==========================================================================

    size_type copy(_CharType *__s, size_type __n, size_type __pos = 0) const;

    //==========================================================================
    // swap
    //==========================================================================

    void swap(basic_string &__s) noexcept;

    //==========================================================================
    // c_str
    //==========================================================================

    const _CharType *c_str() const noexcept
    {
        return _data();
    }

    //==========================================================================
    // data
    //==========================================================================

    _CharType *data() noexcept
    {
        return _data();
    }

    const _CharType *data() const noexcept
    {
        return _data();
    }

    //==========================================================================
    // get_allocator
    //==========================================================================

    allocator_type get_allocator() const noexcept
    {
        return _get_allocator();
    }

    //==========================================================================
    // find
    //==========================================================================

    size_type find(const _CharType *__s, size_type __pos, size_type __n) const noexcept;

    size_type find(const basic_string &__str, size_type __pos = 0) const noexcept
    {
        return find(__str.data(), __pos, __str.size());
    }

    size_type find(const _CharType *__s, size_type __pos = 0) const noexcept
    {
        return find(__s, __pos, traits_type::length(__s));
    }

    size_type find(_CharType __c, size_type __pos = 0) const noexcept;

    //==========================================================================
    // rfind
    //==========================================================================

    size_type rfind(const _CharType *__s, size_type __pos, size_type __n) const noexcept;

    size_type rfind(const basic_string &__str, size_type __pos = npos) const noexcept
    {
        return rfind(__str.data(), __pos, __str.size());
    }

    size_type rfind(const _CharType *__s, size_type __pos = npos) const noexcept
    {
        return rfind(__s, __pos, traits_type::length(__s));
    }

    size_type rfind(_CharType __c, size_type __pos = npos) const noexcept;

    //==========================================================================
    // find_first_of
    //==========================================================================

    size_type find_first_of(const basic_string &__str, size_type __pos = 0) const noexcept
    {
        return find_first_of(__str.data(), __pos, __str.size());
    }

    size_type find_first_of(const _CharType *__s, size_type __pos, size_type __n) const noexcept;

    size_type find_first_of(const _CharType *__s, size_type __pos = 0) const noexcept
    {
        return find_first_of(__s, __pos, traits_type::length(__s));
    }

    size_type find_first_of(_CharType __c, size_type __pos = 0) const noexcept
    {
        return find(__c, __pos);
    }

    //==========================================================================
    // find_last_of
    //==========================================================================

    size_type find_last_of(const basic_string &__str, size_type __pos = npos) const noexcept
    {
        return find_last_of(__str.data(), __pos, __str.size());
    }

    size_type find_last_of(const _CharType *__s, size_type __pos, size_type __n) const noexcept;

    size_type find_last_of(const _CharType *__s, size_type __pos = 0) const noexcept
    {
        return find_last_of(__s, __pos, traits_type::length(__s));
    }

    size_type find_last_of(_CharType __c, size_type __pos = 0) const noexcept
    {
        return rfind(__c, __pos);
    }

    //==========================================================================
    // find_first_not_of
    //==========================================================================

    size_type find_first_not_of(const basic_string &__str, size_type __pos = 0) const noexcept
    {
        return find_first_not_of(__str.data(), __pos, __str.size());
    }

    size_type find_first_not_of(const _CharType *__s, size_type __pos, size_type __n) const noexcept;

    size_type find_first_not_of(const _CharType *__s, size_type __pos = 0) const noexcept
    {
        return find_first_not_of(__s, __pos, traits_type::length(__s));
    }

    size_type find_first_not_of(_CharType __c, size_type __pos = 0) const noexcept;

    //==========================================================================
    // find_last_not_of
    //==========================================================================

    size_type find_last_not_of(const basic_string &__str, size_type __pos = npos) const noexcept
    {
        return find_last_not_of(__str.data(), __pos, __str.size());
    }

    size_type find_last_not_of(const _CharType *__s, size_type __pos, size_type __n) const noexcept;

    size_type find_last_not_of(const _CharType *__s, size_type __pos = npos) const noexcept
    {
        return find_last_not_of(__s, __pos, traits_type::length(__s));
    }

    size_type find_last_not_of(_CharType __c, size_type __pos = npos) const noexcept;

    //==========================================================================
    // substr
    //==========================================================================

    basic_string substr(size_type __pos = 0, size_type __n = npos) const
    {
        return basic_string(*this, _check(__pos, ""), __n);
    }

    //==========================================================================
    // compare
    //==========================================================================
};

template <typename _CharType, typename _Traits, typename _Alloc>
const typename basic_string<_CharType, _Traits, _Alloc>::size_type basic_string<_CharType, _Traits, _Alloc>::npos;

//==========================================================================
// Private member function implementations
//==========================================================================

template <typename _CharType, typename _Traits, typename _Alloc>
typename basic_string<_CharType, _Traits, _Alloc>::pointer
basic_string<_CharType, _Traits, _Alloc>::_create(size_type &__capacity, size_type __old_capacity)
{
    if (__capacity > max_size())
    {
        // TODO: Handle this properly
        throw 1;
    }

    if (__capacity > __old_capacity && __capacity < 2 * __old_capacity)
    {
        __capacity = 2 * __old_capacity;

        if (__capacity > max_size())
        {
            __capacity = max_size();
        }
    }

    return _alloc_traits::allocate(_get_allocator(), __capacity + 1);
}

template <typename _CharType, typename _Traits, typename _Alloc>
template <typename _InIterator>
void basic_string<_CharType, _Traits, _Alloc>::_construct(_InIterator __beg, _InIterator __end, std::input_iterator_tag)
{
    size_type __len = 0;
    size_type __capacity = size_type(_local_capacity);

    while (__beg != __end && __len < __capacity)
    {
        _data()[__len++] = *__beg;
        ++__beg;
    }

    try
    {
        while (__beg != __end)
        {
            if (__len == __capacity)
            {
                __capacity = __len + 1;
                pointer __another = _create(__capacity, __len);
                _copy(__another, _data(), __len);
                _dispose();
                _data(__another);
                _capacity(__capacity);
            }
            _data()[__len++] = *__beg;
            ++__beg;
        }
    }
    catch (...)
    {
        _dispose();
        throw;
    }

    _set_length(__len);
}

template <typename _CharType, typename _Traits, typename _Alloc>
template <typename _InIterator>
void basic_string<_CharType, _Traits, _Alloc>::_construct(_InIterator __beg, _InIterator __end, std::forward_iterator_tag)
{
    size_type __dnew = static_cast<size_type>(std::distance(__beg, __end));

    if (__dnew > size_type(_local_capacity))
    {
        _data(_create(__dnew, size_type(0)));
        _capacity(__dnew);
    }

    try
    {
        _copy_chars(_data(), __beg, __end);
    }
    catch (...)
    {
        _dispose();
        throw;
    }

    _set_length(__dnew);
}

template <typename _CharType, typename _Traits, typename _Alloc>
void basic_string<_CharType, _Traits, _Alloc>::_construct(size_type __n, _CharType __c)
{
    if (__n > size_type(_local_capacity))
    {
        _data(_create(__n, size_type(0)));
        _capacity(__n);
    }

    if (__n)
    {
        _assign(_data(), __n, __c);
    }

    _set_length(__n);
}

template <typename _CharType, typename _Traits, typename _Alloc>
void basic_string<_CharType, _Traits, _Alloc>::_assign(const basic_string &__str)
{
    if (this == &__str)
    {
        return;
    }

    const size_type __rsize = __str.length();
    const size_type __capacity = capacity();

    if (__rsize > __capacity)
    {
        size_type __new_capacity = __rsize;
        pointer __tmp = _create(__new_capacity, __capacity);

        _dispose();
        _data(__tmp);
        _capacity(__new_capacity);
    }

    if (__rsize)
    {
        _copy(_data(), __str._data(), __rsize);
    }

    _set_length(__rsize);
}

template <typename _CharType, typename _Traits, typename _Alloc>
void basic_string<_CharType, _Traits, _Alloc>::_mutate(size_type __pos, size_type __len1, const _CharType *__s, size_type __len2)
{
    const size_type __how_much = length() - __pos - __len1;

    size_type __new_capacity = length() + __len2 - __len1;
    pointer __r = _create(__new_capacity, capacity());

    if (__pos)
    {
        _copy(__r, _data(), __pos);
    }

    if (__s && __len2)
    {
        _copy(__r + __pos, __s, __len2);
    }

    if (__how_much)
    {
        _copy(__r + __pos + __len2, _data() + __pos + __len1, __how_much);
    }

    _dispose();
    _data(__r);
    _capacity(__new_capacity);
}

template <typename _CharType, typename _Traits, typename _Alloc>
void basic_string<_CharType, _Traits, _Alloc>::_erase(size_type __pos, size_type __n)
{
    const size_type __how_much = length() - __pos - __n;

    if (__how_much && __n)
    {
        _move(_data() + __pos, _data() + __pos + __n, __how_much);
    }

    _set_length(length() - __n);
}

template <typename _CharType, typename _Traits, typename _Alloc>
basic_string<_CharType, _Traits, _Alloc> &
basic_string<_CharType, _Traits, _Alloc>::_append(const _CharType *__s, size_type __n)
{
    const size_type __len = __n + size();

    if (__len <= capacity())
    {
        if (__n)
        {
            _copy(_data() + size(), __s, __n);
        }
    }
    else
    {
        _mutate(size(), size_type(0), __s, __n);
    }

    _set_length(__len);

    return *this;
}

template <typename _CharType, typename _Traits, typename _Alloc>
template <typename _InputIterator>
basic_string<_CharType, _Traits, _Alloc> &
basic_string<_CharType, _Traits, _Alloc>::_replace_dispatch(const_iterator __i1,
                                                            const_iterator __i2,
                                                            _InputIterator __k1,
                                                            _InputIterator __k2,
                                                            std::__false_type)
{
    const basic_string __s(__k1, __k2, get_allocator());
    const size_type __n1 = __i2 - __i1;

    return _replace(__i1 - begin(), __n1, __s._data(), __s.size());
}

template <typename _CharType, typename _Traits, typename _Alloc>
basic_string<_CharType, _Traits, _Alloc> &
basic_string<_CharType, _Traits, _Alloc>::_replace_aux(size_type __pos1,
                                                       size_type __n1,
                                                       size_type __n2,
                                                       _CharType __c)
{
    _check_length(__n1, __n2, "");

    const size_type __old_size = size();
    const size_type __new_size = __old_size + __n2 - __n1;

    if (__new_size <= capacity())
    {
        pointer __p = _data() + __pos1;

        const size_type __how_much = __old_size - __pos1 - __n1;

        if (__how_much && __n1 != __n2)
        {
            _move(__p + __n2, __p + __n1, __how_much);
        }
    }
    else
    {
        _mutate(__pos1, __n1, 0, __n2);
    }

    if (__n2)
    {
        _assign(_data() + __pos1, __n2, __c);
    }

    _set_length(__new_size);

    return *this;
}

template <typename _CharType, typename _Traits, typename _Alloc>
basic_string<_CharType, _Traits, _Alloc> &
basic_string<_CharType, _Traits, _Alloc>::_replace(size_type __pos,
                                                   size_type __len1,
                                                   const _CharType *__s,
                                                   const size_type __len2)
{
    _check_length(__len1, __len2, "");

    const size_type __old_size = size();
    const size_type __new_size = __old_size + __len2 - __len1;

    if (__new_size <= capacity())
    {
        pointer __p = data() + __pos;

        const size_type __how_much = __old_size - __pos - __len1;

        if (_disjunct(__s))
        {
            if (__how_much && __len1 != __len2)
            {
                _move(__p + __len2, __p + __len1, __how_much);
            }

            if (__len2)
            {
                _copy(__p, __s, __len2);
            }
        }
        else
        {
            if (__len2 && __len2 <= __len1)
            {
                _move(__p, __s, __len2);
            }

            if (__how_much && __len1 != __len2)
            {
                _move(__p + __len2, __p + __len1, __how_much);
            }

            if (__len2 > __len1)
            {
                if (__s + __len2 <= __p + __len1)
                {
                    _move(__p, __s, __len2);
                }
                else if (__s >= __p + __len1)
                {
                    _copy(__p, __s + __len2 - __len1, __len2);
                }
                else
                {
                    const size_type __nleft = (__p + __len1) - __s;
                    _move(__p, __s, __nleft);
                    _copy(__p + __nleft, __p + __len2, __len2 - __nleft);
                }
            }
        }
    }
    else
    {
        _mutate(__pos, __len1, __s, __len2);
    }

    _set_length(__new_size);

    return *this;
}

//==========================================================================
// Public member function implementations
//==========================================================================

template <typename _CharType, typename _Traits, typename _Alloc>
void basic_string<_CharType, _Traits, _Alloc>::swap(basic_string &__s) noexcept
{
    if (this == &__s)
    {
        return;
    }

    _alloc_traits::_on_swap(_get_allocator(), __s._get_allocator());

    if (_is_local())
    {
        if (__s._is_local())
        {
            if (length() && __s.length())
            {
                _CharType __tmp_data[_local_capacity + 1];
                traits_type::copy(__tmp_data, __s._local_buffer, __s._local_capacity + 1);
                traits_type::copy(__s._local_buffer, _local_buffer, __s._local_capacity + 1);
                traits_type::copy(_local_buffer, __tmp_data, __s._local_capacity + 1);
            }
            else if (__s.length())
            {
                traits_type::copy(_local_buffer, __s._local_buffer, _local_capacity + 1);
                _length(__s.length());
                __s._set_length(0);
                return;
            }
            else if (length())
            {
                traits_type::copy(__s._local_buffer, _local_buffer, _local_capacity + 1);
                __s._length(length());
                _set_length(0);
                return;
            }
        }
        else
        {
            const size_type __tmp_capacity = __s._allocated_capacity;
            traits_type::copy(__s._local_buffer, _local_buffer, _local_capacity + 1);
            _data(__s._data());
            __s._data(__s._local_buffer);
            _capacity(__tmp_capacity);
        }
    }
    else
    {
        const size_type __tmp_capacity = _allocated_capacity;

        if (__s._is_local())
        {
            traits_type::copy(_local_buffer, __s._local_buffer, __s._local_capacity + 1);
            __s.data(_data());
            _data(_local_buffer);
        }
        else
        {
            pointer __tmp_ptr = data();
            _data(__s._data());
            __s._data(__tmp_ptr);
            _capacity(_allocated_capacity);
        }

        __s._capacity(__tmp_capacity);
    }

    const size_type __tmp_length = length();
    _length(__s.length());
    __s._set_length(__tmp_length);
}

template <typename _CharType, typename _Traits, typename _Alloc>
void basic_string<_CharType, _Traits, _Alloc>::reserve(size_type __res)
{
    const size_type __capacity = capacity();

    if (__res <= __capacity)
    {
        return;
    }

    pointer __tmp = _create(__res, __capacity);
    _copy(__tmp, _data(), length() + 1);
    _dispose();
    _data(__tmp);
    _capacity(__res);
}

template <typename _CharType, typename _Traits, typename _Alloc>
void basic_string<_CharType, _Traits, _Alloc>::reserve()
{
    if (_is_local())
    {
        return;
    }

    const size_type __length = length();
    const size_type __capacity = _allocated_capacity;

    if (__length <= size_type(_local_capacity))
    {
        _copy(_local_data(), _data(), __length + 1);
        _destroy(__capacity);
        _data(_local_data());
    }
    else if (__length < __capacity)
    {
        try
        {
            pointer __tmp = _alloc_traits::allocate(_get_allocator(), __length + 1);
            _copy(__tmp, _data(), __length + 1);
            _dispose();
            _data(__tmp);
            _capacity(__length);
        }
        catch (...)
        {
            throw;
        }
    }
}

template <typename _CharType, typename _Traits, typename _Alloc>
void basic_string<_CharType, _Traits, _Alloc>::resize(size_type __n, _CharType __c)
{
    const size_type __size = size();
    if (__size < __n)
    {
        append(__n - __size, __c);
    }
    else if (__n < __size)
    {
        _set_length(__n);
    }
}

template <typename _CharType, typename _Traits, typename _Alloc>
typename basic_string<_CharType, _Traits, _Alloc>::size_type
basic_string<_CharType, _Traits, _Alloc>::copy(_CharType *__s, size_type __n, size_type __pos) const
{
    _check(__pos, "");
    __n = _limit(__pos, __n);

    if (__n)
    {
        _copy(__s, _data() + __pos, __n);
    }

    return __n;
}

}  // namespace OS6STD

OS6STD_END_SYSTEM_HEADER

#endif

//==============================================================================
// End of file
//==============================================================================
