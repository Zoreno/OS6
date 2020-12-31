/**
 * @file process.h
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

#ifndef _PROCESS_H
#define _PROCESS_H

#include <stdint.h>

#include <arch/arch.h>
#include <mm/virt_mem.h>
#include <sync/spinlock.h>
#include <util/list.h>
#include <util/tree.h>
#include <vfs/vfs.h>

#define MAX_PIDS 256

typedef int32_t pid_t;
typedef long long int user_t;
typedef long long int status_t;

typedef struct _thread
{
    uintptr_t *rsp;
    uintptr_t rip;

    uint8_t fpu_enabled;
    uint8_t fp_regs[512];
} thread_t;

typedef struct _image
{
    size_t size;
    uintptr_t entry;

    uintptr_t heap;
    uintptr_t heap_actual;

    uintptr_t stack;

    uintptr_t start;

    spinlock_t lock;
} image_t;

typedef struct _fd_table
{
    fs_node_t **entries;
    uint64_t *offsets;
    int *modes;
    size_t length;
    size_t capacity;
    size_t refs;
} fd_table_t;

typedef struct _process
{
    pid_t id;

    char *name;
    char *description;

    char **cmdline;
    int argc;

    thread_t thread;

    tree_node_t *tree_entry;

    image_t image;

    fs_node_t *wd_node;
    char *wd_path;

    fd_table_t *file_descriptors;

    status_t status;

    uint8_t started;
    uint8_t finished;
    uint8_t running;
    uint8_t suspended;
    uint8_t sleeping;

    system_stack_t *regs;

    list_node_t sched_node;

    uint64_t sleep_ticks;

    pml4_t *page_directory;

} process_t;

void debug_print_process(process_t *process);
void debug_print_process_tree();

void process_delete(process_t *process);
void process_cleanup(process_t *process, int retval);
void process_reap(process_t *proc);
void process_exit(int retval);
pid_t process_fork();
void process_switch_task(uint8_t reschedule);
process_t *process_get_current();
pid_t process_get_pid();
void process_yield(uint8_t reschedule);
void process_sleep(uint64_t ms);
void process_disown(process_t *process);
int waitpid(int pid, int *status, int options);

size_t process_append_fd(process_t *proc, fs_node_t *node);
size_t process_move_fd(process_t *proc, int src, int dest);

void tasking_install();

#endif

//=============================================================================
// End of file
//=============================================================================