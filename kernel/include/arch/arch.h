#ifndef _ARCH_H
#define _ARCH_H

#include <stdint.h>

#define ARCH_X86_64

typedef void (*INT_HANDLER)(void);

typedef struct __system_stack
{
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8; /* pushed by 'pushall' */
    uint64_t rdi, rsi, rbp, rbx, rdx, rcx, rax;    /* pushed in registers */
    uint64_t int_no, err_code;                     /* our 'push byte #' and ecodes do this */
    uint64_t rip, cs, rflags, userrsp, ss;         /* pushed by the processor automatically */

} system_stack_t;

typedef void (*IRQ_HANDLER)(system_stack_t *);

typedef void (*on_tick_handler_func)(void);

void arch_initialize();
void arch_shutdown();

uint8_t inportb(uint16_t port);
uint16_t inportw(uint16_t port);
uint32_t inportl(uint16_t port);

void outportb(uint16_t port, uint8_t value);
void outportw(uint16_t port, uint16_t value);
void outportl(uint16_t port, uint32_t value);

static inline void udelay(uint64_t n)
{
    if (!n)
    {
        return;
    }

    __asm__ volatile("1: dec %%rax; jne 1b;" ::"a"(n * 1000));
}

static inline void mdelay(uint32_t n)
{
    while (n--)
    {
        udelay(1000);
    }
}

void sti();
void cli();

void interrupt_done(uint32_t intno);

void set_interrupt_handler(int intno, INT_HANDLER int_handler, int flags);

void set_irq_handler(int irq, IRQ_HANDLER irq_handler);

uint32_t get_tick_count();

void set_on_tick_handler(on_tick_handler_func on_tick_handler);

void set_mask_interrupt(uint8_t irq);
void clear_mask_interrupt(uint8_t irq);

int atomic_swap(volatile long long int *x, long long int v);
void atomic_store(volatile long long int *p, long long int x);
void atomic_inc(volatile long long int *x);
void atomic_dec(volatile long long int *x);

#endif