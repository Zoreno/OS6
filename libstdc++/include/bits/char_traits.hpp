/**
 * @file char_traits.hpp
 * @author Joakim Bertils
 * @version 0.1
 * @date 2020-08-23
 *
 * @brief Defines classes for character-like type traits.
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

#ifndef _CHAR_TRAITS_HPP
#define _CHAR_TRAITS_HPP

#pragma GCC system_header

#include <bits/c++config.hpp>
#include <bits/postypes.hpp>
#include <cwchar>

OS6STD_BEGIN_SYSTEM_HEADER

namespace __priv
{
//=============================================================================
// std::_Char_types
//=============================================================================

namespace detail
{
template <typename _CharType>
struct _Char_types
{
    using int_type = unsigned long;
    using pos_type = std::streampos;
    using off_type = std::streamoff;
    using state_type = std::mbstate_t;
};
}  // namespace detail

//=============================================================================
// std::char_traits<_CharType> interface
//=============================================================================

/**
 * @brief Traits class for basic character and string operations.
 *
 * The char traits class is a traits class template that tries to abstract the
 * basic character and string operations for a given character type so that more
 * generic algorithms can be implemented in terms of the character traits.
 *
 * This class serves as the basis for explicit instantiations for common
 * character types. The user may provide a specialization for any custom
 * character-like type.
 *
 * @tparam _CharType The character type.
 *
 */
template <typename _CharType>
struct char_traits
{
    /**
     * @brief Type representing the character
     * 
     * 
     */
    using char_type = _CharType;

    /**
     * @brief Integer type capable of representing all values for the character
     * type as well as EOF.
     *
     *
     */
    using int_type = typename detail::_Char_types<_CharType>::int_type;

    /**
     * @brief Integer representing a position in a stream.
     * 
     * 
     */
    using pos_type = typename detail::_Char_types<_CharType>::pos_type;

    /**
     * @brief Integer representing an offset in a stream.
     * 
     * 
     */
    using off_type = typename detail::_Char_types<_CharType>::off_type;

    /**
     * @brief Multibyte conversion state type.
     * 
     * 
     */
    using state_type = typename detail::_Char_types<_CharType>::state_type;

    /**
     * @brief Assign a character
     * 
     * @param __c1 Character to assign to.
     * @param __c2 Character value to assign.
     * 
     */
    static void assign(char_type &__c1, const char_type &__c2);

    /**
     * @brief Assigns the character @a __a to each character in @a __n in the
     * sequence @a __s.
     *
     * @param __s The character sequence to assign to.
     * @param __n The number of characters to assign.
     * @param __a The character to assign.
     *
     * @return Pointer to beginning of the assigned sequence @a __p.
     */
    static char_type *assign(char_type *__s, std::size_t __n, char_type __a);

    /**
     * @brief Compares two characters.
     * 
     * @param __c1 The first character to compare.
     * @param __c2 The second character to compare.
     * 
     * @return true iff @a __c1 and @a __c2 are equal.
     */
    static bool eq(const char_type &__c1, const char_type &__c2);

    /**
     * @brief Compares two characters.
     * 
     * @param __c1 The first character to compare.
     * @param __c2 The second character to compare.
     * 
     * @return true iff @a __c1 is less than @a __c2.
     */
    static bool lt(const char_type &__c1, const char_type &__c2);

    /**
     * @brief Compares the first @a __n characters of the character strings 
     * @a __s1 and @a __s2.
     *
     * @param __s1 The first string to compare.
     * @param __s2 The second string to compare.
     * @param __n The number of characters to compare.
     *
     * @return < 0 if @a __s1 < @a __s2.
     * @return 0 if @a __s1 == @a __s2.
     * @return > 0 if @a __s1 > @a __s2.
     */
    static int compare(const char_type *__s1, const char_type *__s2, std::size_t __n);

    /**
     * @brief Returns the length of the character sequence pointed to by @a __p.
     * 
     * @param __p Pointer to character sequence.
     * 
     * @return Length of string in number of characters.
     */
    static std::size_t length(const char_type *__p);

    /**
     * @brief Finds a character in a character sequence.
     * 
     * @param __s Pointer to character sequence.
     * @param __n Number of characters to search.
     * @param __a Character to find.
     * 
     * @return Pointer to first character equal to @a __a if found, else nullptr.
     */
    static const char_type *find(const char_type *__s, std::size_t __n, const char_type &__a);

