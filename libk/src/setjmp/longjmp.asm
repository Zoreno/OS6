global longjmp

bits 64

longjmp:

    mov     rbp, [rdi + 8]
    cli
    mov     rsp, [rdi + 48]
    mov     rax, [rdi + 56]
    push    rax ; RAX clobbered, but that does not matter as we overwrite it later.
    mov     rbx, [rdi + 0]
    mov     r12, [rdi + 16]
    mov     r13, [rdi + 24]
    mov     r14, [rdi + 32]
    mov     r15, [rdi + 40]
    sti  ; TODO: This will always enable interrupts, even if they were disabled
         ; before

    mov     rax, rsi
    ret
