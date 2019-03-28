#include <arch/x86-64/gdt.h>

#include <string.h>

extern void arch_x86_64_gdt_flush(uint64_t gdt_ptr);

typedef struct
{
    uint16_t limit;
    uint64_t base;
} __attribute__((packed, aligned(8))) arch_x86_64_gdtr;

arch_x86_64_gdt_descriptor _gdt[ARCH_X86_64_GDT_MAX_DESCRIPTORS];

arch_x86_64_gdtr _gdtr;

void arch_x86_64_gdt_set_descriptor(int i, int flags, int gran)
{
    if (i > ARCH_X86_64_GDT_MAX_DESCRIPTORS)
    {
        return;
    }

    memset((void *)&_gdt[i], 0, sizeof(arch_x86_64_gdt_descriptor));

    _gdt[i].flags = flags;
    _gdt[i].gran = gran;
}

arch_x86_64_gdt_descriptor *arch_x86_64_get_descriptor(int i)
{
    if (i > ARCH_X86_64_GDT_MAX_DESCRIPTORS)
    {
        return NULL;
    }

    return &_gdt[i];
}

void arch_x86_64_initialize_gdt()
{
    _gdtr.limit = (uint16_t)(sizeof(arch_x86_64_gdt_descriptor) * ARCH_X86_64_GDT_MAX_DESCRIPTORS - 1);
    _gdtr.base = (uint64_t)_gdt;

    arch_x86_64_gdt_set_descriptor(0, 0, 0);
    arch_x86_64_gdt_set_descriptor(1,
                                   ARCH_X86_64_GDT_DESC_READWRITE | ARCH_X86_64_GDT_DESC_EXEC_CODE | ARCH_X86_64_GDT_DESC_CODEDATA | ARCH_X86_64_GDT_DESC_DPL0 | ARCH_X86_64_GDT_DESC_MEMORY,
                                   ARCH_X86_64_GDT_GRAN_4K | ARCH_X86_64_GDT_GRAN_LIMITHI);

    arch_x86_64_gdt_set_descriptor(2,
                                   ARCH_X86_64_GDT_DESC_READWRITE | ARCH_X86_64_GDT_DESC_CODEDATA | ARCH_X86_64_GDT_DESC_DPL0 | ARCH_X86_64_GDT_DESC_MEMORY,
                                   ARCH_X86_64_GDT_GRAN_4K | ARCH_X86_64_GDT_GRAN_LIMITHI);

    arch_x86_64_gdt_flush((uint64_t)&_gdtr);
}
