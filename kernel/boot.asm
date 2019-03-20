;;
;; boot.asm
;;
;; Contains routines for taking over the booting process from the bios
;; and launching the kernel.
;;

;;=============================================================================
;; Multiboot header setup
;;=============================================================================

%define mb_ver 2

%if mb_ver == 1

MULTIBOOT_MODULE_ALIGN equ 1<<0
MULTIBOOT_MEMINFO equ 1<<1
MULTIBOOT_MAGIC equ 0x1BADB002
MULTIBOOT_FLAGS equ MULTIBOOT_MODULE_ALIGN | MULTIBOOT_MEMINFO
MULTIBOOT_CHECKSUM equ -(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS)

%elif mb_ver == 2


%endif

; The multiboot header required for the bootloader to 
; identify this image as a kernel compatible with the
; multiboot specification
section .multiboot_header

header_start:

%if mb_ver == 1
    dd MULTIBOOT_MAGIC
    dd MULTIBOOT_FLAGS
    dd MULTIBOOT_CHECKSUM
%elif mb_ver == 2
    dd 0xe85250d6
    dd 0
    dd header_end - header_start
    dd - (0xe85250d6 + 0 + (header_end - header_start))

    ; End tag
    dw 0
    dw 0
    dd 8
%endif
header_end:

;;=============================================================================
;; Kernel start routine
;;=============================================================================

STACK_SIZE equ 0x4000
VIDEO_MEM equ 0xb8000

TEXT_COLOR equ 0x0f

global start

section .text
bits 32

;;
;; Entry point for the kernel. 
;;
start:
    mov esp, stack_top ; Set the stack pointer

    push eax
    push ebx

    call check_multiboot    ; Check if multiboot is present
    call check_cpuid        ; Check if the CPUID instruction is present
    call check_long_mode    ; Check if long mode is available

    call setup_page_tables  ; Setup identity mapping with the page tables
    call enable_paging      ; Install the page tables and set the long mode and paging flags.

    lgdt [gdt64.pointer]    ; Load the 64 bit GDT

    mov ebx, message
    call print_string_pm32

    hlt

    pop ebx
    pop eax

    jmp gdt64.code:long_mode_start  ; Jump to 64 bit land!

    mov dword [VIDEO_MEM], 0x2f4b2f4f ; Print OK on the screen.
    hlt

;;=============================================================================
;; Text printing utilities
;;=============================================================================

TERM_W equ 80
TERM_H equ 25

message: db 'This is a very very long string that should span more than one line in the terminal', 0
xpos: dd 0
ypos: dd 0

;;
;; Prints a string in 32 bit mode
;; Assumes that ebx contains a pointer to a string in memory
;;
print_string_pm32:
    pusha                   ; Save all registers

.print_string_pm_loop32: 

    mov edx, dword [ypos]   ; Store the y position
    mov eax, TERM_W         ; Multiply with the width of the terminal
    mul edx        
    mov edx, eax            ; Result is stored in eax, so move it to edx again
    add edx, dword [xpos]   ; Add the x position
    shl edx, 1              ; Each entry is of size 2 bytes, so multiply with 2
                            ; to get the correct offset.
    add edx, VIDEO_MEM      ; Store the pointer to the video memory
   
    mov al, [ebx]           ; Get the current character from the string
    mov ah, TEXT_COLOR      ; Set the text coloring

    cmp al, 0               ; If the current character is null, we are done
    je .print_string_pm_done32

    cmp al, '\n'
    je .newline32

    mov [edx], ax           ; Store the character in video memory
    inc ebx                 ; Increase the current character counter
    
    mov eax, dword [xpos]   ; Get the current x position
    inc eax                 ; Increase it by one
    mov dword [xpos], eax   ; Store it back

    cmp eax, TERM_W         ; Check if it is below the width of the terminal

    jb .newline_done32      ; If it is, jump past the newline block

.newline32:
    mov dword [xpos], 0     ; Reset the x position  

    mov eax, dword [ypos]   ; Get the y position 
    inc eax                 ; Increase it by one
    mov dword [ypos], eax   ; Store it back

.newline_done32:

    jmp .print_string_pm_loop32

.print_string_pm_done32:
    popa                    ; Restore all registers
    ret                     ; Return

;;=============================================================================
;; Kernel booting routines
;;=============================================================================

;;
;; Prints an error message followd by the error code given in al
;;
error:
    mov dword [0xb8000], 0x4f524f45 ; "ER"
    mov dword [0xb8004], 0x4f3a4f52 ; "R:"
    mov dword [0xb8008], 0x4f204f20 ; "  "
    mov byte  [0xb800a], al
    hlt

;;
;; Multiboot2 bootloaders write this value in the eax register to identify 
;; themselves
;;
MB_BL_MAGIC equ 0x36d76289

;;
;; Check for multiboot support
;;
check_multiboot:
    cmp eax, MB_BL_MAGIC; Check if the eax register is set according to the 
                        ; specification
    jne .no_multiboot   ; If not, jump to the error branch
    ret                 ; Return from the subroutine

.no_multiboot:
    mov al, "0"         ; Set the error code to 0
    jmp error           ; Jump to the error block

