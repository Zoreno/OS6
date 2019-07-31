/**
 * @file pit.c
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

#include <arch/x86-64/pit.h>

#include <arch/x86-64/pic.h>

#include <stdio.h>

#include <process/process.h>

#define ARCH_X86_64_PIT_REG_COUNTER0 0x40
#define ARCH_X86_64_PIT_REG_COUNTER1 0x41
#define ARCH_X86_64_PIT_REG_COUNTER2 0x42
#define ARCH_X86_64_PIT_REG_COMMAND 0x43

static volatile tick_count_t _pit_ticks = 0;
static int _is_initialized = 0;

static on_tick_handler_func _on_tick_handler = NULL;

void arch_x86_64_set_on_tick_handler(on_tick_handler_func on_tick_handler)
{
    _on_tick_handler = on_tick_handler;
}

void arch_x86_64_pit_irq(system_stack_t *regs)
{
    (void)regs;

    ++_pit_ticks;

    //printf("[PIC] PIT tick: %8i\n", _pit_ticks);

    if (_on_tick_handler)
    {
        _on_tick_handler();
    }
}

void arch_x86_64_pit_send_command(uint8_t cmd)
{
    outportb(ARCH_X86_64_PIT_REG_COMMAND, cmd);
}

void arch_x86_64_pit_send_data(uint16_t counter, uint8_t data)
{
    uint8_t port = 0;

    switch (counter)
    {
    case ARCH_X86_64_PIT_OCW_COUNTER_0:
        port = ARCH_X86_64_PIT_REG_COUNTER0;
        break;
    case ARCH_X86_64_PIT_OCW_COUNTER_1:
        port = ARCH_X86_64_PIT_REG_COUNTER1;
        break;
    case ARCH_X86_64_PIT_OCW_COUNTER_2:
        port = ARCH_X86_64_PIT_REG_COUNTER2;
        break;

    default:
        // ERROR: Invalid counter
        break;
    }

    outportb(port, data);
}

uint8_t arch_x86_64_pit_read_data(uint16_t counter)
{
    uint8_t port = 0;

    switch (counter)
    {
    case ARCH_X86_64_PIT_OCW_COUNTER_0:
        port = ARCH_X86_64_PIT_REG_COUNTER0;
        break;
    case ARCH_X86_64_PIT_OCW_COUNTER_1:
        port = ARCH_X86_64_PIT_REG_COUNTER1;
        break;
    case ARCH_X86_64_PIT_OCW_COUNTER_2:
        port = ARCH_X86_64_PIT_REG_COUNTER2;
        break;

    default:
        // ERROR: Invalid counter
        break;
    }

    return inportb(port);
}

tick_count_t arch_x86_64_pit_set_tick_count(tick_count_t i)
{
    tick_count_t old_val = _pit_ticks;
    _pit_ticks = i;
    return old_val;
}

tick_count_t arch_x86_64_pit_get_tick_count()
{
    return _pit_ticks;
}

void arch_x86_64_pit_start_counter(uint32_t freq, uint8_t counter, uint8_t mode)
{
    if (freq == 0)
        return;

    uint32_t divisor = (uint16_t)(1193181 / (uint16_t)freq);

    uint8_t ocw = 0;

    ocw = (ocw & ARCH_X86_64_PIT_OCW_MASK_MODE) | mode;
    ocw = (ocw & ARCH_X86_64_PIT_OCW_MASK_RL) | ARCH_X86_64_PIT_OCW_RL_DATA;
    ocw = (ocw & ARCH_X86_64_PIT_OCW_MASK_COUNTER) | counter;

    arch_x86_64_pit_send_command(ocw);

    arch_x86_64_pit_send_data(0, divisor & 0xFF);
    arch_x86_64_pit_send_data(0, (divisor >> 8) & 0xFF);

    _pit_ticks = 0;
}

void arch_x86_64_initialize_pit()
{
    set_irq_handler(PIC_IRQ_TIMER, arch_x86_64_pit_irq);

    arch_x86_64_pic_clear_mask_interrupt(PIC_IRQ_TIMER);

    _is_initialized = 1;
}

int arch_X86_64_pit_is_initialzed()
{
    return _is_initialized;
}