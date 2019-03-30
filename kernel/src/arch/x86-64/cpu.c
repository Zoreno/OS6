#include <arch/arch.h>
#include <arch/x86-64/cpu.h>
#include <arch/x86-64/gdt.h>
#include <arch/x86-64/idt.h>
#include <stdio.h>

void arch_x86_64_initialize_cpu()
{
    printf("[ARCH] Initializing CPU...\n");
    //arch_x86_64_initialize_gdt();
    arch_x86_64_initialize_idt(0x08);
    printf("[ARCH] CPU Done!\n");
}

void arch_x86_64_shutdown_cpu()
{
}