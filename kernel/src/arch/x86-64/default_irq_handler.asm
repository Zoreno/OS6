bits 64

extern arch_x86_64_default_irq_handler

%macro DEF_IRQ_HANDLER 1

global default_irq_handler_%1
default_irq_handler_%1:
    cli
    mov rdi, %1
    call arch_x86_64_default_irq_handler
    sti
    iretq
%endmacro

%assign i 1
%rep 256
DEF_IRQ_HANDLER i
%assign i i+1
%endrep