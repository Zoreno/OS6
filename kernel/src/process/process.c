/**
 * @file process.c
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

#include <arch/arch.h>
#include <arch/x86-64/fpu.h>
#include <debug/backtrace.h>
#include <exec/elf64.h>
#include <logging/logging.h>
#include <process/process.h>
#include <sync/spinlock.h>
#include <util/bitset.h>
#include <util/hexdump.h>

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG_SCHED 0

#if DEBUG_SCHED
#define PRINT(fmt, ...) log_debug(fmt, ##__VA_ARGS__)
#define LOOKUP_SYMBOL(addr) kernel_lookup_symbol(addr)
#else
#define PRINT(fmt, ...)
#define LOOKUP_SYMBOL(addr)
#endif

#define KERNEL_STACK_SIZE 0x8000

#define PUSH(stack, type, item) \
    stack -= sizeof(type);      \
    *((type *)stack) = item

// ASM functions
extern uintptr_t read_ip();
extern void switch_to(thread_t **thread, thread_t *next);
extern void set_rsp(uint64_t val);
extern uint64_t get_rsp_val();
extern void put_in_rax(uint64_t val);
extern void setup_forked_kthread_stack(uint64_t *val);

//=============================================================================
// Static variables
//=============================================================================

static tree_t *process_tree;
static list_t *process_list;
static list_t *process_ready_queue;
static list_t *process_sleeping_list;

volatile process_t *current_process = NULL;
process_t *kernel_idle_task = NULL;

static bitset_t pid_set;

static spinlock_t tree_lock = {0};
static spinlock_t process_queue_lock = {0};
static spinlock_t process_sleeping_lock = {0};

//=============================================================================
// Forward declarations
//=============================================================================

process_t *spawn_idle_thread();
process_t *spawn_init();
void make_process_ready(process_t *proc);
void wakeup_sleeping_processes();

//=============================================================================
// Debug
//=============================================================================

void debug_print_process(process_t *process)
{
    PRINT(
        "----------------------------------------------------------------------"
        "-----------");
    PRINT("PID: %d, Name: %s, Description: %s",
          process->id,
          process->name,
          process->description);
    PRINT("Cmdline: %s", process->cmdline);
    PRINT(
        "Image: {.entry = %#016x, .size = %#016x, .stack = %#016x, .start = "
        "%#016x}",
        process->image.entry,
        process->image.size,
        process->image.stack,
        process->image.start);
    PRINT(
        "Status: %#08x, started: %d, finished: %d, running: %d, suspended: %d",
        process->status,
        process->started,
        process->finished,
        process->running,
        process->suspended);
    PRINT(
        "----------------------------------------------------------------------"
        "-----------");
}

void debug_print_process_tree_node(tree_node_t *node, size_t height)
{
    if (!node)
    {
        return;
    }

    process_t *proc = (process_t *)node->value;

    for (uint32_t i = 0; i < height; ++i)
    {
        printf("  ");
    }

    printf("%d %s", proc->id, proc->name);

    if (proc->description)
    {
        printf(" %s", proc->description);
    }

    if (proc->finished)
    {
        printf(" [zombie]");
    }

    printf("");

    for (list_node_t *child = node->children->head; child != NULL;
         child = child->next)
    {
        debug_print_process_tree_node(child->payload, height + 1);
    }
}

void debug_print_process_tree()
{
    debug_print_process_tree_node(process_tree->root, 0);
}

//=============================================================================
// PID
//=============================================================================

static int get_next_free_pid()
{
    int index = bitset_ffub(&pid_set);

    if (index != -1)
    {
        bitset_set(&pid_set, index);
    }

    return index;
}

//=============================================================================
// Initialization
//=============================================================================

void initialize_process_tree()
{
    process_tree = tree_create();
    process_list = list_create();
    process_ready_queue = list_create();
    process_sleeping_list = list_create();

    bitset_init(&pid_set, MAX_PIDS);
    bitset_set(&pid_set, 0);
    bitset_set(&pid_set, 1);

    spinlock_init(&tree_lock);
    spinlock_init(&process_queue_lock);
    spinlock_init(&process_sleeping_lock);
}

void tasking_install()
{
    log_info("[DONE] Installing tasking...");

    cli();

    initialize_process_tree();

    current_process = spawn_init();
    kernel_idle_task = spawn_idle_thread();

    current_process->running = 1;

    sti();

    log_info("[PROC] Done!");
}

//=============================================================================
// Idle thread
//=============================================================================

static void kernel_idle(void)
{
    while (1)
    {
        // printf("Idle thread");
        sti();
        __asm__ volatile("hlt");  // TODO: Make this arch-agnostic

        // switch_task(0);
    }
}

process_t *spawn_idle_thread()
{
    log_info("[PROC] Spawning idle thread");

    process_t *idle = malloc(sizeof(process_t));
    memset(idle, 0x00, sizeof(process_t));
    idle->id = -1;
    idle->name = strdup("[kernel idle thread]");

    // Setup the stack
    uint64_t *stack = (uint64_t *)malloc(sizeof(uint64_t) * KERNEL_STACK_SIZE);
    memset(stack, 0, sizeof(uint64_t) * (KERNEL_STACK_SIZE));
    idle->image.stack = (uint64_t)(stack + KERNEL_STACK_SIZE);
    stack[KERNEL_STACK_SIZE - 1] = (uint64_t)&kernel_idle;
    idle->thread.rsp = &(stack[KERNEL_STACK_SIZE - 17]);

    idle->thread.rip = (uintptr_t)&kernel_idle;

    idle->started = 1;
    idle->running = 1;

    idle->page_directory = virt_mem_get_current_dir();

    return idle;
}

//=============================================================================
// Spawn kernel idle process
//=============================================================================

extern uint64_t stack_top;
extern uint64_t stack_bottom;

process_t *spawn_init()
{
    log_info("[PROC] Spawning init thread");

    process_t *init = malloc(sizeof(process_t));

    memset(init, 0, sizeof(process_t));

    tree_set_root(process_tree, (void *)init);

    list_insert(process_list, (void *)init);

    init->tree_entry = process_tree->root;
    init->id = 1;
    init->name = strdup("init");
    init->cmdline = NULL;
    init->argc = 0;
    init->status = 0;

    init->file_descriptors = malloc(sizeof(fd_table_t));

    init->file_descriptors->refs = 1;
    init->file_descriptors->length = 0;
    init->file_descriptors->capacity = 4;
    init->file_descriptors->entries =
        malloc(sizeof(fs_node_t *) * init->file_descriptors->capacity);
    init->file_descriptors->modes =
        malloc(sizeof(int) * init->file_descriptors->capacity);
    init->file_descriptors->offsets =
        malloc(sizeof(uint64_t) * init->file_descriptors->capacity);

    init->wd_node = clone_fs(fs_root);
    init->wd_path = strdup("/");

    init->thread.rip = 0;
    init->thread.rsp = 0;

    init->image.entry = 0;
    init->image.size = 0;
    init->image.heap = 0;
    init->image.heap_actual = 0;
    init->image.stack = (uint64_t)&stack_bottom;
    init->image.start = 0;

    spinlock_init(&init->image.lock);

    init->finished = 0;
    init->suspended = 0;
    init->started = 0;
    init->running = 0;
    init->sleeping = 0;

    init->sched_node.payload = init;

    init->page_directory = virt_mem_get_current_dir();

    return init;
}

//=============================================================================
// Spawn new process
//=============================================================================

process_t *spawn_process(process_t *parent)
{
    log_info("[PROC] Spawning new process");

    ASSERT(process_tree->root);

    process_t *proc = (process_t *)malloc(sizeof(process_t));
    memset(proc, 0, sizeof(process_t));

    proc->id = get_next_free_pid();

    PRINT("Got pid: %d", proc->id);

    // Set this name as a placeholder
    proc->name = strdup("new process");
    proc->description = NULL;
    proc->cmdline = parent->cmdline;
    proc->argc = parent->argc;

    // proc->thread.fpu_enabled = 0;
    // memcpy((void *)proc->thread.fp_regs, (void *)parent->thread.fp_regs,
    // 512);

    uint64_t *stack = (uint64_t *)malloc(sizeof(uint64_t) * KERNEL_STACK_SIZE);

    log_debug("Child stack: %#016x-%#016x", stack, stack + KERNEL_STACK_SIZE);
    log_debug("Parent stack: %#016x-%#016x",
              parent->image.stack,
              parent->image.stack + KERNEL_STACK_SIZE * sizeof(uint64_t));
    log_debug("Parent stack: %#016x-%#016x", &stack_bottom, &stack_top);

    memcpy(stack,
           (void *)parent->image.stack,
           sizeof(uint64_t) * (KERNEL_STACK_SIZE));

    // hexdump(stack, sizeof(uint64_t) * KERNEL_STACK_SIZE);

    proc->image.stack = (uint64_t)(stack);

    stack[KERNEL_STACK_SIZE - 1] = (uint64_t)parent->thread.rip;
    proc->thread.rsp = &(stack[KERNEL_STACK_SIZE - 17]);
    proc->thread.rip = (uintptr_t)parent->thread.rip;

    proc->image.entry = parent->image.entry;
    proc->image.size = parent->image.size;
    proc->image.heap = parent->image.heap;
    proc->image.heap_actual = parent->image.heap_actual;
    proc->image.start = parent->image.start;

    spinlock_init(&proc->image.lock);

    proc->file_descriptors = malloc(sizeof(fd_table_t));
    proc->file_descriptors->refs = 1;
    proc->file_descriptors->length = parent->file_descriptors->length;
    proc->file_descriptors->capacity = parent->file_descriptors->capacity;

    proc->file_descriptors->entries =
        malloc(sizeof(fs_node_t *) * proc->file_descriptors->capacity);
    proc->file_descriptors->modes =
        malloc(sizeof(int) * proc->file_descriptors->capacity);
    proc->file_descriptors->offsets =
        malloc(sizeof(uint64_t) * proc->file_descriptors->capacity);

    for (uint32_t i = 0; i < parent->file_descriptors->length; ++i)
    {
        proc->file_descriptors->entries[i] =
            clone_fs(parent->file_descriptors->entries[i]);
        proc->file_descriptors->modes[i] = parent->file_descriptors->modes[i];
        proc->file_descriptors->offsets[i] =
            parent->file_descriptors->offsets[i];
    }

    proc->wd_node = clone_fs(parent->wd_node);
    proc->wd_path = strdup(parent->wd_path);

    proc->status = 0;
    proc->finished = 0;
    proc->suspended = 0;
    proc->started = 0;
    proc->running = 0;
    proc->sleeping = 0;

    tree_node_t *entry = tree_node_create(proc);

    proc->tree_entry = entry;

    spinlock_lock(&tree_lock);
    tree_node_insert_child_node(process_tree, parent->tree_entry, entry);
    list_insert(process_list, (void *)proc);
    spinlock_unlock(&tree_lock);

    proc->sched_node.payload = proc;

    return proc;
}

//=============================================================================
// Fork
//=============================================================================

pid_t process_fork()
{
    PRINT("Fork");

    cli();

    uintptr_t return_addr = (uintptr_t)__builtin_return_address(0);

    current_process->regs->rax = 0;

    process_t *parent = (process_t *)current_process;

    ASSERT(parent);

    pml4_t *new_page_directory =
        virt_mem_clone_address_space(parent->page_directory);

    parent->thread.rip = return_addr;

    process_t *new_proc = spawn_process(parent);

    ASSERT(new_proc);

    new_proc->page_directory = new_page_directory;

    volatile uintptr_t var = (uintptr_t)(get_rsp_val() + 8);
    parent->thread.rsp = (uint64_t *)(get_rsp_val() + 8);
    volatile uintptr_t diff =
        parent->image.stack + sizeof(uint64_t) * KERNEL_STACK_SIZE - 1 - (var);

    log_debug("Parent thread rsp: %#016x", parent->thread.rsp);
    log_debug("Diff: %#016x", diff);

    new_proc->thread.rsp =
        (uint64_t *)(((uint64_t)new_proc->thread.rsp) - 8 * ((uint64_t)diff));

    log_debug("New thread rsp: %#016x", new_proc->thread.rsp);

    while (*(new_proc->thread.rsp) != return_addr)
    {
        new_proc->thread.rsp++;
    }

    new_proc->thread.rsp--;

    setup_forked_kthread_stack(new_proc->thread.rsp);

    new_proc->thread.rsp -= 15;

    make_process_ready(new_proc);

    sti();

    return new_proc->id;
}

//=============================================================================
// Clone
//=============================================================================

//=============================================================================
// Process cleanup
//=============================================================================

void process_delete(process_t *proc)
{
    tree_node_t *entry = proc->tree_entry;

    if (!entry)
    {
        return;
    }

    free((void *)proc->image.stack);

    // Check if we are trying to kill init
    ASSERT((entry != process_tree->root));

    if (process_tree->root == entry)
    {
        return;
    }

    spinlock_lock(&tree_lock);

    int has_children = entry->children->length;

    // TODO: All the reparented processes should be marked as orphans
    tree_remove_reparent_root(process_tree, entry);

    list_delete(process_list, list_find(process_list, proc));
    spinlock_unlock(&tree_lock);

    // Release our PID
    bitset_clear(&pid_set, proc->id);

    free(proc);
}

void process_cleanup(process_t *proc, int retval)
{
    proc->status = retval;
    proc->finished = 1;

    proc->file_descriptors->refs--;

    if (proc->file_descriptors->refs == 0)
    {
        PRINT("Releasing FDS for process %i", process_get_pid());

        for (size_t i = 0; i < proc->file_descriptors->capacity; ++i)
        {
            if (proc->file_descriptors->entries[i])
            {
                close_fs(proc->file_descriptors->entries[i]);
                proc->file_descriptors->entries[i] = NULL;
            }
        }

        free(proc->file_descriptors->entries);
        free(proc->file_descriptors->offsets);
        free(proc->file_descriptors->modes);
        free(proc->file_descriptors);
    }
}

void process_reap(process_t *proc)
{
    free(proc->name);

    if (proc->description)
    {
        free(proc->description);
    }

    process_delete(proc);
}

void process_exit(int retval)
{
    log_debug("Task %d exited with code: %d", current_process->id, retval);

    if (current_process->id == 0)
    {
        // This should be a kernel panic. We tried to exit kernel idle
        process_switch_task(1);
        return;
    }

    process_cleanup(process_get_current(), retval);

    // Do not reschedule
    process_switch_task(0);
}

//=============================================================================
// Task switch
//=============================================================================

uint8_t process_available()
{
    return process_ready_queue->head != NULL;
}

process_t *next_ready_process()
{
    PRINT("Next ready process");

    if (!process_available())
    {
        PRINT("No process available");

        return kernel_idle_task;
    }

    spinlock_lock(&process_queue_lock);
    list_node_t *np = list_dequeue(process_ready_queue);
    spinlock_unlock(&process_queue_lock);

    PRINT("Dequeued from ready list");

    ASSERT(np);

    process_t *next = np->payload;

    PRINT("Returning next");

    return next;
}

void switch_next(uintptr_t return_addr)
{
    current_process->thread.rip = return_addr;

    PRINT("Switch next");

    process_t *old_process = (process_t *)current_process;
    thread_t *old_thread = &old_process->thread;

    current_process = next_ready_process();

    arch_x64_64_restore_fpu((void *)current_process->thread.fp_regs);

    if (current_process->finished)
    {
        PRINT("Switched to finished process");
        switch_next(return_addr);
    }

    current_process->started = 1;
    current_process->running = 1;

    PRINT("Old thread rip: %#016x", old_thread->rip);
    // LOOKUP_SYMBOL(old_thread->rip);
    // PRINT("");

    virt_mem_switch_dir(current_process->page_directory);

    switch_to(&old_thread, (thread_t *)&current_process->thread);
}

void make_process_ready(process_t *proc)
{
    PRINT("Make process ready");
    spinlock_lock(&process_queue_lock);
    list_append(process_ready_queue, &proc->sched_node);
    spinlock_unlock(&process_queue_lock);
}

void process_switch_task(uint8_t reschedule)
{
    uintptr_t return_addr = (uintptr_t)__builtin_return_address(0);

    PRINT("Switch task");

    if (!current_process)
    {
        PRINT("No current task");
        return;
    }

    wakeup_sleeping_processes();

    debug_print_process((process_t *)current_process);

    if (!current_process->running)
    {
        PRINT("Current process not running");
        switch_next(return_addr);
    }

    current_process->running = 0;

    arch_x64_64_save_fpu((void *)current_process->thread.fp_regs);

    if (reschedule && current_process != kernel_idle_task)
    {
        make_process_ready((process_t *)current_process);
    }

    switch_next(return_addr);
}

void process_yield(uint8_t reschedule)
{
    process_switch_task(reschedule);
}

//=============================================================================
// Process queries
//=============================================================================

process_t *process_get_current()
{
    return (process_t *)current_process;
}

pid_t process_get_pid()
{
    return current_process->id;
}

uint8_t process_compare(void *proc_v, void *pid_v)
{
    pid_t pid = (*(pid_t *)pid_v);
    process_t *proc = (process_t *)proc_v;

    return (uint8_t)(proc->id == pid);
}

process_t *process_from_pid(pid_t pid)
{
    if (pid < 0)
    {
        return NULL;
    }

    spinlock_lock(&tree_lock);
    tree_node_t *entry = tree_find(process_tree, &pid, process_compare);
    spinlock_unlock(&tree_lock);

    if (entry)
    {
        return (process_t *)entry->value;
    }

    return NULL;
}

process_t *process_get_parent(process_t *process)
{
    process_t *result = NULL;

    spinlock_lock(&tree_lock);
    tree_node_t *entry = process->tree_entry;

    if (entry->parent)
    {
        result = entry->parent->value;
    }

    spinlock_unlock(&tree_lock);

    return result;
}

void process_disown(process_t *proc)
{
    ASSERT(process_tree->root);

    tree_node_t *entry = proc->tree_entry;

    spinlock_lock(&tree_lock);
    tree_break_off(process_tree, entry);
    tree_node_insert_child_node(process_tree, process_tree->root, entry);
    spinlock_unlock(&tree_lock);
}

//=============================================================================
// FD
//=============================================================================

size_t process_append_fd(process_t *proc, fs_node_t *node)
{
    for (size_t i = 0; i < proc->file_descriptors->length; ++i)
    {
        if (!proc->file_descriptors->entries[i])
        {
            proc->file_descriptors->entries[i] = node;
            proc->file_descriptors->modes[i] = 0;
            proc->file_descriptors->offsets[i] = 0;
            return i;
        }
    }

    if (proc->file_descriptors->length == proc->file_descriptors->capacity)
    {
        proc->file_descriptors->capacity *= 2;
        proc->file_descriptors->entries =
            realloc(proc->file_descriptors->entries,
                    sizeof(fs_node_t *) * proc->file_descriptors->capacity);
        proc->file_descriptors->modes =
            realloc(proc->file_descriptors->modes,
                    sizeof(int) * proc->file_descriptors->capacity);
        proc->file_descriptors->offsets =
            realloc(proc->file_descriptors->offsets,
                    sizeof(uint64_t) * proc->file_descriptors->capacity);
    }

    proc->file_descriptors->entries[proc->file_descriptors->length] = node;
    proc->file_descriptors->modes[proc->file_descriptors->length] = 0;
    proc->file_descriptors->offsets[proc->file_descriptors->length] = 0;
    proc->file_descriptors->length++;
    return proc->file_descriptors->length - 1;
}

size_t process_move_fd(process_t *proc, int src, int dest)
{
    if ((size_t)src > proc->file_descriptors->length ||
        (dest != -1 && (size_t)dest > proc->file_descriptors->length))
    {
        return -1;
    }
    if (dest == -1)
    {
        dest = process_append_fd(proc, NULL);
    }
    if (proc->file_descriptors->entries[dest] !=
        proc->file_descriptors->entries[src])
    {
        close_fs(proc->file_descriptors->entries[dest]);
        proc->file_descriptors->entries[dest] =
            proc->file_descriptors->entries[src];
        proc->file_descriptors->modes[dest] =
            proc->file_descriptors->modes[src];
        proc->file_descriptors->offsets[dest] =
            proc->file_descriptors->offsets[src];
        open_fs(proc->file_descriptors->entries[dest], 0);
    }
    return dest;
}

//=============================================================================
// waitpid
//=============================================================================

static int wait_candidate(process_t *parent,
                          int pid,
                          int options,
                          process_t *proc)
{
    log_debug("[WAITPID]: Checking wait candidate");

    if (!proc)
    {
        return 0;
    }

    if (pid < -1)
    {
        if (proc->id == -pid)
        {
            return 1;
        }
    }
    else if (pid > 0)
    {
        /* Specific pid */
        if (proc->id == pid)
        {
            return 1;
        }
    }
    else
    {
        return 1;
    }

    return 0;
}

