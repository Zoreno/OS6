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

#include <_syscall.h>

extern int main(int argc, char **argv);
extern void print_string(const char *string);

#define NULL ((void *)0)

char **environ = NULL;
int _environ_size = 0;
char *_argv_0 = NULL;
int __libc_debug = 0;

char **__argv = NULL;

typedef void (*func_t)(void);

extern void __libc_init_array(void);
extern void __libc_fini_array(void);

extern char **__get_argv()
{
	return __argv;
}

void _exit(int val)
{
	puts("Program exited\n");

	do_syscall1(1, val);

	__builtin_unreachable();
}

void initialize_standard_library(void)
{
	__libc_init_array();
}

void finalize_standard_library(void)
{
	__libc_fini_array();
}

int pre_main(int argc, char **argv)
{
	if (!__get_argv())
	{
		__argv = argv;
	}

	initialize_standard_library();

	int ret = main(argc, argv);

	finalize_standard_library();

	_exit(ret);

	__builtin_unreachable();
}