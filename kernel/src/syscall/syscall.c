#include <syscall/syscall.h>

#include <arch/arch.h>

#include <stdio.h>

void syscall_handler(system_stack_t *stack)
{
    printf("Got system call: %i\n", stack->rax);

    stack->rax = 14;
}

void syscall_install()
{
    set_irq_handler(SYSCALL_INTNO, syscall_handler);
}