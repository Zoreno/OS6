#ifndef _ARCH_X86_64_GDT_H
#define _ARCH_X86_64_GDT_H

#include <stdint.h>

#define ARCH_X86_64_GDT_MAX_DESCRIPTORS 3
#define ARCH_X86_64_GDT_DESC_ACCESS 0x01
#define ARCH_X86_64_GDT_DESC_READWRITE 0x02
#define ARCH_X86_64_GDT_DESC_EXPANSION 0x04
#define ARCH_X86_64_GDT_DESC_EXEC_CODE 0x08
#define ARCH_X86_64_GDT_DESC_CODEDATA 0x10
#define ARCH_X86_64_GDT_DESC_DPL0 0x00
#define ARCH_X86_64_GDT_DESC_DPL1 0x20
#define ARCH_X86_64_GDT_DESC_DPL2 0x40
#define ARCH_X86_64_GDT_DESC_DPL3 0x60
#define ARCH_X86_64_GDT_DESC_MEMORY 0x80

#define ARCH_X86_64_GDT_GRAN_LIMITHI 0x0F
#define ARCH_X86_64_GDT_GRAN_OS 0x10
#define ARCH_X86_64_GDT_GRAN_32BIT 0x40
#define ARCH_X86_64_GDT_GRAN_4K 0x80

typedef struct
{
    uint32_t res1;
    uint8_t res2;
    uint8_t flags;
    uint8_t gran;
    uint8_t res3;
} __attribute__((packed, aligned(8))) arch_x86_64_gdt_descriptor;

void arch_x86_64_gdt_set_descriptor(int i, int flags, int gran);
arch_x86_64_gdt_descriptor *arch_x86_64_get_descriptor(int i);
void arch_x86_64_initialize_gdt();

#endif