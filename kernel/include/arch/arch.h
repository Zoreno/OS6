/**
 * @file arch.h
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-06-22
 * 
 * @brief 
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

#ifndef _ARCH_H
#define _ARCH_H

#include <stdint.h>

#define ARCH_X86_64

#define TIMER_FREQ 50

typedef uint64_t tick_count_t;

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

    __asm__ volatile("1: dec %%rax; jne 1b;"
                     :
                     : "a"(n * 1000));
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

tick_count_t get_tick_count();

void set_on_tick_handler(on_tick_handler_func on_tick_handler);

void set_mask_interrupt(uint8_t irq);
void clear_mask_interrupt(uint8_t irq);

//=============================================================================
// Atomic operations
//=============================================================================

int atomic_swap(volatile long long int *x, long long int v);
void atomic_store(volatile long long int *p, long long int x);
void atomic_inc(volatile long long int *x);
void atomic_dec(volatile long long int *x);

#define ATOMIC_XADD(P, V) __sync_fetch_and_add((P), (V))
#define ATOMIC_CMPXCHG(P, O, N) __sync_val_compare_and_swap((P), (O), (N))
#define ATOMIC_INC(P) __sync_add_and_fetch((P), 1);
#define ATOMIC_DEC(P) __sync_add_and_fetch((P), -1);
#define ATOMIC_ADD(P, V) __sync_and_fetch((P), (V))
#define ATOMIC_SET_BIT(P, B) __sync_or_and_fetch((P), 1 << (V))
#define ATOMIC_CLEAR_BIT(P, B) __sync_and_and_fetch((P), ~(1 << (V)))
#define BARRIER __asm__ volatile("" :: \
                                     : "memory")
static inline void *xchg_64(void *ptr, void *x)
{
    __asm__ __volatile__("xchgq %0,%1"
                         : "=r"((unsigned long long)x)
                         : "m"(*(volatile long long *)ptr), "0"((unsigned long long)x)
                         : "memory");

    return x;
}

static inline unsigned xchg_32(void *ptr, unsigned x)
{
    __asm__ __volatile__("xchgl %0,%1"
                         : "=r"((unsigned)x)
                         : "m"(*(volatile unsigned *)ptr), "0"(x)
                         : "memory");

    return x;
}

static inline unsigned short xchg_16(void *ptr, unsigned short x)
{
    __asm__ __volatile__("xchgw %0,%1"
                         : "=r"((unsigned short)x)
                         : "m"(*(volatile unsigned short *)ptr), "0"(x)
                         : "memory");

    return x;
}

static inline char atomic_bitsetandtest(void *ptr, int x)
{
    char out;
    __asm__ __volatile__("lock; bts %2,%1\n"
                         "sbb %0,%0\n"
                         : "=r"(out), "=m"(*(volatile long long *)ptr)
                         : "Ir"(x)
                         : "memory");

    return out;
}

#endif