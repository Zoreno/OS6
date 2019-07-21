/**
 * @file _libc_init.c
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-07-20
 * 
 * @brief LIBC initalization
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

extern int main(int argc, char **argv);
extern void print_string(const char *string);

#define NULL ((void *)0)

char **environ = NULL;
int _environ_size = 0;
char *_argv_0 = NULL;
int __libc_debug = 0;

char **__argv = NULL;

typedef void (*func_t)(void);

extern func_t __init_array_start;
extern func_t __init_array_end;
extern func_t __fini_array_start;
extern func_t __fini_array_end;

extern char **__get_argv()
{
    return __argv;
}

void _exit(int val)
{
    puts("Program exited\n");

    // TODO: Replace with syscall_exit
    for (;;)
        ;

    __builtin_unreachable();
}

void initialize_standard_library(void)
{
}

int pre_main(int argc, char **argv)
{
    if (!__get_argv())
    {
        __argv = argv;
    }

    initialize_standard_library();

    int ret = main(argc, argv);

    _exit(ret);
}