;;
;; boot.asm
;;
;; Contains routines for taking over the booting process from the bios
;; and launching the kernel.
;;

;;=============================================================================
;; Multiboot header setup
;;=============================================================================

; Sets the multiboot standard to compile for
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
align 8
header_start:

%if mb_ver == 1
    dd MULTIBOOT_MAGIC
    dd MULTIBOOT_FLAGS
    dd MULTIBOOT_CHECKSUM
%elif mb_ver == 2

    ; Multiboot 2 magic number
    dd 0xe85250d6

    ; Flags
    dd 0

    ; Size of the header
    dd header_end - header_start

    ; Checksum
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

; Stack size for the kernel
STACK_SIZE equ 0x20000

; Pointer to the video memory of the debug terminal
VIDEO_MEM equ 0xb8000

; The text color to use
; TODO: Make this a variable and make routines to change it.
TEXT_COLOR equ 0xf0

global start

section .text
bits 32

;;
;; Entry point for the kernel. 
;;
start:
    cli                     ; Disable all interrupts as we do not yet have
                            ; an IDT.

    mov esp, stack_top      ; Set the stack pointer

    push eax
    push ebx

    call clear_screen32

    call check_multiboot    ; Check if multiboot is present
    call check_cpuid        ; Check if the CPUID instruction is present
    call check_long_mode    ; Check if long mode is available

    call setup_page_tables  ; Setup identity mapping with the page tables
    call enable_paging      ; Install the page tables and set the long mode and paging flags.

    call install_GDT        ; Install the GDT

    mov ebx, message
    call print_string_pm32

    pop ebx
    pop eax

before_jump:

    jmp gdt64.code:long_mode_start  ; Jump to 64 bit land!

    mov dword [VIDEO_MEM], 0x2f4b2f4f ; Print OK on the screen.
    hlt

;;=============================================================================
;; Text printing utilities (32 bit mode)
;;=============================================================================

TERM_W equ 80
TERM_H equ 25

;;
;; Clears the screen
;;
clear_screen32:
    pusha                   ; Save all registers
    mov edi, VIDEO_MEM      ; Set the address
    mov al, 0               ; Set the character to store
    mov ah, TEXT_COLOR      ; Set the text color
    mov ecx, TERM_H * TERM_W    ; Number of words in terminal buffer
    rep stosw               ; Repeat the store operation for the full terminal,
                            ; storing ax to edi. Repeats ecx times, increasing
                            ; edi each time.
    popa                    ; Restore registers
    ret                     ; Return

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

    cmp al, 0x12            ; Check if the current character is a new line character
    je .newline32           ; Jump directly to the new line block

    cmp al, '\'             ; Check for escape character
    je .handle_escape32 

.output_normal32:
    mov [edx], ax           ; Store the character in video memory
    jmp .output_done32

.handle_escape32:
    inc ebx                 ; Go to the next character

    mov al, [ebx]           ; Get the current character from the string            
    mov ah, TEXT_COLOR      ; Set the text coloring

    cmp al, 'n'             ; New line
    jmp .newline32

    jmp .output_normal32    ; Malformed escape, just continue printing for now

.output_done32:    
    mov eax, dword [xpos]   ; Get the current x position
    inc eax                 ; Increase it by one
    mov dword [xpos], eax   ; Store it back

    cmp eax, TERM_W         ; Check if it is below the width of the terminal
    jb .newline_done32      ; If it is, jump past the newline block

.newline32:
    mov dword [xpos], 0     ; Reset the x position  

    mov eax, dword [ypos]   ; Get the y position 
    inc eax                 ; Increase it by one

    cmp eax, TERM_H         ; Check if we need to scroll
    jb .no_scroll32

.scroll32:
    mov ecx, TERM_W * (TERM_H - 1) * 2  ; Number of bytes we need to move.
    xor edx, edx            ; Clear edx
.scroll_loop32:
    
    mov eax, dword [VIDEO_MEM + edx + 2 * TERM_W]   ; Move from one line
    mov dword [VIDEO_MEM + edx], eax    ; and store it in the next

    add edx, 4              ; Go to the next character

    cmp edx, ecx            ; Check if we have more characters to move
    jb .scroll_loop32

    jmp .newline_done32

.no_scroll32:
    mov dword [ypos], eax   ; Store it back

.newline_done32:

    inc ebx                 ; Increase the current character counter

    jmp .print_string_pm_loop32

.print_string_pm_done32:
    popa                    ; Restore all registers
    ret                     ; Return

