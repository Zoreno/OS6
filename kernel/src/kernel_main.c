#include <debug/debug_terminal.h>

#include <string.h>

#include <ctype.h>

int kernel_main(unsigned long long rbx, unsigned long long rax)
{
    (void)rbx;
    (void)rax;

    debug_terminal_initialize();

    debug_terminal_print("Welcome to the OS6 operating system. The kernel is now running in 64 bit mode.");

    isalnum('c');
    isalpha('c');
    iscntrl('c');
    isgraph('c');
    isdigit('c');
    islower('c');
    isprint('c');
    ispunct('c');
    isspace('c');
    isupper('c');
    isxdigit('c');

    tolower('c');
    toupper('c');

    isascii('c');
    toascii('c');

    for (;;)
        ;
}