int waitpid(int pid, int *status, int options)
{
    process_t *proc = (process_t *)current_process;

    log_debug("[WAITPID]: PID: %i, Current process: %i", pid, proc->id);

    int child_count = tree_count_children(proc->tree_entry);

    log_debug("[WAITPID]: Proc child count: %i", child_count);

    do
    {
        process_t *candidate = NULL;
        int has_children = 0;

        for (list_node_t *node = proc->tree_entry->children->head; node != NULL;
             node = node->next)
        {
            log_debug("[WAITPID]: Child tree node %#016x", node);

            if (!node->payload)
            {
                continue;
            }

            tree_node_t *tree_node = (tree_node_t *)node->payload;
            process_t *child = tree_node->value;

            if (wait_candidate(proc, pid, options, child))
            {
                has_children = 1;

                if (child->finished)
                {
                    candidate = child;
                    break;
                }
            }
        }

        if (!has_children)
        {
            log_debug("[WAITPID]: No Children matching");
            return -ECHILD;
        }

        if (candidate)
        {
            if (status)
            {
                *status = candidate->status;
            }

            int pid = candidate->id;

            if (candidate->finished)
            {
                process_reap(candidate);
            }

            log_debug("[WAITPID]: Process finished. Returning...");

            return pid;
        }
        else
        {
            process_sleep(200);
        }

    } while (1);
}