;;=============================================================================
;; Input and output port operations
;;=============================================================================

;;
;; Read from an IO port pushed on the stack. The return value is stored in eax.
;;
inport32:
    mov edx, [esp + 4]      ; Get the port index from the stack
    in al, dx               ; Do the in instruction.
    ret                     ; Return

;;
;; Write to an output port pushed on the stack. The argument to write is also
;; pushed on the stack.
;;
outport32:
    mov edx, [esp + 4]      ; Get the port index.
    mov al, [esp + 8]       ; Get the value to write.
    out dx, al              ; Write the value
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

    mov ebx, mb_message
    call print_string_pm32

    cmp eax, MB_BL_MAGIC; Check if the eax register is set according to the 
                        ; specification
    jne .no_multiboot   ; If not, jump to the error branch

    mov ebx, done_message
    call print_string_pm32

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

    mov ebx, cpuid_message
    call print_string_pm32 

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

    mov ebx, done_message
    call print_string_pm32

    ret                 ; Return

.no_cpuid:              
    mov al, "1"         ; Set the error code to 1
    jmp error           ; Jump to the error block.

;;
;; Check for long mode using the CPUID instruction
;;
check_long_mode:

    mov ebx, long_mode_message
    call print_string_pm32

    mov eax, 0x80000000 ; Argument for cpuid
    cpuid               ; Do the CPUID instruction
    cmp eax, 0x80000001 ; EAX after the CPUID must be at least 0x80000001
    jb .no_long_mode    ; If not, we do not have long mode support

    mov eax, 0x80000001 ; Argument for CPUID
    cpuid               ; Do the CPUID instruction
    test edx, 1 << 29   ; Check the long mode bit
    jz .no_long_mode    ; If it is not set, we do not have long mode support

    mov ebx, done_message
    call print_string_pm32

    ret                 ; Return

.no_long_mode:
    mov al, "2"         ; Set the error code to 2
    jmp error           ; Jump to the error block.

;;=============================================================================
;; Paging setup
;;=============================================================================

PML4E_P equ (1 << 0)
PML4E_RW equ (1 << 1)
PML4E_U equ (1 << 2)
PML4E_PWT equ (1 << 3)
PML4E_PCD equ (1 << 4)
PML4E_A equ (1 << 5)
PML4E_PS equ (1 << 7)

PDPTE_P equ (1 << 0)
PDPTE_RW equ (1 << 1)
PDPTE_U equ (1 << 2)
PDPTE_PWT equ (1 << 3)
PDPTE_PCD equ (1 << 4)
PDPTE_A equ (1 << 5)
PDPTE_PS equ (1 << 7)

PDE_P equ (1 << 0)
PDE_RW equ (1 << 1)
PDE_U equ (1 << 2)
PDE_PWT equ (1 << 3)
PDE_PCD equ (1 << 4)
PDE_A equ (1 << 5)
PDE_D equ (1 << 6)
PDE_PS equ (1 << 7)

;;
;; Setup identity mapping with huge pages
;; 
setup_page_tables:

    mov ebx, page_table_message
    call print_string_pm32

    xor ebx, ebx
    mov eax, p3_table   ; Map the P4 table entry to the P3 table
    or eax, PML4E_P | PML4E_RW  ; Present and writable flags
    mov [p4_table], eax ; Store the result in memory
    mov [p4_table + 4], ebx

    mov eax, p2_table   ; Map the P3 table entry to the P2 table
    or eax, PDPTE_P | PDPTE_RW  ; Present and writable flags
    mov [p3_table], eax ; Store the result in memory
    mov [p3_table + 4], ebx

    mov ecx, 0          ; Counter

.map_p2_table:

    mov eax, 0x200000   ; The size of each page
    mul ecx             ; Multiply with index to get offset into memory
    or eax, PDE_P | PDE_RW | PDE_PS ; Present + writable + huge pages
    mov [p2_table + ecx * 8], eax   ; Store the entry
    xor eax, eax
    mov [p2_table + ecx * 8 + 4], eax ; Null out the rest of the descriptor


    inc ecx             ; Increase the current index
    cmp ecx, 512        ; Check if the index is 512
    jne .map_p2_table   ; If not, jump to the start of the loop

    mov ebx, done_message
    call print_string_pm32

    ret                 ; Return

EFER_MSI equ 0xC0000080

