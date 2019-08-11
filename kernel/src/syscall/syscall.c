#include <syscall/syscall.h>

#include <arch/arch.h>

#include <stdio.h>

#include <debug/backtrace.h>

#include <process/process.h>

void syscall_handler(system_stack_t *stack)
{
	//printf("Got system call: %i\n", stack->rax);

	// TODO: This should be a lookup in a syscall array
	// TODO: All syscall function handlers should be in a separate file
	switch (stack->rax)
	{
	case 0:
	{
		printf("%s", stack->rbx);
	}
	break;
	case 1:
	{
		process_exit(stack->rbx);
	}
	break;
	case 2:
	{
		process_yield(1);
	}
	break;
	case 3:
	{
		process_sleep(stack->rbx);
	}
	break;
	default:
	{
		printf("[SYSCALL] Invalid system call: %i", stack->rax);
		for (;;)
			;
	}
	break;
	}

	stack->rax = 14;
}

void syscall_install()
{
	set_irq_handler(SYSCALL_INTNO, syscall_handler);
}