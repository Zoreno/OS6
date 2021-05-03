global arch_x86_64_idt_flush

arch_x86_64_idt_flush:
    mov rax, rdi
    lidt [rax]
    ret
