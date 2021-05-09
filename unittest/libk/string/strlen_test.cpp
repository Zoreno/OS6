/**
 * @file strlen_test.cpp
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-12-30
 *
 * @brief
 *
 * @copyright Copyright (C) 2019,
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

#include <gmock/gmock.h>
#include <gtest/gtest.h>

extern "C"
{
#include "../../../libk/src/string/strlen.c"
}

//==============================================================================
// Test fixtures
//==============================================================================

//==============================================================================
// Tests
//==============================================================================

TEST(Strlen, Strlen_ReadsCorrectAmountOfCharacters)
{
    const char *str = "Hello World!";

    size_t length = strlen(str);

    ASSERT_EQ(length, 12);
}

TEST(Strlen, Strlen_ZeroLengthString)
{
    const char *str = "";

    size_t length = strlen(str);

    ASSERT_EQ(length, 0);
}

//==============================================================================
// Main file
//==============================================================================

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}

//==============================================================================
// End of file
//==============================================================================
