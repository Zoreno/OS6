ENTRY(start)

SECTIONS
{
    . = 1M;

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
}