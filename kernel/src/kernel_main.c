#include <debug/debug_terminal.h>

#include <boot/multiboot2.h>

#include <string.h>
#include <stdio.h>

extern void run_unit_tests();

int kernel_main(unsigned long long rbx, unsigned long long rax)
{
    debug_terminal_initialize();

    printf("================================================================================");
    printf("|| Welcome to the OS6 operating system.                                       ||");
    printf("|| The kernel is now running in 64 bit mode.                                  ||");
    printf("================================================================================");

    run_unit_tests();

    printf("rbx: %l, rax: %l\n", rbx, rax);

    struct multiboot_tag *start_tag = (struct multiboot_tag *)rbx;

    printf("Total size: %i\n", start_tag->size);

    for (;;)
        ;
}
