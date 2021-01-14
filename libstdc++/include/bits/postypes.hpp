/**
 * @file postypes.hpp
 * @author Joakim Bertils
 * @version 0.1
 * @date 2021-01-10
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

#ifndef _POS_TYPES_HPP
#define _POS_TYPES_HPP

#pragma GCC system_header

#include <bits/c++config.hpp>
#include <cwchar>

OS6STD_BEGIN_SYSTEM_HEADER

namespace OS6STD
{
/**
 * @brief Type used by fpos, char_traits<char> and char_traits<wchar_t>
 * 
 * 
 */
using streamoff = int64_t;
using streamsize = ptrdiff_t;

//=============================================================================
// fpos class interface
//=============================================================================

template <typename _StateT>
class fpos
{
private:
    streamoff _off;
    _StateT _state;

public:
    fpos();
    fpos(streamoff __off);
    fpos(const fpos &) = default;

    ~fpos() = default;

    fpos &operator=(const fpos &) = default;
    operator streamoff() const;
    fpos &operator+=(streamoff __off);
    fpos &operator-=(streamoff __off);
    fpos operator+(streamoff __off) const;
    fpos operator-(streamoff __off) const;

    void state(_StateT __st);
};

//=============================================================================
// fpos class implementation
//=============================================================================

template <typename _StateT>
fpos<_StateT>::fpos()
    : _off(0), _state()
{
}

template <typename _StateT>
fpos<_StateT>::fpos(streamoff __off)
    : _off(__off), _state()
{
}

template <typename _StateT>
fpos<_StateT>::operator streamoff() const
{
    return _off;
}

template <typename _StateT>
fpos<_StateT> &fpos<_StateT>::operator+=(streamoff __off)
{
    _off += __off;

    return *this;
}

template <typename _StateT>
fpos<_StateT> &fpos<_StateT>::operator-=(streamoff __off)
{
    _off -= __off;

    return *this;
}

template <typename _StateT>
fpos<_StateT> fpos<_StateT>::operator+(streamoff __off) const
{
    fpos<_StateT> __pos(*this);

    __pos += __off;

    return __pos;
}

template <typename _StateT>
fpos<_StateT> fpos<_StateT>::operator-(streamoff __off) const
{
    fpos<_StateT> __pos(*this);

    __pos -= __off;

    return __pos;
}

template <typename _StateT>
void fpos<_StateT>::state(_StateT __st)
{
    return _state;
}

//=============================================================================
// fpos class related functions
//=============================================================================

template <typename _StateT>
inline bool operator==(const fpos<_StateT> &__lhs, const fpos<_StateT> &__rhs)
{
    return streamoff(__lhs) == streamoff(__rhs);
}

template <typename _StateT>
inline bool operator!=(const fpos<_StateT> &__lhs, const fpos<_StateT> &__rhs)
{
    return streamoff(__lhs) != streamoff(__rhs);
}

//=============================================================================
// fpos specializations
//=============================================================================

using streampos = fpos<mbstate_t>;
using wstreampos = fpos<mbstate_t>;
using u8streampos = fpos<mbstate_t>;
using u16streampos = fpos<mbstate_t>;
using u32streampos = fpos<mbstate_t>;

}  // namespace OS6STD

OS6STD_END_SYSTEM_HEADER

#endif

//=============================================================================
// End of file
//=============================================================================
