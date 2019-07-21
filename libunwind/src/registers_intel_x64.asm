global __store_registers_intel_x64:function
global __load_registers_intel_x64:function

section .text

__store_registers_intel_x64:
    mov [rdi + 000], rax
    mov [rdi + 008], rbx
    mov [rdi + 016], rcx
    mov [rdi + 024], rdx
    mov [rdi + 032], rdi
    mov [rdi + 040], rsi
    mov [rdi + 048], rbp
    add rsp, 8
    mov [rdi + 056], rsp
    sub rsp, 8
    mov [rdi + 064], r8
    mov [rdi + 072], r9
    mov [rdi + 080], r10
    mov [rdi + 088], r11
    mov [rdi + 096], r12
    mov [rdi + 104], r13
    mov [rdi + 112], r14
    mov [rdi + 120], r15

    mov rax, [rsp]
    mov [rdi + 128], rax

    ret

__load_registers_intel_x64:
    mov rdx, [rdi + 008]
    mov rcx, [rdi + 016]
    mov rbx, [rdi + 024]
    mov rsi, [rdi + 040]
    mov rbp, [rdi + 048]
    mov r8,  [rdi + 064]
    mov r9,  [rdi + 072]
    mov r10, [rdi + 080]
    mov r11, [rdi + 088]
    mov r12, [rdi + 096]
    mov r13, [rdi + 104]
    mov r14, [rdi + 112]
    mov r15, [rdi + 120]

    mov rsp, [rdi + 056]

    mov rax, [rdi + 128]
    push rax

    mov rax, [rdi + 000]
    mov rdi, [rdi + 032]

    ret