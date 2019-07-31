#include <process/process.h>

#include <util/bitset.h>

#include <sync/spinlock.h>

#include <arch/arch.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <assert.h>
#include <debug/backtrace.h>
#include <exec/elf64.h>

#define DEBUG_SCHED 0

#if DEBUG_SCHED
#define PRINT(fmt, ...) printf(fmt, ##__VA_ARGS__)
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
	PRINT("---------------------------------------------------------------------------------\n");
	PRINT("PID: %d, Name: %s, Description: %s\n", process->id, process->name, process->description);
	PRINT("Cmdline: %s\n", process->cmdline);
	PRINT("Image: {.entry = %#016x, .size = %#016x, .stack = %#016x, .start = %#016x}\n", process->image.entry, process->image.size, process->image.stack, process->image.start);
	PRINT("Status: %#08x, started: %d, finished: %d, running: %d, suspended: %d\n", process->status, process->started, process->finished, process->running, process->suspended);
	PRINT("---------------------------------------------------------------------------------\n");
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
		PRINT("  ");
	}

	PRINT("%d %s", proc->id, proc->name);

	if (proc->description)
	{
		PRINT(" %s", proc->description);
	}

	if (proc->finished)
	{
		PRINT(" [zombie]");
	}

	PRINT("\n");

	for (list_node_t *child = node->children->head;
		 child != NULL;
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
	PRINT("Installing tasking\n");

	cli();

	initialize_process_tree();

	current_process = spawn_init();
	kernel_idle_task = spawn_idle_thread();

	current_process->running = 1;

	sti();
}

//=============================================================================
// Idle thread
//=============================================================================

static void kernel_idle(void)
{
	while (1)
	{
		printf("Idle thread\n");
		sti();
		__asm__ volatile("hlt"); // TODO: Make this arch-agnostic

		//switch_task(0);
	}
}

process_t *spawn_idle_thread()
{
	PRINT("Spawning idle thread\n");

	process_t *idle = malloc(sizeof(process_t));
	memset(idle, 0x00, sizeof(process_t));
	idle->id = -1;
	idle->name = strdup("[kernel idle thread]");

	uint64_t *stack = (uint64_t *)malloc(sizeof(uint64_t) * KERNEL_STACK_SIZE);
	memset(stack, 0, sizeof(uint64_t) * (KERNEL_STACK_SIZE));

	idle->image.stack = (uint64_t)(stack + KERNEL_STACK_SIZE);

	stack[KERNEL_STACK_SIZE - 1] = (uint64_t)&kernel_idle;
	idle->thread.rsp = &(stack[KERNEL_STACK_SIZE - 17]);
	idle->thread.rip = (uintptr_t)&kernel_idle;

	idle->started = 1;
	idle->running = 1;

	return idle;
}

//=============================================================================
// Spawn kernel idle process
//=============================================================================

extern uint64_t stack_top;

process_t *spawn_init()
{
	PRINT("Spawning init thread\n");

	process_t *init = malloc(sizeof(process_t));

	memset(init, 0, sizeof(process_t));

	tree_set_root(process_tree, (void *)init);

	list_insert(process_list, (void *)init);

	init->tree_entry = process_tree->root;
	init->id = 1;
	init->name = strdup("init");
	init->cmdline = NULL;
	init->status = 0;

	init->thread.rip = 0;
	init->thread.rsp = 0;

	init->image.entry = 0;
	init->image.size = 0;
	init->image.stack = (uint64_t)&stack_top;
	init->image.start = 0;

	init->finished = 0;
	init->suspended = 0;
	init->started = 0;
	init->running = 0;
	init->sleeping = 0;

	init->sched_node.payload = init;

	return init;
}

//=============================================================================
// Spawn new process
//=============================================================================

