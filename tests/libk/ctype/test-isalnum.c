/**
 * @file test-isalnum.c
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-04-27
 * 
 * @brief Unit test file for the isalnum function
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

#include <ctype.h>
#include <stdio.h>

#include "../../../libk/src/ctype/isalnum.c"

#include <jtest.h>

JTEST(isalnum, char)
{
	ASSERT_EQUAL(libk_isalnum('c'), 1);
}

JTEST(isalnum, colon)
{
	ASSERT_EQUAL(libk_isalnum(':'), 0);
}

JTEST(isalnum, number)
{
	ASSERT_EQUAL(libk_isalnum('1'), 1);
}