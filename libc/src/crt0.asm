section .text

global _start

extern pre_main

_start:
	; Set up end of the stack frame linked list.
	mov rbp, 0
	push rbp ; rip=0
	push rbp ; rbp=0
	mov rbp, rsp

	call pre_main