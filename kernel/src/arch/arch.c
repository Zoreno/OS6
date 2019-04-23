#include <arch/arch.h>
#include <stdio.h>

#ifdef ARCH_X86_64
#include <arch/x86-64/cpu.h>
#include <arch/x86-64/pic.h>
#include <arch/x86-64/idt.h>
#include <arch/x86-64/pit.h>
#include <arch/x86-64/atomic.h>
#endif

void arch_initialize()
{
#ifdef ARCH_X86_64
    printf("[ARCH] x64-64 initializing\n");

    arch_x86_64_initialize_cpu();

    printf("[ARCH] Initializing PIC\n");

    arch_x86_64_initialize_pic(0x20, 0x28);

    printf("[ARCH] PIC Done!\n");

    printf("[ARCH] Initializing PIT\n");

    //arch_x86_64_initialize_pit();
    //arch_x86_64_pit_start_counter(100, ARCH_X86_64_PIT_OCW_COUNTER_0, ARCH_X86_64_PIT_OCW_MODE_SQUAREWAVEGEN);

    printf("[ARCH] PIT Done!\n");

    printf("[ARCH] x64-64 Done! \n");
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
    printf("STI()\n");

    __asm__ volatile("sti");
}

void cli()
{
    printf("CLI()\n");

    __asm__ volatile("cli");
}

void interrupt_done(uint32_t intno)
{
    intno -= 0x20;

    if (intno >= 16)
    {
        return;
    }

    if (intno >= 8)
    {
        arch_x86_64_pic_send_command(PIC_OCW2_MASK_EOI, 1);
    }

    arch_x86_64_pic_send_command(PIC_OCW2_MASK_EOI, 0);
}

void set_interrupt_handler(int intno, INT_HANDLER int_handler, int flags)
{
    // TODO: Define and parse arch-agnostic interrupt flags
    (void)flags;

    arch_x86_64_install_ir(intno, ARCH_X86_64_IDT_DESC_PRESENT | ARCH_X86_64_IDT_DESC_BIT32, 0x08, int_handler);
}

void set_irq_handler(int irq, IRQ_HANDLER irq_handler)
{
    arch_x86_64_install_irq(irq, irq_handler);
}

uint32_t get_tick_count()
{
    return arch_x86_64_pit_get_tick_count();
}

void set_on_tick_handler(on_tick_handler_func on_tick_handler)
{
    arch_x86_64_set_on_tick_handler(on_tick_handler);
}

void set_mask_interrupt(uint8_t irq)
{
    arch_x86_64_pic_set_mask_interrupt(irq);
}

void clear_mask_interrupt(uint8_t irq)
{
    arch_x86_64_pic_clear_mask_interrupt(irq);
}

int atomic_swap(volatile long long int *x, long long int v)
{
    return arch_x86_64_atomic_swap(x, v);
}

void atomic_store(volatile long long int *p, long long int x)
{
    arch_x86_64_atomic_store(p, x);
}

void atomic_inc(volatile long long int *x)
{
    arch_x86_64_atomic_inc(x);
}

void atomic_dec(volatile long long int *x)
{
    arch_x86_64_atomic_dec(x);
}