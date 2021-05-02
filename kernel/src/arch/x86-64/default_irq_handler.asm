bits 64

extern arch_x86_64_default_irq_handler

%macro pushall 0
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
%endmacro

%macro popall 0
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
%endmacro


%macro DEF_ISR_HANDLER 1
global arch_x86_64_isr_%1
arch_x86_64_isr_%1:
    cli
    push 0
    push %1
    jmp arch_x86_64_isr_common_handler

%endmacro

%macro DEF_ISR_HANDLER_ERC 1
global arch_x86_64_isr_%1
arch_x86_64_isr_%1:
    cli
    push %1
    jmp arch_x86_64_isr_common_handler

%endmacro

DEF_ISR_HANDLER 0
DEF_ISR_HANDLER 1
DEF_ISR_HANDLER 2
DEF_ISR_HANDLER 3
DEF_ISR_HANDLER 4
DEF_ISR_HANDLER 5
DEF_ISR_HANDLER 6
DEF_ISR_HANDLER 7
DEF_ISR_HANDLER_ERC 8
DEF_ISR_HANDLER 9
DEF_ISR_HANDLER_ERC 10
DEF_ISR_HANDLER_ERC 11
DEF_ISR_HANDLER_ERC 12
DEF_ISR_HANDLER_ERC 13
DEF_ISR_HANDLER_ERC 14
DEF_ISR_HANDLER 15
DEF_ISR_HANDLER 16
DEF_ISR_HANDLER_ERC 17
DEF_ISR_HANDLER 18
DEF_ISR_HANDLER 19
DEF_ISR_HANDLER 20
DEF_ISR_HANDLER 21
DEF_ISR_HANDLER 22
DEF_ISR_HANDLER 23
DEF_ISR_HANDLER 24
DEF_ISR_HANDLER 25
DEF_ISR_HANDLER 26
DEF_ISR_HANDLER 27
DEF_ISR_HANDLER 28
DEF_ISR_HANDLER 29
DEF_ISR_HANDLER_ERC 30
DEF_ISR_HANDLER 31

DEF_ISR_HANDLER 128

arch_x86_64_isr_common_handler:
    pushall
    mov rdi, rsp
    mov rax, arch_x86_64_default_irq_handler
    cld
    call rax
    popall
    add rsp, 16 ; Make sure to pop the error code and int_no
    sti
    iretq

%macro DEF_IRQ_HANDLER 1
global arch_x86_64_irq_%1
arch_x86_64_irq_%1:
    cli
    push 0
    push %1+32
    jmp arch_x86_64_irq_common_handler

%endmacro

DEF_IRQ_HANDLER 0
DEF_IRQ_HANDLER 1
DEF_IRQ_HANDLER 2
DEF_IRQ_HANDLER 3
DEF_IRQ_HANDLER 4
DEF_IRQ_HANDLER 5
DEF_IRQ_HANDLER 6
DEF_IRQ_HANDLER 7
DEF_IRQ_HANDLER 8
DEF_IRQ_HANDLER 9
DEF_IRQ_HANDLER 10
DEF_IRQ_HANDLER 11
DEF_IRQ_HANDLER 12
DEF_IRQ_HANDLER 13
DEF_IRQ_HANDLER 14
DEF_IRQ_HANDLER 15

arch_x86_64_irq_common_handler:
    pushall
    mov rdi, rsp
    mov rax, arch_x86_64_default_irq_handler
    cld
    call rax
    popall
    add rsp, 16 ; Make sure to pop the error code and int_no
    sti
    iretq
