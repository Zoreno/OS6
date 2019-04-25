#ifndef _ARCH_X86_64_FPU_H
#define _ARCH_X86_64_FPU_H

#include <stdint.h>

void arch_x64_64_set_fpu_cw(const uint16_t cw);

void arch_x64_64_enable_fpu();

void arch_x64_64_disable_fpu();

void arch_x64_64_init_fpu();

void arch_x64_64_install_fpu();

#endif