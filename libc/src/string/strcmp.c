/**
 * @file strcmp.c
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-06-22
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

#include <string.h>

// TODO: This can be optimized by comparing 4/8 bytes at a time
int strcmp(const char *str1, const char *str2)
{
	int res = 0;

	while (!(res = *(unsigned char *)str1 - *(unsigned char *)str2) && *str2)
	{
		++str1;
		++str2;
	}

	if (res < 0)
	{
		return -1;
	}
	if (res > 0)
	{
		return 1;
	}

	return 0;
}