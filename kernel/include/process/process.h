#ifndef _PROCESS_H
#define _PROCESS_H

#include <stdint.h>

#include <util/tree.h>
#include <util/list.h>
#include <arch/arch.h>

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

    uintptr_t stack;

    uintptr_t start;
} image_t;

typedef struct _process
{
    pid_t id;

    char *name;
    char *description;

    char **cmdline;

    thread_t thread;

    tree_node_t *tree_entry;

    image_t image;

    status_t status;

    uint8_t started;
    uint8_t finished;
    uint8_t running;
    uint8_t suspended;

    system_stack_t *regs;

    list_node_t sched_node;

} process_t;

void debug_print_process(process_t *process);
void debug_print_process_tree();

void tasking_install();
pid_t fork();
void switch_task(uint8_t reschedule);
process_t *get_current_process();
pid_t get_pid();
void process_yield(uint8_t reschedule);

#endif