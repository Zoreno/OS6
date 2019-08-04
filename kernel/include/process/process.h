#ifndef _PROCESS_H
#define _PROCESS_H

#include <stdint.h>

#include <util/tree.h>
#include <util/list.h>
#include <arch/arch.h>

#include <mm/virt_mem.h>

#include <sync/spinlock.h>

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

void delete_process(process_t *process);
void cleanup_process(process_t *process, int retval);
void reap_process(process_t *proc);
void task_exit(int retval);
void tasking_install();
pid_t fork();
void switch_task(uint8_t reschedule);
process_t *get_current_process();
pid_t get_pid();
void process_yield(uint8_t reschedule);
void process_sleep(uint64_t ms);
void process_disown(process_t *process);

#endif