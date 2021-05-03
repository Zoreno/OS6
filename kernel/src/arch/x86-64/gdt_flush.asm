global arch_x86_64_gdt_flush

extern _gdtr
extern stack_top

bits 64

arch_x86_64_gdt_flush:
    mov rax, rdi
    lgdt [_gdtr]

    mov ax, 0x08
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ax, 0x10
    mov ds, ax
    mov ss, ax


    mov rcx, qword .reload_cs
    mov rsi, rsp
    
    push rax
    push rsi
    push 2
    push 0x08
    push rcx
    iretq

.reload_cs:
    ret
