/**
 * @file test_is_void.cpp
 * @author Joakim Bertils
 * @version 0.1
 * @date 2020-08-14
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

class ClassType
{
public:
    ClassType() {}

private:
    int a;
};

class IncompleteClassType;

UTEST(type_traits, is_void)
{
    using std::is_void;

    ASSERT_TRUE(std::is_void<void>::value == true);

    ASSERT_TRUE(std::is_void<char>::value == false);
    ASSERT_TRUE(std::is_void<signed char>::value == false);
    ASSERT_TRUE(std::is_void<unsigned char>::value == false);

    ASSERT_TRUE(std::is_void<short>::value == false);
    ASSERT_TRUE(std::is_void<unsigned short>::value == false);
    ASSERT_TRUE(std::is_void<signed short>::value == false);

    ASSERT_TRUE(std::is_void<int>::value == false);
    ASSERT_TRUE(std::is_void<unsigned int>::value == false);
    ASSERT_TRUE(std::is_void<signed int>::value == false);

    ASSERT_TRUE(std::is_void<long>::value == false);
    ASSERT_TRUE(std::is_void<unsigned long>::value == false);
    ASSERT_TRUE(std::is_void<signed long>::value == false);

    ASSERT_TRUE(std::is_void<long long>::value == false);
    ASSERT_TRUE(std::is_void<unsigned long long int>::value == false);
    ASSERT_TRUE(std::is_void<signed long long int>::value == false);

    ASSERT_TRUE(std::is_void<float>::value == false);
    ASSERT_TRUE(std::is_void<double>::value == false);
    ASSERT_TRUE(std::is_void<long double>::value == false);

    ASSERT_TRUE(std::is_void<ClassType>::value == false);
    ASSERT_TRUE(std::is_void<IncompleteClassType>::value == false);
}
