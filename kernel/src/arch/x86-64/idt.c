#include <arch/x86-64/idt.h>

#include <string.h>
#include <stdio.h>
#include <debug/debug_terminal.h>

#include <exec/elf64.h>
#include <debug/backtrace.h>

extern void arch_x86_64_idt_flush(uint64_t idtr_ptr);

typedef struct
{
    uint16_t limit;
    uint64_t base;
} __attribute__((packed)) arch_x86_64_idtr;

arch_x86_64_idt_descriptor _idt[ARCH_X86_64_IDT_MAX_INTERRUPTS];
arch_x86_64_idtr _idtr;

void arch_x86_64_default_irq_handler(system_stack_t *regs);

const char *get_interrupt_name(uint64_t int_no)
{
    switch (int_no)
    {
    case 0:
        return "Divide by zero";
    case 1:
        return "Debug";
    case 2:
        return "Non-maskable interrupt";
    case 3:
        return "Breakpoint";
    case 4:
        return "Overflow";
    case 5:
        return "Bound Range Exceeded";
    case 6:
        return "Invalid Opcode";
    case 7:
        return "Device not available";
    case 8:
        return "Double fault";
    case 9:
        return "Coprocessor segment overrrun";
    case 10:
        return "Invalid TSS";
    case 11:
        return "Segment not present";
    case 12:
        return "Stack-segment fault";
    case 13:
        return "General Protection Fault";
    case 14:
        return "Page fault";
    case 16:
        return "x87 floating point exception";
    case 17:
        return "Alignment check";
    case 18:
        return "Machine check";
    case 19:
        return "SIMD floating point exception";
    case 20:
        return "Virtualization exception";
    case 30:
        return "Security exception";

    // TODO: Update this to a non-hardcoded value
    case 128:
        return "System call";
    default:
        return "Invalid exception";
    }
}

void print_regs(system_stack_t *regs)
{
    debug_terminal_set_back_color(VGA_COLOR_BLUE);
    debug_terminal_set_text_color(VGA_COLOR_WHITE);
    debug_terminal_clear();

    char buf[32];
    const char *int_name;

    if (regs->int_no >= 32 && regs->int_no < 48)
    {
        sprintf(buf, "IRQ%i", regs->int_no - 32);

        int_name = buf;
    }
    else
    {
        int_name = get_interrupt_name(regs->int_no);
    }

    printf("[IRQ] ==========================================================================\n");
    printf("[IRQ] Unhandled Exception %i (Error code: %i) (%s)\n",
           regs->int_no, regs->err_code, int_name);
    printf("[IRQ] Register dump:\n");
    printf("[IRQ] RAX: %#016x RBX: %#016x RCX: %#016x\n",
           regs->rax, regs->rbx, regs->rcx);
    printf("[IRQ] RDX: %#016x RDI: %#016x RSI: %#016x\n",
           regs->rdx, regs->rdi, regs->rsi);
    printf("[IRQ] RBP: %#016x RIP: %#016x\n",
           regs->rbp, regs->rip);

    printf("[IRQ]  R8: %#016x  R9: %#016x R10: %#016x \n",
           regs->r8, regs->r9, regs->r10);
    printf("[IRQ] R11: %#016x R12: %#016x R13: %#016x \n",
           regs->r11, regs->r12, regs->r13);
    printf("[IRQ] R14: %#016x R15: %#016x\n",
           regs->r14, regs->r15);

    printf("[IRQ]  CS: %#016x USERRSP: %#016x SS: %#016x\n",
           regs->cs, regs->userrsp, regs->ss);
    printf("[IRQ] RFLAGS: %064b\n",
           regs->rflags);
    printf("[IRQ] Backtrace: \n");

    kernel_lookup_symbol((void *)regs->rip);

    printf("\n");

    print_backtrace((unsigned long long int *)regs->rbp);

    printf("[IRQ] ==========================================================================\n");
}

static IRQ_HANDLER _irq_handlers[16] = {0};

void arch_x86_64_default_irq_handler(system_stack_t *regs)
{

    if (regs->int_no >= 32 && regs->int_no < 48)
    {

        int irq = regs->int_no - 32;

        if (_irq_handlers[irq])
        {
            _irq_handlers[irq](regs);

            interrupt_done(regs->int_no);

            return;
        }
    }

    print_regs(regs);

    for (;;)
        ;
}

arch_x86_64_idt_descriptor *arch_x86_64_get_ir(uint32_t i)
{
    if (i > ARCH_X86_64_IDT_MAX_INTERRUPTS)
    {
        return NULL;
    }

    return &_idt[i];
}

