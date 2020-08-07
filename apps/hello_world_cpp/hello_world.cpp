/**
 * @file hello_world.cpp
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-06-16
 * 
 * @brief Simple c++ program printing hello world in userspace
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

#include <exception>
#include <stdio.h>

#include <limits>
#include <type_traits>

#include <__test/__test.hpp>

int array[20] = {0};

int main(int argc, char **argv)
{
	run_tests();

	for (int i = 0; i < 20; ++i)
	{
		printf("%i\n", array[i]);
	}

	return 0;
}