    /**
     * @brief Moves a character sequence to another.
     * 
     * @param __s1 Pointer to destination sequence.
     * @param __s2 Pointer to source sequence.
     * @param __n Number of characters to move.
     * 
     * @return Pointer to beginning of destination sequence.
     */
    static char_type *move(char_type *__s1, const char_type *__s2, std::size_t __n);

    /**
     * @brief Copies a character sequence to another.
     * 
     * @param __s1 Pointer to destination sequence.
     * @param __s2 Pointer to source sequence.
     * @param __n Number of characters to move.
     * 
     * @return Pointer to beginning of destination sequence.
     */
    static char_type *copy(char_type *__s1, const char_type *__s2, std::size_t __n);

    /**
     * @brief Converts a value of int_type to char_type.
     * 
     * @param __c int_type value.
     * 
     * @return char_type value.
     */
    static char_type to_char_type(const int_type &__c);

    /**
     * @brief Converts a value of char_type to int_type.
     * 
     * @param __c char_type value.
     * 
     * @return int_type value.
     */
    static int_type to_int_type(const char_type &__c);

    /**
     * @brief Compares int_type values.
     *
     * Note that EOF values are always equal, irregardless of how they are
     * represented.
     *
     * @param __c1 First int_type value.
     * @param __c2 Second int_type value.
     *
     * @return True if @a __c1 == @a __c2.
     */
    static bool eq_int_type(const int_type &__c1, const int_type &__c2);

    /**
     * @brief Returns a value not equivalent to any valid value of type
     * char_type.
     *
     *
     * @return A value representing EOF
     */
    static constexpr int_type eof() noexcept;

    /**
     * @brief Checks if @a __c is EOF.
     * 
     * @param __c Char to check.
     * 
     * @return @a __c if EOF, otherwise some other non-EOF char.
     */
    static constexpr int_type not_eof(const int_type &__c);
};

//=============================================================================
// std::char_traits<_CharType> implementation
//=============================================================================

template <typename _CharType>
void char_traits<_CharType>::assign(char_type &__c1, const char_type &__c2)
{
    __c1 = __c2;
}

template <typename _CharType>
typename char_traits<_CharType>::char_type *char_traits<_CharType>::assign(char_type *__s, std::size_t __n, char_type __a)
{
    std::fill_n(__s, __n, __a);

    return __s;
}

template <typename _CharType>
bool char_traits<_CharType>::eq(const char_type &__c1, const char_type &__c2)
{
    return __c1 == __c2;
}

template <typename _CharType>
bool char_traits<_CharType>::lt(const char_type &__c1, const char_type &__c2)
{
    return __c1 < __c2;
}

template <typename _CharType>
int char_traits<_CharType>::compare(
    const char_type *__s1,
    const char_type *__s2,
    std::size_t __n)
{
    for (std::size_t __i = 0; __i < __n; ++__i)
    {
        if (lt(__s1[__i], __s2[__i]))
        {
            return -1;
        }
        else if (lt(__s2[__i], __s1[__i]))
        {
            return 1;
        }
    }

    return 0;
}

template <typename _CharType>
std::size_t char_traits<_CharType>::length(const char_type *__p)
{
    std::size_t __i = 0;

    // Compare with the type's equivalent of a 0
    while (!eq(__p[__i], char_type()))
    {
        ++__i;
    }

    return __i;
}

template <typename _CharType>
const typename char_traits<_CharType>::char_type *char_traits<_CharType>::find(const char_type *__s, std::size_t __n, const char_type &__a)
{
    for (std::size_t __i = 0; __i < __n; ++__i)
    {
        if (eq(__s[__i], __a))
        {
            return __s + __i;
        }
    }

    return nullptr;
}

template <typename _CharType>
typename char_traits<_CharType>::char_type *char_traits<_CharType>::move(char_type *__s1, const char_type *__s2, std::size_t __n)
{
    if (__n == 0)
    {
        return __s1;
    }

    return static_cast<_CharType *>(__builtin_memmove(__s1, __s2, __n * sizeof(char_type)));
}

template <typename _CharType>
typename char_traits<_CharType>::char_type *char_traits<_CharType>::copy(char_type *__s1, const char_type *__s2, std::size_t __n)
{
    std::copy(__s2, __s2 + __n, __s1);

    return __s1;
}

template <typename _CharType>
typename char_traits<_CharType>::char_type char_traits<_CharType>::to_char_type(const int_type &__c)
{
    return static_cast<char_type>(__c);
}

