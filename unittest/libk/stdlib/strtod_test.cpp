/**
 * @file strtod_test.cpp
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

extern "C"
{
#include "../../../libk/src/stdlib/strtod.c"
}

//==============================================================================
// Test fixtures
//==============================================================================

//==============================================================================
// Tests
//==============================================================================

TEST(Strtod, Strtod_SimpleNumber)
{
    const char *str = "10";

    double d = libk_strtod(str, NULL);

    ASSERT_EQ(d, 10.0);
}

TEST(Strtod, Strtod_SimpleNumberSetsEndpointer)
{
    const char *str = "10";
    char *end;

    double d = libk_strtod(str, &end);

    ASSERT_EQ(d, 10.0);
    ASSERT_EQ(end, str + strlen(str));
}

TEST(Strtod, Strtod_InitialWhitespace)
{
    const char *str = "   10";

    double d = libk_strtod(str, NULL);

    ASSERT_EQ(d, 10.0);
}

TEST(Strtod, Strtod_PlusSign)
{
    const char *str = "+10";

    double d = libk_strtod(str, NULL);

    ASSERT_EQ(d, 10.0);
}

TEST(Strtod, Strtod_MinusSign)
{
    const char *str = "-10";

    double d = libk_strtod(str, NULL);

    ASSERT_EQ(d, -10.0);
}

TEST(Strtod, Strtod_EndingOnNonDigitCharacter)
{
    const char *str = "10!";

    double d = libk_strtod(str, NULL);

    ASSERT_EQ(d, 10.0);
}

TEST(Strtod, Strtod_NotANumber)
{
    const char *str = "asdf";
    char *end;

    double d = libk_strtod(str, &end);

    ASSERT_EQ(d, 0);
    ASSERT_EQ(end, str);
}

TEST(Strtod, Strtod_FractionalPart)
{
    const char *str = "10.23";
    char *end;

    double d = libk_strtod(str, &end);

    ASSERT_EQ(d, 10.23);
    ASSERT_EQ(end, str + strlen(str));
}

TEST(Strtod, Strtod_ExponentialPart)
{
    const char *str = "1e2";
    char *end;

    double d = libk_strtod(str, &end);

    ASSERT_EQ(d, 100);
    ASSERT_EQ(end, str + strlen(str));
}

TEST(Strtod, Strtod_ExponentialPartWithPlus)
{
    const char *str = "1e+2";
    char *end;

    double d = libk_strtod(str, &end);

    ASSERT_EQ(d, 100);
    ASSERT_EQ(end, str + strlen(str));
}

TEST(Strtod, Strtod_ExponentialPartWithMinus)
{
    const char *str = "1e-1";
    char *end;

    double d = libk_strtod(str, &end);

    ASSERT_EQ(d, 0.1);
    ASSERT_EQ(end, str + strlen(str));
}

TEST(Strtod, Strtod_ExponentialPartWithLeadingZeros)
{
    const char *str = "1e01";
    char *end;

    double d = libk_strtod(str, &end);

    ASSERT_EQ(d, 10);
    ASSERT_EQ(end, str + strlen(str));
}

TEST(Strtod, Strtod_ExponentialPartWithMultipleDigits)
{
    const char *str = "1e10";
    char *end;

    double d = libk_strtod(str, &end);

    ASSERT_EQ(d, 1e10);
    ASSERT_EQ(end, str + strlen(str));
}

TEST(Strtod, Strtod_LowNumberShouldEqualZero)
{
    const char *str = "2.2250738585072011e-308";
    char *end;

    double d = libk_strtod(str, &end);

    ASSERT_EQ(d, 0);
    ASSERT_EQ(end, str + strlen(str));
}

TEST(Strtod, Strtod_ExponentialPartMissing)
{
    const char *str = "1e";
    char *end;

    double d = libk_strtod(str, &end);

    ASSERT_EQ(d, 0);
    ASSERT_EQ(end, str);
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
