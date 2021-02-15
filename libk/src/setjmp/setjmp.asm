global setjmp

bits 64

setjmp:
    mov     [rdi + 0], rbx
    mov     [rdi + 8], rbp
    mov     [rdi +16], r12
    mov     [rdi +24], r13
    mov     [rdi +32], r14
    mov     [rdi +40], r15
    lea     rax, [rsp + 8]
    mov     [rdi +48], rax
    mov     rax, [rsp]
    mov     [rdi + 56], rax
    mov     rax, 0 
    ret