void arch_x86_64_install_ir(uint32_t i, uint16_t flags, uint16_t sel, INT_HANDLER int_handler)
{
    if (i > ARCH_X86_64_IDT_MAX_INTERRUPTS)
    {
        return;
    }

    if (!int_handler)
    {
        return;
    }

    memset(&_idt[i], 0, sizeof(arch_x86_64_idt_descriptor));

    uint64_t base = (uint64_t) & (*int_handler);

    _idt[i].baseLo = (uint16_t)(base & 0xFFFF);
    _idt[i].baseMid = (uint16_t)((base >> 16) & 0xFFFF);
    _idt[i].flags = (uint8_t)flags;
    _idt[i].baseHi = (uint32_t)((base >> 32) & 0xFFFFFFFF);
    _idt[i].sel = sel;
}

void arch_x86_64_install_irq(int irq, IRQ_HANDLER irq_handler)
{
    _irq_handlers[irq] = irq_handler;
}

extern void arch_x86_64_isr_0(void);
extern void arch_x86_64_isr_1(void);
extern void arch_x86_64_isr_2(void);
extern void arch_x86_64_isr_3(void);
extern void arch_x86_64_isr_4(void);
extern void arch_x86_64_isr_5(void);
extern void arch_x86_64_isr_6(void);
extern void arch_x86_64_isr_7(void);
extern void arch_x86_64_isr_8(void);
extern void arch_x86_64_isr_9(void);
extern void arch_x86_64_isr_10(void);
extern void arch_x86_64_isr_11(void);
extern void arch_x86_64_isr_12(void);
extern void arch_x86_64_isr_13(void);
extern void arch_x86_64_isr_14(void);
extern void arch_x86_64_isr_15(void);
extern void arch_x86_64_isr_16(void);
extern void arch_x86_64_isr_17(void);
extern void arch_x86_64_isr_18(void);
extern void arch_x86_64_isr_19(void);
extern void arch_x86_64_isr_20(void);
extern void arch_x86_64_isr_21(void);
extern void arch_x86_64_isr_22(void);
extern void arch_x86_64_isr_23(void);
extern void arch_x86_64_isr_24(void);
extern void arch_x86_64_isr_25(void);
extern void arch_x86_64_isr_26(void);
extern void arch_x86_64_isr_27(void);
extern void arch_x86_64_isr_28(void);
extern void arch_x86_64_isr_29(void);
extern void arch_x86_64_isr_30(void);
extern void arch_x86_64_isr_31(void);

extern void arch_x86_64_isr_128(void);

extern void arch_x86_64_irq_0(void);
extern void arch_x86_64_irq_1(void);
extern void arch_x86_64_irq_2(void);
extern void arch_x86_64_irq_3(void);
extern void arch_x86_64_irq_4(void);
extern void arch_x86_64_irq_5(void);
extern void arch_x86_64_irq_6(void);
extern void arch_x86_64_irq_7(void);
extern void arch_x86_64_irq_8(void);
extern void arch_x86_64_irq_9(void);
extern void arch_x86_64_irq_10(void);
extern void arch_x86_64_irq_11(void);
extern void arch_x86_64_irq_12(void);
extern void arch_x86_64_irq_13(void);
extern void arch_x86_64_irq_14(void);
extern void arch_x86_64_irq_15(void);

