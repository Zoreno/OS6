#include <syscall/syscall.h>

#include <arch/arch.h>

#include <stdio.h>

#include <debug/backtrace.h>

#include <process/process.h>

void syscall_handler(system_stack_t *stack)
{
    //printf("Got system call: %i\n", stack->rax);

    switch (stack->rax)
    {
    case 0:
    {
        printf("%s", stack->rbx);
    }
    case 1:
    {
        task_exit(stack->rbx);
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