template <typename _CharType>
typename char_traits<_CharType>::int_type char_traits<_CharType>::to_int_type(const char_type &__c)
{
    return static_cast<int_type>(__c);
}

template <typename _CharType>
bool char_traits<_CharType>::eq_int_type(const int_type &__c1, const int_type &__c2)
{
    return __c1 == __c2;
}

template <typename _CharType>
constexpr typename char_traits<_CharType>::int_type char_traits<_CharType>::eof() noexcept
{
    return static_cast<int_type>(-1);
}

template <typename _CharType>
constexpr typename char_traits<_CharType>::int_type char_traits<_CharType>::not_eof(const int_type &__c)
{
    return !eq_int_type(__c, eof()) ? __c : to_int_type(char_type());
}

}  // namespace __priv

//=============================================================================
// std namespace
//=============================================================================

namespace OS6STD
{
//=============================================================================
// std::char_traits<typename _CharType>
//=============================================================================

/**
 * @brief Simple overload of the char_traits base class.
 * 
 * @tparam _CharType Char type.
 * 
 */
template <typename _CharType>
struct char_traits : public __priv::char_traits<_CharType>
{
};

//=============================================================================
// std::char_traits<char> interface
//=============================================================================

template <>
struct char_traits<char>
{
    /**
     * @brief Type representing the character
     * 
     * 
     */
    using char_type = char;

    /**
     * @brief Integer type capable of representing all values for the character
     * type as well as EOF.
     *
     *
     */
    using int_type = int;

    /**
     * @brief Integer representing a position in a stream.
     * 
     * 
     */
    using pos_type = streampos;

    /**
     * @brief Integer representing an offset in a stream.
     * 
     * 
     */
    using off_type = streamoff;

    /**
     * @brief Multibyte conversion state type.
     * 
     * 
     */
    using state_type = mbstate_t;

    /**
     * @brief Assign a character
     * 
     * @param __c1 Character to assign to.
     * @param __c2 Character value to assign.
     * 
     */
    static void assign(char_type &__c1, const char_type &__c2);

    /**
     * @brief Assigns the character @a __a to each character in @a __n in the
     * sequence @a __s.
     *
     * @param __s The character sequence to assign to.
     * @param __n The number of characters to assign.
     * @param __a The character to assign.
     *
     * @return Pointer to beginning of the assigned sequence @a __p.
     */
    static char_type *assign(char_type *__s, std::size_t __n, char_type __a);

    /**
     * @brief Compares two characters.
     * 
     * @param __c1 The first character to compare.
     * @param __c2 The second character to compare.
     * 
     * @return true iff @a __c1 and @a __c2 are equal.
     */
    static bool eq(const char_type &__c1, const char_type &__c2);

    /**
     * @brief Compares two characters.
     * 
     * @param __c1 The first character to compare.
     * @param __c2 The second character to compare.
     * 
     * @return true iff @a __c1 is less than @a __c2.
     */
    static bool lt(const char_type &__c1, const char_type &__c2);

    /**
     * @brief Compares the first @a __n characters of the character strings 
     * @a __s1 and @a __s2.
     *
     * @param __s1 The first string to compare.
     * @param __s2 The second string to compare.
     * @param __n The number of characters to compare.
     *
     * @return < 0 if @a __s1 < @a __s2.
     * @return 0 if @a __s1 == @a __s2.
     * @return > 0 if @a __s1 > @a __s2.
     */
    static int compare(const char_type *__s1, const char_type *__s2, std::size_t __n);

    /**
     * @brief Returns the length of the character sequence pointed to by @a __p.
     * 
     * @param __p Pointer to character sequence.
     * 
     * @return Length of string in number of characters.
     */
    static std::size_t length(const char_type *__p);

    /**
     * @brief Finds a character in a character sequence.
     * 
     * @param __s Pointer to character sequence.
     * @param __n Number of characters to search.
     * @param __a Character to find.
     * 
     * @return Pointer to first character equal to @a __a if found, else nullptr.
     */
    static const char_type *find(const char_type *__s, std::size_t __n, const char_type &__a);

    /**
     * @brief Moves a character sequence to another.
     * 
     * @param __s1 Pointer to destination sequence.
     * @param __s2 Pointer to source sequence.
     * @param __n Number of characters to move.
     * 
     * @return Pointer to beginning of destination sequence.
     */
    static char_type *move(char_type *__s1, const char_type *__s2, std::size_t __n);

