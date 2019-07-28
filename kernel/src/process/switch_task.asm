global switch_to
global set_rsp
global set_rsp_arg1
global get_rsp_val
global put_in_rax
global setup_forked_kthread_stack

bits 64

switch_to:
    push rax
    push rcx
    push rdx
    push rbx
    push rbp
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
    push rdi
    mov r15, [rdi]
    mov [r15], rsp,; first argument is self's task struct in rdi. Save rsp in task_struct->rsp_val, which is at the start
    mov [rdi], rsi,
    mov rsp, [rsi] ; second argument (in rsi) is next task's struct.
    ; at this point, control has switched to different struct's stack. Instructions after this point are execd after
    ; control comes back to current struct
    pop rdi
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rbp
    pop rbx
    pop rdx
    pop rcx
    pop rax
    ret

set_rsp:
    mov rsp, rdi
    ret

set_rsp_arg1:
    mov rsp, rdi
    mov rdi, rsi
    ret

get_rsp_val:
    mov rax, rsp
    ret

put_in_rax:
    mov rax, rdi
    ret

setup_forked_kthread_stack:
    mov DWORD [rdi], 0x0
    mov [rdi - 8], rcx
    mov [rdi - 16], rdx
    mov [rdi - 24], rbx
    mov [rdi - 32], rbp
    mov [rdi - 40], rsi
    mov [rdi - 48], rdi
    mov [rdi - 56], r8
    mov [rdi - 64], r9
    mov [rdi - 72], r10
    mov [rdi - 80], r11
    mov [rdi - 88], r12
    mov [rdi - 96], r13
    mov [rdi - 104], r14
    mov [rdi - 112], r15
    mov [rdi - 120], rdi
    ret