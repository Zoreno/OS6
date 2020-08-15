/**
 * @file new_allocator.hpp
 * @author Joakim Bertils
 * @version 0.1
 * @date 2020-07-29
 * 
 * @brief Base allocator using new and delete operators.
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

#ifndef _NEW_ALLOCATOR_HPP
#define _NEW_ALLOCATOR_HPP

#pragma GCC system_header

#include <bits/c++config.hpp>
#include <bits/move.hpp>

#include <new>
#include <type_traits>

OS6STD_BEGIN_SYSTEM_HEADER

namespace OS6STD
{
using std::ptrdiff_t;
using std::size_t;

template <typename _T>
class new_allocator
{
public:
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using pointer = _T *;
    using const_pointer = const _T *;
    using reference = _T &;
    using const_reference = const _T &;
    using value_type = _T;

    template <typename _U>
    struct rebind
    {
        using other = new_allocator<_U>;
    };

    using propagate_on_container_move_assignment = true_type;

    constexpr new_allocator() noexcept
    {
    }

    constexpr new_allocator(const new_allocator &) noexcept
    {
    }

    template <typename _U>
    constexpr new_allocator(const new_allocator<_U> &) noexcept
    {
    }

    ~new_allocator() noexcept
    {
    }

    pointer address(reference __x) const noexcept
    {
        return std::__addressof(__x);
    }

    const_pointer address(const_reference __x) const noexcept
    {
        return std::__addressof(__x);
    }

    pointer allocate(size_type __n, const void *__p = static_cast<const void *>(0))
    {
        if (__n > this->max_size())
        {
            // TODO: This should throw bad alloc
        }

        // TODO: Handle aligned allocation

        return static_cast<_T *>(::operator new(__n * sizeof(_T)));
    }

    void deallocate(pointer __p, size_type)
    {
        ::operator delete(__p);
    }

    size_type max_size() const noexcept
    {
        return size_t(-1) / sizeof(_T);
    }

    template <typename _U, typename... _Args>
    void construct(_U *__p, _Args &&... __args)
    {
        ::new ((void *)__p) _U(std::forward<_Args>(__args)...);
    }

    template <typename _U>
    void destroy(_U *__p)
    {
        __p->~_U();
    }
};

template <typename _T>
inline bool
operator==(const new_allocator<_T> &, const new_allocator<_T> &)
{
    return true;
}

template <typename _T>
inline bool
operator!=(const new_allocator<_T> &, const new_allocator<_T> &)
{
    return false;
}

}  // namespace OS6STD

OS6STD_END_SYSTEM_HEADER

#endif
