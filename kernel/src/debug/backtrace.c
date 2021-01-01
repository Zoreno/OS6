/**
 * @file backtrace.c
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-06-22
 * 
 * @brief Simple backtrace interface
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

#include <debug/backtrace.h>

#include <stdint.h>
#include <stdio.h>

#include <exec/elf64.h>

void print_backtrace(unsigned long long int *reg)
{
    int depth = 0;

    while (*reg)
    {
        printf("bt [%i]: ", depth);

        int ext = kernel_lookup_symbol((void *)*(reg + 1));

        printf("\n");

        if (ext)
        {
            break;
        }

        reg = (unsigned long long int *)*reg;

        ++depth;
    }
}

void backtrace()
{
#ifndef OS6_DEBUG

    printf("Backtrace not available in release build\n");

#else

    register unsigned long long int *_rbp __asm__("rbp");

    print_backtrace(_rbp);

#endif
}

//=============================================================================
// End of file
//=============================================================================