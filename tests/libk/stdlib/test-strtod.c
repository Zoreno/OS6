/**
 * @file test-strtod.c
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-07-07
 * 
 * @brief Unit test file for the strtod function
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

#include <stdlib.h>

#include "../../../libk/src/stdlib/strtod.c"

#include <jtest.h>

JTEST(strtod, integers)
{
    // Without sign
    ASSERT_DBL_NEAR(1, libk_strtod("1", NULL));
    ASSERT_DBL_NEAR(0, libk_strtod("0", NULL));

    // With sign
    ASSERT_DBL_NEAR(1, libk_strtod("+1", NULL));
    ASSERT_DBL_NEAR(-1, libk_strtod("-1", NULL));
}

JTEST(strtod, fractions)
{
    ASSERT_DBL_NEAR(1.1, libk_strtod("1.1", NULL));
    ASSERT_DBL_NEAR(0.1, libk_strtod("0.1", NULL));

    ASSERT_DBL_NEAR(0.1, libk_strtod(".1", NULL));
    ASSERT_DBL_NEAR(-123.123, libk_strtod("-123.123", NULL));
}

JTEST(strtod, exponents)
{
    ASSERT_DBL_NEAR(1e1, libk_strtod("1e1", NULL));
    ASSERT_DBL_NEAR(1e-2, libk_strtod("1e-2", NULL));
    ASSERT_DBL_NEAR(1e+2, libk_strtod("1e+2", NULL));
    ASSERT_DBL_NEAR(1e-1, libk_strtod("1E-1", NULL));
    ASSERT_DBL_NEAR(1e+1, libk_strtod("1E+1", NULL));
    ASSERT_DBL_NEAR(1e1, libk_strtod("1E+0001", NULL));
}

JTEST(strtod, full)
{
    ASSERT_DBL_NEAR(12.32e1, libk_strtod("+12.32e+0001", NULL));
}

//==============================================================================
// End of file
//==============================================================================