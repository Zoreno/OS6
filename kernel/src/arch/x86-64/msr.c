#include <arch/x86-64/msr.h>

uint64_t rdmsr(msr_registers_t msr)
{
    uint32_t low;
    uint32_t high;

    __asm__ volatile("rdmsr" : "=a"(low), "=d"(high) : "c"((uint64_t)msr));

    return ((uint64_t)high << 32) | low;
}

void wrmsr(msr_registers_t msr, uint64_t value)
{
    uint32_t low = value & 0xFFFFFFFF;
    uint32_t high = value >> 32;

    __asm__ volatile("wrmsr" ::"c"((uint64_t)msr), "a"(low), "d"(high));
}