    /**
     * @brief Copies a character sequence to another.
     * 
     * @param __s1 Pointer to destination sequence.
     * @param __s2 Pointer to source sequence.
     * @param __n Number of characters to move.
     * 
     * @return Pointer to beginning of destination sequence.
     */
    static char_type *copy(char_type *__s1, const char_type *__s2, std::size_t __n);

    /**
     * @brief Converts a value of int_type to char_type.
     * 
     * @param __c int_type value.
     * 
     * @return char_type value.
     */
    static char_type to_char_type(const int_type &__c);

    /**
     * @brief Converts a value of char_type to int_type.
     * 
     * @param __c char_type value.
     * 
     * @return int_type value.
     */
    static int_type to_int_type(const char_type &__c);

    /**
     * @brief Compares int_type values.
     *
     * Note that EOF values are always equal, irregardless of how they are
     * represented.
     *
     * @param __c1 First int_type value.
     * @param __c2 Second int_type value.
     *
     * @return True if @a __c1 == @a __c2.
     */
    static bool eq_int_type(const int_type &__c1, const int_type &__c2);

    /**
     * @brief Returns a value not equivalent to any valid value of type
     * char_type.
     *
     *
     * @return A value representing EOF
     */
    static constexpr int_type eof() noexcept;

    /**
     * @brief Checks if @a __c is EOF.
     * 
     * @param __c Char to check.
     * 
     * @return @a __c if EOF, otherwise some other non-EOF char.
     */
    static constexpr int_type not_eof(const int_type &__c);
};

//=============================================================================
// std::char_traits<char> implementation
//=============================================================================

void char_traits<char>::assign(char_type &__c1, const char_type &__c2)
{
    __c1 = __c2;
}

typename char_traits<char>::char_type *char_traits<char>::assign(char_type *__s, std::size_t __n, char_type __a)
{
    if (__n == 0)
    {
        return __s;
    }

    return static_cast<char_type *>(__builtin_memset(__s, __a, __n));
}

bool char_traits<char>::eq(const char_type &__c1, const char_type &__c2)
{
    return __c1 == __c2;
}

bool char_traits<char>::lt(const char_type &__c1, const char_type &__c2)
{
    return (static_cast<unsigned char>(__c1) < static_cast<unsigned char>(__c2));
}

int char_traits<char>::compare(const char_type *__s1, const char_type *__s2, std::size_t __n)
{
    if (__n == 0)
    {
        return 0;
    }

    return __builtin_memcmp(__s1, __s2, __n);
}

std::size_t char_traits<char>::length(const char_type *__p)
{
    return __builtin_strlen(__p);
}

const typename char_traits<char>::char_type *char_traits<char>::find(const char_type *__s, std::size_t __n, const char_type &__a)
{
    if (__n == 0)
    {
        return 0;
    }

    return static_cast<const char_type *>(__builtin_memchr(__s, __a, __n));
}

typename char_traits<char>::char_type *char_traits<char>::move(char_type *__s1, const char_type *__s2, std::size_t __n)
{
    if (__n == 0)
    {
        return __s1;
    }

    return static_cast<char_type *>(__builtin_memmove(__s1, __s2, __n));
}

typename char_traits<char>::char_type *char_traits<char>::copy(char_type *__s1, const char_type *__s2, std::size_t __n)
{
    if (__n == 0)
    {
        return __s1;
    }

    return static_cast<char_type *>(__builtin_memcpy(__s1, __s2, __n));
}

typename char_traits<char>::char_type char_traits<char>::to_char_type(const int_type &__c)
{
    return static_cast<char_type>(__c);
}

typename char_traits<char>::int_type char_traits<char>::to_int_type(const char_type &__c)
{
    return static_cast<int_type>(__c);
}

bool char_traits<char>::eq_int_type(const int_type &__c1, const int_type &__c2)
{
    return __c1 == __c2;
}

constexpr typename char_traits<char>::int_type char_traits<char>::eof() noexcept
{
    return static_cast<int_type>(-1);
}

constexpr typename char_traits<char>::int_type char_traits<char>::not_eof(const int_type &__c)
{
    return (__c == eof()) ? 0 : __c;
}

}  // namespace OS6STD

OS6STD_END_SYSTEM_HEADER

#endif

//=============================================================================
// End of file
//=============================================================================
