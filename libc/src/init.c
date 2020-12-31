/**
 * @file init.c
 * @author Joakim Bertils
 * @version 0.1
 * @date 2020-12-31
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

#include <stdint.h>
#include <stdio.h>

extern void (*__preinit_array_start[])(void) __attribute__((weak));
extern void (*__preinit_array_end[])(void) __attribute__((weak));
extern void (*__init_array_start[])(void) __attribute__((weak));
extern void (*__init_array_end[])(void) __attribute__((weak));

void __libc_init_array(void)
{
    size_t count;
    size_t i;

    count = __preinit_array_end - __preinit_array_start;

    printf("Calling %i preinit functions\n", count);

    for (int i = 0; i < count; ++i)
    {
        if (__preinit_array_start[i])
        {
            __preinit_array_start[i]();
        }
    }

    count = __init_array_end - __init_array_start;

    printf("Calling %i init functions\n", count);

    for (int i = 0; i < count; ++i)
    {
        if (__init_array_start[i])
        {
            __init_array_start[i]();
        }
    }
}