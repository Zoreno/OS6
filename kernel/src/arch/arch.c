#include <arch/arch.h>

#ifdef ARCH_X86_64
#include <arch/x86-64/cpu.h>
#endif

void arch_initialize()
{
#ifdef ARCH_X86_64
    arch_x86_64_initialize_cpu();
#endif
}

void arch_shutdown(){
#ifdef ARCH_X86_64
#endif
}

uint8_t inportb(uint16_t port)
{
    uint8_t ret;

    __asm__ volatile("in %1, %0"
                     : "=a"(ret)
                     : "Nd"(port));

    return ret;
}

uint16_t inportw(uint16_t port)
{
    uint16_t ret;

    __asm__ volatile("in %1, %0"
                     : "=a"(ret)
                     : "Nd"(port));

    return ret;
}

uint32_t inportl(uint16_t port)
{
    uint32_t ret;

    __asm__ volatile("in %1, %0"
                     : "=a"(ret)
                     : "Nd"(port));

    return ret;
}

void outportb(uint16_t port, uint8_t value)
{
    __asm__ volatile("out %0, %1"
                     :
                     : "a"(value), "Nd"(port));
}

void outportw(uint16_t port, uint16_t value)
{
    __asm__ volatile("out %0, %1"
                     :
                     : "a"(value), "Nd"(port));
}

void outportl(uint16_t port, uint32_t value)
{
    __asm__ volatile("out %0, %1"
                     :
                     : "a"(value), "Nd"(port));
}

void sti()
{
    __asm__ volatile("sti");
}

void cli()
{
    __asm__ volatile("cli");
}

void interrupt_done(uint32_t intno)
{
    intno -= 0x20;

    if (intno > 16)
    {
        return;
    }
}