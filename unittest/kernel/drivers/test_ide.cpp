/**
 * @file test_ide.cpp
 * @author Joakim Bertils
 * @version 0.1
 * @date 2021-07-03
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

#include <gmock/gmock.h>
#include <gtest/gtest.h>

class MockIO
{
public:
    MOCK_METHOD1(inportb, uint8_t(uint16_t));
};

extern "C"
{
    uint8_t inportb(uint16_t port)
    {
        MockIO mock;

        return mock.inportb(port);
    }
}

extern "C"
{
#include "../../../kernel/src/drivers/ide.c"
}

//==============================================================================
// Test fixtures
//==============================================================================

//==============================================================================
// Tests
//==============================================================================

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