CR0_PE equ (1 << 0)
CR0_MP equ (1 << 1)
CR0_EM equ (1 << 2)
CR0_TS equ (1 << 3)
CR0_ET equ (1 << 4)
CR0_NE equ (1 << 5)
CR0_WP equ (1 << 16)
CR0_AM equ (1 << 18)
CR0_NW equ (1 << 29)
CR0_CD equ (1 << 30)
CR0_PG equ (1 << 31)

CR4_VME equ (1 << 0)
CR4_PVI equ (1 << 1)
CR4_TSD equ (1 << 2)
CR4_DE equ (1 << 3)
CR4_PSE equ (1 << 4)
CR4_PAE equ (1 << 5)
CR4_MCE equ (1 << 6)
CR4_PGE equ (1 << 7)
CR4_PCE equ (1 << 8)
CR4_OSFXSR equ (1 << 9)
CR4_OSXMMEXCPT equ (1 << 10)
CR4_UIMP equ (1 << 11)
CR4_LA57 equ (1 << 12)
CR4_VMXE equ (1 << 13)
CR4_SMXE equ (1 << 14)
CR4_PSGSBASE equ (1 << 16)
CR4_PCIDE equ (1 << 17)
CR4_OSXSAVE equ (1 << 18)
CR4_SMEP equ (1 << 20)
CR4_SMAP equ (1 << 21)
CR4_PKE equ (1 << 22)

EFER_SCE equ (1 << 0)
EFER_LME equ (1 << 8)
EFER_LMA equ (1 << 10)
EFER_NXE equ (1 << 11)
EFER_SVME equ (1 << 12)
EFER_LMSLE equ (1 << 13)
EFER_FFXSR equ (1 << 14)
EFER_TCE equ (1 << 15)

;;
;; Enables paging for the processor, using the tables we have setup
;; 
enable_paging:

    mov ebx, paging_message
    call print_string_pm32

    mov eax, p4_table   ; Get the address of the top level table
    mov cr3, eax        ; Store the table address in CR3

    mov eax, cr4        ; Get the CR4 register
    or eax, CR4_PAE     ; Set the PAE flag
    mov cr4, eax        ; Store the CR4 register

    mov ecx, EFER_MSI   ; Get the EFER MSR
    rdmsr               
    or eax, EFER_LME    ; Set the long mode enable bit
    wrmsr               ; Write the register back

    mov eax, cr0        ; Get the CR0 register
    or eax, CR0_PG      ; Set the paging flag
    mov cr0, eax        ; Store the register back

    mov ebx, done_message
    call print_string_pm32

    ret                 ; Return

;;=============================================================================
;; GDT install
;;=============================================================================

;;
;; Installs the GDT structure
;;
install_GDT:

    mov ebx, install_gdt_message
    call print_string_pm32

    lgdt [gdt64.pointer]    ; Load the 64 bit GDT

    mov ebx, done_message
    call print_string_pm32

    ret

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

    mov r15, rbx

    mov ax, 0           ; Load 0 into all segement registers as they are
    mov ss, ax          ; ignored in 64 bit mode
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    call clear_screen64

    mov rdi, rbx
    mov rbp, 0

    call kernel_main

    mov rax, 0x4f592f412f4b2f4f ; Print OKAY to screen
    mov qword [0xb8000], rax
    hlt

;;=============================================================================
;; Text printing utilities (64 bit mode)
;;=============================================================================

clear_screen64:
    push rdi
    push rax
    push rcx
    mov rdi, VIDEO_MEM  ; Set the address
    mov rax, (TEXT_COLOR << 8) | \
    (TEXT_COLOR << 24) | \
    (TEXT_COLOR << 40) | \
    (TEXT_COLOR << 56)   
    mov rcx, TERM_H * TERM_W / 4    ; Number of qwords in the terminal buffer
    rep stosq               ; Repeat the store operation

    pop rcx
    pop rax
    pop rdi
    ret                     ; Return

;;=============================================================================
;; Initialized memory area 
;;=============================================================================

section .data

message: db 'Preparing to switch to 64 bit mode...', 0x12, 0
xpos: dd 0
ypos: dd 0
mb_message: db 'Checking for multiboot... ', 0
cpuid_message: db 'Checking for CPUID support... ', 0
long_mode_message: db 'Checking for long mode.... ', 0
page_table_message: db 'Setting up page tables... ', 0
paging_message: db 'Enabling paging... ', 0
install_gdt_message: db 'Installing GDT... ', 0
done_message: db 'Done!\n', 0

section .rodata

; TODO: Make this structure more readable
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

global stack_top

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