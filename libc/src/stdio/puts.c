/**
 * @file puts.c
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-07-20
 * 
 * @brief Syscall helpers
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

#include <stdio.h>

#include <_syscall.h>

int puts(const char *string)
{
    return do_syscall1(0, (int64_t)string);
}