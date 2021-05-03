/**
 * @file syscall.c
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-07-19
 * 
 * @brief System call interface
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

#include <syscall/syscall.h>

#include <logging/logging.h>

int syscall_fork();
int syscall_clone();
int syscall_kill(pid_t process, uint32_t signal);

// int syscall_gettimeofday(struct timeval *tv, void *tz);

typedef int (*syscall_func_t)(uint64_t, ...);

syscall_func_t _syscalls[128] = {0};

void syscall_handler(system_stack_t* stack)
{
    syscall_func_t syscall_func = _syscalls[stack->rax];

    if (!syscall_func)
    {
        printf("[SYSCALL] Invalid system call: %i\n", stack->rax);
        return;
    }

    // TODO: Verify that the correct number of parameters are passed to the
    // functions.
    int retval = syscall_func(stack->rbx, stack->rcx, stack->rdx);

    stack->rax = retval;
}

#define DECLARE_SYSCALL(NAME, FUNC) _syscalls[SYSCALL_##NAME] = (syscall_func_t)syscall_##FUNC

void syscall_install()
{
    log_info("[SYSCALL] Installing syscalls...");

    // Sticks head in the sand...
#pragma GCC diagnostic ignored "-Wcast-function-type"
    DECLARE_SYSCALL(DEBUG_PRINT, debug_print);
    DECLARE_SYSCALL(EXIT, exit);
    DECLARE_SYSCALL(YIELD, yield);
    DECLARE_SYSCALL(SLEEP, sleep);

    DECLARE_SYSCALL(OPEN, open);
    DECLARE_SYSCALL(CLOSE, close);
    DECLARE_SYSCALL(READ, read);
    DECLARE_SYSCALL(WRITE, write);
    DECLARE_SYSCALL(SEEK, seek);
    DECLARE_SYSCALL(READDIR, readdir);
    DECLARE_SYSCALL(ACCESS, access);
    DECLARE_SYSCALL(IOCTL, ioctl);
    DECLARE_SYSCALL(STATF, statf);
    DECLARE_SYSCALL(STAT, stat);
    DECLARE_SYSCALL(MKDIR, mkdir);
    DECLARE_SYSCALL(UNLINK, unlink);
    DECLARE_SYSCALL(SYMLINK, symlink);
    DECLARE_SYSCALL(READLINK, readlink);
    DECLARE_SYSCALL(LSTAT, lstat);

    DECLARE_SYSCALL(GETPID, getpid);
    DECLARE_SYSCALL(GETTID, gettid);

    DECLARE_SYSCALL(CHDIR, chdir);
    DECLARE_SYSCALL(GETCWD, getcwd);

    DECLARE_SYSCALL(GETTIMEOFDAY, gettimeofday);
    DECLARE_SYSCALL(SETTIMEOFDAY, settimeofday);
#pragma GCC diagnostic pop

    set_irq_handler(SYSCALL_INTNO, syscall_handler);

    log_info("[SYSCALL] Done!");
}

//=============================================================================
// End of file
//=============================================================================