/**
 * @file memset_test.cpp
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

#include <gtest/gtest.h>
#include <gmock/gmock.h>

extern "C" 
{
#include "../../../libk/src/string/memset.c"
}

//==============================================================================
// Test fixtures
//==============================================================================

//==============================================================================
// Tests
//==============================================================================

TEST(Memset, Memset_SetsAllElements) 
{ 
    char array[3];

    void *ret = libk_memset(array, 2, sizeof(array));

    ASSERT_THAT(array, ::testing::ElementsAre(2, 2, 2));
    ASSERT_EQ(array, ret);
}

TEST(Memset, Memset_OnlySetsElementsInRange) 
{ 
    char array[3] = {0,0,0};

    void *ret = libk_memset(array, 2, sizeof(array) - 1);

    ASSERT_THAT(array, ::testing::ElementsAre(2, 2, 0));
    ASSERT_EQ(array, ret);
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