process_t *spawn_process(process_t *parent)
{
	PRINT("Spawning new process\n");

	ASSERT(process_tree->root);

	process_t *proc = (process_t *)malloc(sizeof(process_t));
	memset(proc, 0, sizeof(process_t));

	proc->id = get_next_free_pid();

	PRINT("Got pid: %d\n", proc->id);

	proc->name = strdup("new process");
	proc->description = NULL;
	proc->cmdline = parent->cmdline;

	//proc->thread.fpu_enabled = 0;
	//memcpy((void *)proc->thread.fp_regs, (void *)parent->thread.fp_regs, 512);

	uint64_t *stack = (uint64_t *)malloc(sizeof(uint64_t) * KERNEL_STACK_SIZE);

	PRINT("Child stack: %#016x-%#016x\n", stack, stack + KERNEL_STACK_SIZE);

	memcpy(stack, (void *)parent->image.stack, sizeof(uint64_t) * (KERNEL_STACK_SIZE));

	proc->image.stack = (uint64_t)(stack + KERNEL_STACK_SIZE);

	stack[KERNEL_STACK_SIZE - 1] = (uint64_t)parent->thread.rip;
	proc->thread.rsp = &(stack[KERNEL_STACK_SIZE - 17]);
	proc->thread.rip = (uintptr_t)parent->thread.rip;

	proc->image.entry = parent->image.entry;
	proc->image.size = parent->image.size;
	proc->image.start = parent->image.start;

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

pid_t fork()
{
	PRINT("Fork\n");

	cli();

	uintptr_t return_addr = (uintptr_t)__builtin_return_address(0);

	current_process->regs->rax = 0;

	process_t *parent = (process_t *)current_process;

	ASSERT(parent);

	// TODO: Clone address space

	parent->thread.rip = return_addr;

	process_t *new_proc = spawn_process(parent);

	ASSERT(new_proc);

	volatile uintptr_t var = (uintptr_t)(get_rsp_val() + 8);
	parent->thread.rsp = (uint64_t *)(get_rsp_val() + 8);
	volatile uintptr_t diff = parent->image.stack - 1 - (var);

	PRINT("Parent thread rsp: %#016x\n", parent->thread.rsp);
	PRINT("Diff: %#016x\n", diff);

	new_proc->thread.rsp = (uint64_t *)(((uint64_t)new_proc->thread.rsp) - 8 * ((uint64_t)diff));

	PRINT("New thread rsp: %#016x\n", new_proc->thread.rsp);

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
// Process cleanup
//=============================================================================

void delete_process(process_t *proc)
{
	tree_node_t *entry = proc->tree_entry;

	if (!entry)
	{
		return;
	}

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

void cleanup_process(process_t *proc, int retval)
{
	proc->status = retval;
	proc->finished = 1;
}

void reap_process(process_t *proc)
{
	free(proc->name);

	if (proc->description)
	{
		free(proc->description);
	}

	delete_process(proc);
}

void task_exit(int retval)
{
	printf("Task %d exited with code: %d\n", current_process->id, retval);

	if (current_process->id == 0)
	{
		// This should be a kernel panic. We tried to exit kernel idle
		switch_task(1);
		return;
	}

	cleanup_process(get_current_process(), retval);

	// Do not reschedule
	switch_task(0);
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
	PRINT("Next ready process\n");

	if (!process_available())
	{
		PRINT("No process available\n");

		return kernel_idle_task;
	}

	spinlock_lock(&process_queue_lock);
	list_node_t *np = list_dequeue(process_ready_queue);
	spinlock_unlock(&process_queue_lock);

	PRINT("Dequeued from ready list\n");

	ASSERT(np);

	process_t *next = np->payload;

	PRINT("Returning next\n");

	return next;
}

void switch_next(uintptr_t return_addr)
{
	current_process->thread.rip = return_addr;

	PRINT("Switch next\n");

	process_t *old_process = (process_t *)current_process;
	thread_t *old_thread = &old_process->thread;

	current_process = next_ready_process();

	// TODO: Switch FPU

	if (current_process->finished)
	{
		PRINT("Switched to finished process\n");
		switch_next(return_addr);
	}

	current_process->started = 1;
	current_process->running = 1;

	PRINT("Old thread rip: %#016x\n", old_thread->rip);
	LOOKUP_SYMBOL(old_thread->rip);
	PRINT("\n");

	switch_to(&old_thread, (thread_t *)&current_process->thread);
}

void make_process_ready(process_t *proc)
{
	PRINT("Make process ready\n");
	spinlock_lock(&process_queue_lock);
	list_append(process_ready_queue, &proc->sched_node);
	spinlock_unlock(&process_queue_lock);
}

void switch_task(uint8_t reschedule)
{
	uintptr_t return_addr = (uintptr_t)__builtin_return_address(0);

	PRINT("Switch task\n");

	if (!current_process)
	{
		PRINT("No current task\n");
		return;
	}

	wakeup_sleeping_processes();

	debug_print_process((process_t *)current_process);

	if (!current_process->running)
	{
		PRINT("Current process not running\n");
		switch_next(return_addr);
	}

	current_process->running = 0;

	// TODO: Save FPU state

	if (reschedule && current_process != kernel_idle_task)
	{
		make_process_ready((process_t *)current_process);
	}

	switch_next(return_addr);
}

void process_yield(uint8_t reschedule)
{
	switch_task(reschedule);
}

//=============================================================================
// Process queries
//=============================================================================

process_t *get_current_process()
{
	return (process_t *)current_process;
}

pid_t get_pid()
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

//=============================================================================
// waitpid
//=============================================================================

//=============================================================================
// Sleep
//=============================================================================

void process_sleep(uint64_t ms)
{
	uint64_t ticks = ms * TIMER_FREQ / 1000;

	//printf("Task %i entering sleep mode for %d ticks\n", get_pid(), ticks);

	// We do not append tasks to the sleep queue that do not sleep for a full
	// timeslice
	if (!ticks)
	{
		switch_task(1);

		return;
	}

	current_process->sleep_ticks = ticks + get_tick_count();
	current_process->sleeping = 1;

	spinlock_lock(&process_sleeping_lock);
	list_append(process_sleeping_list, (list_node_t *)&current_process->sched_node);
	spinlock_unlock(&process_sleeping_lock);

	//printf("Task %i entered sleep mode for %d ticks\n", get_pid(), ticks);

	switch_task(0);
}

void wakeup_sleeping_processes()
{
	//printf("Wakign up sleeping processes\n");

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