/**
 * @file char_traits.hpp
 * @author Joakim Bertils
 * @version 0.1
 * @date 2020-08-23
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

#ifndef _CHAR_TRAITS_HPP
#define _CHAR_TRAITS_HPP

#pragma GCC system_header

#include <bits/c++config.hpp>

OS6STD_BEGIN_SYSTEM_HEADER

namespace OS6STD
{
template <typename _CharType>
struct _Char_types
{
    using int_type = unsigned long;
    // TODO: Add pos_type, off_type and state_type
};

template <typename _CharType>
struct char_traits
{
    using char_type = _CharType;
    using int_type = typename _Char_types<_Char_types>::int_type;
    // TODO: Add pos_type, off_type and state_type

    static void assign(char_type &__c1, const char_type &__c2)
    {
        __c1 = __c2;
    }

    static bool eq(const char_type &__c1, const char_type &__c2)
    {
        return __c1 == __c2;
    }

    static bool lt(const char_type &__c1, const char_type &__c2)
    {
        return __c1 < __c2;
    }

    static int compare(const char_type *__s1,
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

    static std::size_t length(const char_type *__s);
    {
        std::size_t __i = 0;

        // Compare with the type's equivalent of a 0
        while (!eq(__p[__i], char_type()))
        {
            ++__i;
        }

        return __i;
    }
};

}  // namespace OS6STD

OS6STD_END_SYSTEM_HEADER

#endif
