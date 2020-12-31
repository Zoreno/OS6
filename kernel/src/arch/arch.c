/**
 * @file arch.c
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

#include <arch/arch.h>

#include <stdio.h>

#include <logging/logging.h>

#ifdef ARCH_X86_64
#include <arch/x86-64/atomic.h>
#include <arch/x86-64/cpu.h>
#include <arch/x86-64/fpu.h>
#include <arch/x86-64/idt.h>
#include <arch/x86-64/pic.h>
#include <arch/x86-64/pit.h>
#endif

void arch_initialize()
{
#ifdef ARCH_X86_64
    log_info("[ARCH] x64-64 initializing");

    arch_x86_64_initialize_cpu();

    log_info("[ARCH] Initializing PIC");

    arch_x86_64_initialize_pic(0x20, 0x28);

    log_info("[ARCH] PIC Done!");

    log_info("[ARCH] Initializing PIT");

    arch_x86_64_initialize_pit();
    arch_x86_64_pit_start_counter(TIMER_FREQ, ARCH_X86_64_PIT_OCW_COUNTER_0, ARCH_X86_64_PIT_OCW_MODE_SQUAREWAVEGEN);

    log_info("[ARCH] PIT Done!");

    log_info("[ARCH] Initializing FPU");

    arch_x64_64_install_fpu();

    log_info("[ARCH] FPU Done!");

    log_info("[ARCH] x64-64 Done!");
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
    //printf("STI()\n");

    __asm__ volatile("sti" ::
                         : "memory");
}

void cli()
{
    //printf("CLI()\n");

    __asm__ volatile("cli" ::
                         : "memory");
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

tick_count_t get_tick_count()
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