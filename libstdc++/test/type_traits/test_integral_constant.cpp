/**
 * @file test_integral_constant.cpp
 * @author Joakim Bertils
 * @version 0.1
 * @date 2020-08-13
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

#include <utest.h>

#include <type_traits>

typedef std::integral_constant<int, 1> one;
typedef std::integral_constant<int, 0> zero;
typedef std::integral_constant<int, -1> mone;

typedef std::integral_constant<bool, true> tr;
typedef std::integral_constant<bool, false> fa;

UTEST(type_traits, integral_constant_operator)
{
    ASSERT_TRUE(one{}() == 1);
    ASSERT_TRUE(zero{}() == 0);
    ASSERT_TRUE(mone{}() == -1);
    ASSERT_TRUE(tr{}() == true);
    ASSERT_TRUE(fa{}() == false);
}

UTEST(type_traits, integral_constant_operator2)
{
    ASSERT_TRUE(one() == 1);
    ASSERT_TRUE(zero() == 0);
    ASSERT_TRUE(mone() == -1);
    ASSERT_TRUE(tr() == true);
    ASSERT_TRUE(fa() == false);
}

UTEST(type_traits, integral_constant_static)
{
    const bool *p = &std::integral_constant<bool, true>::value;

    ASSERT_TRUE(p != nullptr);
}

UTEST(type_traits, integral_constant_values)
{
    using std::false_type;
    using std::true_type;

    ASSERT_TRUE(true_type::value == true);
    ASSERT_TRUE(false_type::type::value == false);
    ASSERT_TRUE(true_type::value == true);
    ASSERT_TRUE(false_type::type::value == false);
}
