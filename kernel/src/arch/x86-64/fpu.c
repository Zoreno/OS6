#include <arch/x86-64/fpu.h>

void arch_x64_64_set_fpu_cw(const uint16_t cw)
{
    __asm__ volatile("fldcw %0" ::"m"(cw));
}

void arch_x64_64_get_fpu_cw(uint16_t *cw)
{
    __asm__ volatile("fstcw %0"
                     : "=m"(*cw));
}

void arch_x86_64_set_rounding_mode(uint16_t mode)
{
    union {
        uint16_t cwi;
        arch_x86_64_fpu_cw_t cw;
    } u;

    if (mode > 3)
    {
        return;
    }

    arch_x64_64_get_fpu_cw(&u.cwi);

    u.cw.rc = mode;

    arch_x64_64_set_fpu_cw(u.cwi);
}

uint16_t arch_x86_64_get_rounding_mode()
{
    union {
        uint16_t cwi;
        arch_x86_64_fpu_cw_t cw;
    } u;

    arch_x64_64_get_fpu_cw(&u.cwi);

    return u.cw.rc;
}

void arch_x64_64_enable_fpu()
{
    __asm__ volatile("clts");

    size_t t;

    __asm__ volatile("mov %%cr0, %0"
                     : "=r"(t));

    t &= ~(1 << 2);
    t |= (1 << 1);

    __asm__ volatile("mov %0, %%cr0" ::"r"(t));

    __asm__ volatile("mov %%cr4, %0"
                     : "=r"(t));

    t |= 3 << 9;

    __asm__ volatile("mov %0, %%cr4" ::"r"(t));
}

void arch_x64_64_disable_fpu()
{
    size_t t;

    __asm__ volatile("mov %%cr0, %0"
                     : "=r"(t));

    t |= (1 << 3);

    __asm__ volatile("mov %0, %%cr0" ::"r"(t));
}

void arch_x64_64_init_fpu()
{
    __asm__ volatile("fninit");
}

void arch_x64_64_install_fpu()
{
    arch_x64_64_enable_fpu();
    arch_x64_64_init_fpu();
}