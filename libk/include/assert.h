/**
 * @file assert.h
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-04-25
 * 
 * @brief C standard library assert macro
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

#ifndef _LIBK_ASSERT_H
#define _LIBK_ASSERT_H

#include <stdio.h>

extern void backtrace();
extern void cli();

#ifdef OS6_DEBUG

#define ASSERT(statement)                            \
    if (!statement)                                  \
    {                                                \
        printf("[ASSERT] Failed: %s\n", #statement); \
        backtrace();                                 \
        cli();                                       \
        __asm__ volatile("hlt");                     \
    }

#else

#define ASSERT(statement) ((void)0)

#endif

#endif
