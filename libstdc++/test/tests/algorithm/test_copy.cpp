/**
 * @file test_copy.cpp
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

#include <utest.h>

#include <algorithm>
#include <iterator>

UTEST(algorithm, copy)
{
    int a[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    int b[10] = {0};

    auto it = std::copy(std::begin(a), std::end(a), std::begin(b));

    for (int i = 0; i < 10; ++i)
 
   {
        ASSERT_TRUE(a[i] == b[i]);
    }
}

UTEST(algorithm, copy_already_identical)
{
    int a[10] = {0};
    int b[10] = {0};

    auto it = std::copy(std::begin(a), std::end(a), std::begin(b));

    for (int i = 0; i < 10; ++i)
    {
        ASSERT_TRUE(a[i] == b[i]);
    }
}

UTEST(algorithm, copy_zero_length)
{
    int a[10] = {0};
    int b[10] = {0};

    auto it = std::copy(std::begin(a), std::begin(a), std::begin(b));

    for (int i = 0; i < 10; ++i)
    {
        ASSERT_TRUE(a[i] == b[i]);
    }
}

UTEST_MAIN();
