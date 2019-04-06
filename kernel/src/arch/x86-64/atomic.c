#include <arch/x86-64/atomic.h>

int arch_x86_64_atomic_swap(volatile long long int *x, long long int v)
{
    __asm__ volatile("xchg %0, %1"
                     : "=r"(v), "=m"(*x)
                     : "0"(v)
                     : "memory");

    return v;
}

void arch_x86_64_atomic_store(volatile long long int *p, long long int x)
{
    __asm__ volatile("mov %1, %0"
                     : "=m"(*p)
                     : "r"(x)
                     : "memory");
}

void arch_x86_64_atomic_inc(volatile long long int *x)
{
    __asm__ volatile("lock; inc %0"
                     : "=r"(*x)
                     : "m"(*x)
                     : "memory");
}

void arch_x86_64_atomic_dec(volatile long long int *x)
{
    __asm__ volatile("lock; dec %0"
                     : "=r"(*x)
                     : "m"(*x)
                     : "memory");
}