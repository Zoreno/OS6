ENTRY(start)

SECTIONS
{
    . = 1M;
    __kernel_start = .;

    .boot :
    {
        *(.multiboot_header)
    }

    .text :
    {
        *(.text)
        *(.rodata*)
    }

    .data ALIGN(0x1000) :
    {
        *(.data)
    }

    .bss :
    {
        _sbss = .;
        *(COMMON)
        *(.bss)
        _ebss = .;
    }

    __kernel_end = .;
}