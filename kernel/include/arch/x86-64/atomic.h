#ifndef _ARCH_X86_64_ATOMIC_H
#define _ARCH_X86_64_ATOMIC_H

int arch_x86_64_atomic_swap(volatile long long int *x, long long int v);
void arch_x86_64_atomic_store(volatile long long int *p, long long int x);
void arch_x86_64_atomic_inc(volatile long long int *x);
void arch_x86_64_atomic_dec(volatile long long int *x);

#endif