void arch_x86_64_initialize_idt(uint16_t code_sel)
{
    printf("[ARCH] Initializing IDT...\n");

    _idtr.limit = (sizeof(arch_x86_64_idt_descriptor)) * ARCH_X86_64_IDT_MAX_INTERRUPTS - 1;
    _idtr.base = (uint64_t)&_idt[0];

    // Install all hardware exception handlers

    arch_x86_64_install_ir(0, ARCH_X86_64_IDT_DESC_BIT32 | ARCH_X86_64_IDT_DESC_PRESENT, code_sel, arch_x86_64_isr_0);
    arch_x86_64_install_ir(1, ARCH_X86_64_IDT_DESC_BIT32 | ARCH_X86_64_IDT_DESC_PRESENT, code_sel, arch_x86_64_isr_1);
    arch_x86_64_install_ir(2, ARCH_X86_64_IDT_DESC_BIT32 | ARCH_X86_64_IDT_DESC_PRESENT, code_sel, arch_x86_64_isr_2);
    arch_x86_64_install_ir(3, ARCH_X86_64_IDT_DESC_BIT32 | ARCH_X86_64_IDT_DESC_PRESENT, code_sel, arch_x86_64_isr_3);
    arch_x86_64_install_ir(4, ARCH_X86_64_IDT_DESC_BIT32 | ARCH_X86_64_IDT_DESC_PRESENT, code_sel, arch_x86_64_isr_4);
    arch_x86_64_install_ir(5, ARCH_X86_64_IDT_DESC_BIT32 | ARCH_X86_64_IDT_DESC_PRESENT, code_sel, arch_x86_64_isr_5);
    arch_x86_64_install_ir(6, ARCH_X86_64_IDT_DESC_BIT32 | ARCH_X86_64_IDT_DESC_PRESENT, code_sel, arch_x86_64_isr_6);
    arch_x86_64_install_ir(7, ARCH_X86_64_IDT_DESC_BIT32 | ARCH_X86_64_IDT_DESC_PRESENT, code_sel, arch_x86_64_isr_7);
    arch_x86_64_install_ir(8, ARCH_X86_64_IDT_DESC_BIT32 | ARCH_X86_64_IDT_DESC_PRESENT, code_sel, arch_x86_64_isr_8);
    arch_x86_64_install_ir(9, ARCH_X86_64_IDT_DESC_BIT32 | ARCH_X86_64_IDT_DESC_PRESENT, code_sel, arch_x86_64_isr_9);

    arch_x86_64_install_ir(10, ARCH_X86_64_IDT_DESC_BIT32 | ARCH_X86_64_IDT_DESC_PRESENT, code_sel, arch_x86_64_isr_10);
    arch_x86_64_install_ir(11, ARCH_X86_64_IDT_DESC_BIT32 | ARCH_X86_64_IDT_DESC_PRESENT, code_sel, arch_x86_64_isr_11);
    arch_x86_64_install_ir(12, ARCH_X86_64_IDT_DESC_BIT32 | ARCH_X86_64_IDT_DESC_PRESENT, code_sel, arch_x86_64_isr_12);
    arch_x86_64_install_ir(13, ARCH_X86_64_IDT_DESC_BIT32 | ARCH_X86_64_IDT_DESC_PRESENT, code_sel, arch_x86_64_isr_13);
    arch_x86_64_install_ir(14, ARCH_X86_64_IDT_DESC_BIT32 | ARCH_X86_64_IDT_DESC_PRESENT, code_sel, arch_x86_64_isr_14);
    arch_x86_64_install_ir(15, ARCH_X86_64_IDT_DESC_BIT32 | ARCH_X86_64_IDT_DESC_PRESENT, code_sel, arch_x86_64_isr_15);
    arch_x86_64_install_ir(16, ARCH_X86_64_IDT_DESC_BIT32 | ARCH_X86_64_IDT_DESC_PRESENT, code_sel, arch_x86_64_isr_16);
    arch_x86_64_install_ir(17, ARCH_X86_64_IDT_DESC_BIT32 | ARCH_X86_64_IDT_DESC_PRESENT, code_sel, arch_x86_64_isr_17);
    arch_x86_64_install_ir(18, ARCH_X86_64_IDT_DESC_BIT32 | ARCH_X86_64_IDT_DESC_PRESENT, code_sel, arch_x86_64_isr_18);
    arch_x86_64_install_ir(19, ARCH_X86_64_IDT_DESC_BIT32 | ARCH_X86_64_IDT_DESC_PRESENT, code_sel, arch_x86_64_isr_19);

    arch_x86_64_install_ir(20, ARCH_X86_64_IDT_DESC_BIT32 | ARCH_X86_64_IDT_DESC_PRESENT, code_sel, arch_x86_64_isr_20);
    arch_x86_64_install_ir(21, ARCH_X86_64_IDT_DESC_BIT32 | ARCH_X86_64_IDT_DESC_PRESENT, code_sel, arch_x86_64_isr_21);
    arch_x86_64_install_ir(22, ARCH_X86_64_IDT_DESC_BIT32 | ARCH_X86_64_IDT_DESC_PRESENT, code_sel, arch_x86_64_isr_22);
    arch_x86_64_install_ir(23, ARCH_X86_64_IDT_DESC_BIT32 | ARCH_X86_64_IDT_DESC_PRESENT, code_sel, arch_x86_64_isr_23);
    arch_x86_64_install_ir(24, ARCH_X86_64_IDT_DESC_BIT32 | ARCH_X86_64_IDT_DESC_PRESENT, code_sel, arch_x86_64_isr_24);
    arch_x86_64_install_ir(25, ARCH_X86_64_IDT_DESC_BIT32 | ARCH_X86_64_IDT_DESC_PRESENT, code_sel, arch_x86_64_isr_25);
    arch_x86_64_install_ir(26, ARCH_X86_64_IDT_DESC_BIT32 | ARCH_X86_64_IDT_DESC_PRESENT, code_sel, arch_x86_64_isr_26);
    arch_x86_64_install_ir(27, ARCH_X86_64_IDT_DESC_BIT32 | ARCH_X86_64_IDT_DESC_PRESENT, code_sel, arch_x86_64_isr_27);
    arch_x86_64_install_ir(28, ARCH_X86_64_IDT_DESC_BIT32 | ARCH_X86_64_IDT_DESC_PRESENT, code_sel, arch_x86_64_isr_28);
    arch_x86_64_install_ir(29, ARCH_X86_64_IDT_DESC_BIT32 | ARCH_X86_64_IDT_DESC_PRESENT, code_sel, arch_x86_64_isr_29);

    arch_x86_64_install_ir(30, ARCH_X86_64_IDT_DESC_BIT32 | ARCH_X86_64_IDT_DESC_PRESENT, code_sel, arch_x86_64_isr_30);
    arch_x86_64_install_ir(31, ARCH_X86_64_IDT_DESC_BIT32 | ARCH_X86_64_IDT_DESC_PRESENT, code_sel, arch_x86_64_isr_31);

    // Install the System call handler

    arch_x86_64_install_ir(128, ARCH_X86_64_IDT_DESC_BIT32 | ARCH_X86_64_IDT_DESC_PRESENT, code_sel, arch_x86_64_isr_128);

    // TInstall IRQ Handlers

#define IRQ_BASE 32

    arch_x86_64_install_ir(IRQ_BASE + 0, ARCH_X86_64_IDT_DESC_BIT32 | ARCH_X86_64_IDT_DESC_PRESENT, code_sel, arch_x86_64_irq_0);
    arch_x86_64_install_ir(IRQ_BASE + 1, ARCH_X86_64_IDT_DESC_BIT32 | ARCH_X86_64_IDT_DESC_PRESENT, code_sel, arch_x86_64_irq_1);
    arch_x86_64_install_ir(IRQ_BASE + 2, ARCH_X86_64_IDT_DESC_BIT32 | ARCH_X86_64_IDT_DESC_PRESENT, code_sel, arch_x86_64_irq_2);
    arch_x86_64_install_ir(IRQ_BASE + 3, ARCH_X86_64_IDT_DESC_BIT32 | ARCH_X86_64_IDT_DESC_PRESENT, code_sel, arch_x86_64_irq_3);
    arch_x86_64_install_ir(IRQ_BASE + 4, ARCH_X86_64_IDT_DESC_BIT32 | ARCH_X86_64_IDT_DESC_PRESENT, code_sel, arch_x86_64_irq_4);
    arch_x86_64_install_ir(IRQ_BASE + 5, ARCH_X86_64_IDT_DESC_BIT32 | ARCH_X86_64_IDT_DESC_PRESENT, code_sel, arch_x86_64_irq_5);
    arch_x86_64_install_ir(IRQ_BASE + 6, ARCH_X86_64_IDT_DESC_BIT32 | ARCH_X86_64_IDT_DESC_PRESENT, code_sel, arch_x86_64_irq_6);
    arch_x86_64_install_ir(IRQ_BASE + 7, ARCH_X86_64_IDT_DESC_BIT32 | ARCH_X86_64_IDT_DESC_PRESENT, code_sel, arch_x86_64_irq_7);
    arch_x86_64_install_ir(IRQ_BASE + 8, ARCH_X86_64_IDT_DESC_BIT32 | ARCH_X86_64_IDT_DESC_PRESENT, code_sel, arch_x86_64_irq_8);
    arch_x86_64_install_ir(IRQ_BASE + 9, ARCH_X86_64_IDT_DESC_BIT32 | ARCH_X86_64_IDT_DESC_PRESENT, code_sel, arch_x86_64_irq_9);
    arch_x86_64_install_ir(IRQ_BASE + 10, ARCH_X86_64_IDT_DESC_BIT32 | ARCH_X86_64_IDT_DESC_PRESENT, code_sel, arch_x86_64_irq_10);
    arch_x86_64_install_ir(IRQ_BASE + 11, ARCH_X86_64_IDT_DESC_BIT32 | ARCH_X86_64_IDT_DESC_PRESENT, code_sel, arch_x86_64_irq_11);
    arch_x86_64_install_ir(IRQ_BASE + 12, ARCH_X86_64_IDT_DESC_BIT32 | ARCH_X86_64_IDT_DESC_PRESENT, code_sel, arch_x86_64_irq_12);
    arch_x86_64_install_ir(IRQ_BASE + 13, ARCH_X86_64_IDT_DESC_BIT32 | ARCH_X86_64_IDT_DESC_PRESENT, code_sel, arch_x86_64_irq_13);
    arch_x86_64_install_ir(IRQ_BASE + 14, ARCH_X86_64_IDT_DESC_BIT32 | ARCH_X86_64_IDT_DESC_PRESENT, code_sel, arch_x86_64_irq_14);
    arch_x86_64_install_ir(IRQ_BASE + 15, ARCH_X86_64_IDT_DESC_BIT32 | ARCH_X86_64_IDT_DESC_PRESENT, code_sel, arch_x86_64_irq_15);

    arch_x86_64_idt_flush((uint64_t)&_idtr);

    printf("[ARCH] IDT Initialized!\n");
}