//=============================================================================
// Sleep
//=============================================================================

void process_sleep(uint64_t ms)
{
    uint64_t ticks = ms * TIMER_FREQ / 1000;

    // printf("Task %i entering sleep mode for %d ticks", get_pid(), ticks);

    // We do not append tasks to the sleep queue that do not sleep for a full
    // timeslice
    if (!ticks)
    {
        process_switch_task(1);

        return;
    }

    current_process->sleep_ticks = ticks + get_tick_count();
    current_process->sleeping = 1;

    spinlock_lock(&process_sleeping_lock);
    list_append(process_sleeping_list,
                (list_node_t *)&current_process->sched_node);
    spinlock_unlock(&process_sleeping_lock);

    // printf("Task %i entered sleep mode for %d ticks", get_pid(), ticks);

    process_switch_task(0);
}

void wakeup_sleeping_processes()
{
    // printf("Waking up sleeping processes");

    cli();

    tick_count_t current_ticks = get_tick_count();

    spinlock_lock(&process_sleeping_lock);

    list_node_t *node = process_sleeping_list->head;
    list_node_t *next_node = node;

    while (next_node != NULL)
    {
        process_t *process = next_node->payload;

        if (process->sleep_ticks < current_ticks)
        {
            process->sleeping = 0;
            list_delete(process_sleeping_list, next_node);
            make_process_ready(process);
        }

        next_node = next_node->next;
    }

    spinlock_unlock(&process_sleeping_lock);

    sti();
}

//=============================================================================
// End of file
//=============================================================================