EFLAGS_CF equ (1 << 0)
EFLAGS_PF equ (1 << 2)
EFLAGS_AF equ (1 << 4)
EFLAGS_ZF equ (1 << 6)
EFLAGS_SF equ (1 << 7)
EFLAGS_TF equ (1 << 8)
EFLAGS_IF equ (1 << 9)
EFLAGS_DF equ (1 << 10)
EFLAGS_OF equ (1 << 11)
EFLAGS_IOPL equ (1 << 12) | (1 << 13)
EFLAGS_NT equ (1 << 14)
EFLAGS_RF equ (1 << 16)
EFLAGS_VM equ (1 << 17)
EFLAGS_AC equ (1 << 18)
EFLAGS_VIF equ (1 << 19)
EFLAGS_VIP equ (1 << 20)
EFLAGS_ID equ (1 << 21)

;;
;; Check for long mode using CPUID
;;
check_cpuid:
    ;; Check for CPUID support by flipping the ID bit in the FLAGS register
    ;; If it is possible to flip, we have CPUID support

    pushfd              ; Get the FLAGS register by using the stack
    pop eax             
    mov ecx, eax        ; Copy the flags to ECX for later comparison
    xor eax, EFLAGS_ID  ; Flip the ID bit.
    push eax            ; Store the FLAGS register via the stack
    popfd
    pushfd              ; Get the FLAGS register again
    pop eax
    push ecx            ; Restore the old FLAGS value from the ECX register.
    popfd               
    cmp eax, ecx        ; Compare the old and the new FLAGS values
    je .no_cpuid        ; If they are equal, we have no CPUID support
    ret                 ; Return

.no_cpuid:              
    mov al, "1"         ; Set the error code to 1
    jmp error           ; Jump to the error block.

;;
;; Check for long mode using the CPUID instruction
;;
check_long_mode:
    mov eax, 0x80000000 ; Argument for cpuid
    cpuid               ; Do the CPUID instruction
    cmp eax, 0x80000001 ; EAX after the CPUID must be at least 0x80000001
    jb .no_long_mode    ; If not, we do not have long mode support

    mov eax, 0x80000001 ; Argument for CPUID
    cpuid               ; Do the CPUID instruction
    test edx, 1 << 29   ; Check the long mode bit
    jz .no_long_mode    ; If it is not set, we do not have long mode support
    ret                 ; Return

.no_long_mode:
    mov al, "2"         ; Set the error code to 2
    jmp error           ; Jump to the error block.

;;=============================================================================
;; Paging setup
;;=============================================================================

;;
;; Setup identity mapping with huge pages
;; 
setup_page_tables:
    xor ebx, ebx
    mov eax, p3_table   ; Map the P4 table entry to the P3 table
    or eax, 0b11        ; Present and writable flags
    mov [p4_table], eax ; Store the result in memory
    mov [p4_table + 4], ebx

    mov eax, p2_table   ; Map the P3 table entry to the P2 table
    or eax, 0b11        ; Present and writable flags
    mov [p3_table], eax ; Store the result in memory
    mov [p3_table + 4], ebx

    mov ecx, 0          ; Counter

.map_p2_table:

    mov eax, 0x200000   ; The size of each page
    mul ecx             ; Multiply with index to get offset into memory
    or eax, 0b10000011  ; Present + writable + huge pages
    mov [p2_table + ecx * 8], eax ; Store the entry
    xor eax, eax
    mov [p2_table + ecx * 8 + 4], eax ; Null out the rest of the descriptor


    inc ecx             ; Increase the current index
    cmp ecx, 512        ; Check if the index is 512
    jne .map_p2_table   ; If not, jump to the start of the loop

    ret                 ; Return

;;
;; Enables paging for the processor, using the tables we have setup
;; 
enable_paging:
    mov eax, p4_table   ; Get the address of the top level table
    mov cr3, eax        ; Store the table address in CR3

    mov eax, cr4        ; Get the CR4 register
    or eax, 1 << 5      ; Set the PAE flag
    mov cr4, eax        ; Store the CR4 register

    mov ecx, 0xC0000080 ; Get the EFER MSR
    rdmsr               
    or eax, 1 << 8      ; Set the long mode bit
    wrmsr               ; Write the register back

    mov eax, cr0        ; Get the CR0 register
    or eax, 1 << 31     ; Set the paging flag
    mov cr0, eax        ; Store the register back

    ret                 ; Return

;;=============================================================================
;; 64 bit entry point
;;=============================================================================

section .text
bits 64

extern kernel_main

;;
;; The entry point for long mode operation
;;
long_mode_start:

    push rbx
    push rax

    mov ax, 0           ; Load 0 into all segement registers as they are
    mov ss, ax          ; ignored in 64 bit mode
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ;call kernel_main

    mov rax, 0x4f592f412f4b2f4f ; Print OKAY to screen
    mov qword [0xb8000], rax
    hlt

;;=============================================================================
;; Initialized memory area 
;;=============================================================================

section .rodata
gdt64:
    dq 0
.code: equ $ - gdt64
    dq (1 << 43) | (1 << 44) | (1 << 47) | (1 << 53)
.pointer:
    dw $ - gdt64 - 1    ; Size of the GDT
    dq gdt64            ; Pointer to the start

;;=============================================================================
;; Uninitialized memory area
;;=============================================================================

section .bss
align 0x1000

p4_table:
    resb 0x1000
p3_table:
    resb 0x1000
p2_table:
    resb 0x1000

stack_bottom:
    resb STACK_SIZE ; Allocate memory for the stack
stack_top:

;;=============================================================================
;; End of file
;;=============================================================================