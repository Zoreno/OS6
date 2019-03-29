#include <arch/x86-64/idt.h>

#include <string.h>
#include <stdio.h>

extern void arch_x86_64_idt_flush(uint64_t idtr_ptr);

typedef struct
{
    uint16_t limit;
    uint64_t base;
} __attribute__((packed)) arch_x86_64_idtr;

arch_x86_64_idt_descriptor _idt[ARCH_X86_64_IDT_MAX_INTERRUPTS];
arch_x86_64_idtr _idtr;

void arch_x86_64_default_irq_handler(uint64_t intno);

void arch_x86_64_default_irq_handler(uint64_t intno)
{
    printf("[IRQ] Unhandled Exception (%i)", intno);
    for (;;)
        ;
}

arch_x86_64_idt_descriptor *arch_x86_64_get_ir(uint32_t i)
{
    if (i > ARCH_X86_64_IDT_MAX_INTERRUPTS)
    {
        return NULL;
    }

    return &_idt[i];
}

void arch_x86_64_install_ir(uint32_t i, uint16_t flags, uint16_t sel, INT_HANDLER int_handler)
{
    if (i > ARCH_X86_64_IDT_MAX_INTERRUPTS)
    {
        return;
    }

    if (!int_handler)
    {
        return;
    }

    memset(&_idt[i], 0, sizeof(arch_x86_64_idt_descriptor));

    uint64_t base = (uint64_t) & (*int_handler);

    _idt[i].baseLo = (uint16_t)(base & 0xFFFF);
    _idt[i].baseMid = (uint16_t)((base >> 16) & 0xFFFF);
    _idt[i].flags = (uint8_t)flags;
    _idt[i].baseHi = (uint32_t)((base >> 32) & 0xFFFFFFFF);
    _idt[i].sel = sel;
}

extern void default_irq_handler_1(void);

void arch_x86_64_initialize_idt(uint16_t code_sel)
{
    printf("[ARCH] Initializing IDT...\n");

    _idtr.limit = (sizeof(arch_x86_64_idt_descriptor)) * ARCH_X86_64_IDT_MAX_INTERRUPTS - 1;
    _idtr.base = (uint64_t)&_idt[0];

    int num_ints = ARCH_X86_64_IDT_MAX_INTERRUPTS;

    for (int i = 0; i < num_ints; ++i)
    {
        arch_x86_64_install_ir(i,
                               ARCH_X86_64_IDT_DESC_BIT32 | ARCH_X86_64_IDT_DESC_PRESENT,
                               code_sel,
                               default_irq_handler_1);
    }

    arch_x86_64_idt_flush((uint64_t)&_idtr);

    printf("[ARCH] IDT Initialized!\n");
}
