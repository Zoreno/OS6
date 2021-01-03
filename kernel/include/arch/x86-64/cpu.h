/**
 * @file cpu.h
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-04-23
 * 
 * @brief CPU Initialization and feature detection
 * 
 * @copyright Copyright (C) 2019,
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https: //www.gnu.org/licenses/>.
 * 
 */

#ifndef _ARCH_X86_64_CPU_H
#define _ARCH_X86_64_CPU_H

#include <stdint.h>

#define CPU_FEAT_SSE3 0
#define CPU_FEAT_PCLMUL 1
#define CPU_FEAT_DTES64 2
#define CPU_FEAT_MONITOR 3
#define CPU_FEAT_DS_CPL 4
#define CPU_FEAT_VMX 5
#define CPU_FEAT_SMX 6
#define CPU_FEAT_EST 7
#define CPU_FEAT_TM2 8
#define CPU_FEAT_SSSE3 9
#define CPU_FEAT_CID 10
#define CPU_FEAT_FMA 11
#define CPU_FEAT_CX16 12
#define CPU_FEAT_ETPRD 13
#define CPU_FEAT_PDCM 14
#define CPU_FEAT_PCIDE 15
#define CPU_FEAT_DCA 16
#define CPU_FEAT_SSE4_1 17
#define CPU_FEAT_SSE4_2 18
#define CPU_FEAT_x2PIC 19
#define CPU_FEAT_MOVBE 20
#define CPU_FEAT_POPCNT 21
#define CPU_FEAT_AES 22
#define CPU_FEAT_XSAVE 23
#define CPU_FEAT_OSXSAVE 24
#define CPU_FEAT_AVX 25
#define CPU_FEAT_F16C 26
#define CPU_FEAT_RDRAND 27

#define CPU_FEAT_FPU 28
#define CPU_FEAT_VME 29
#define CPU_FEAT_DE 30
#define CPU_FEAT_PSE 31
#define CPU_FEAT_TSC 32
#define CPU_FEAT_MSR 33
#define CPU_FEAT_PAE 34
#define CPU_FEAT_MCE 35
#define CPU_FEAT_CX8 36
#define CPU_FEAT_APIC 37
#define CPU_FEAT_SEP 38
#define CPU_FEAT_MTRR 39
#define CPU_FEAT_PGE 40
#define CPU_FEAT_MCA 41
#define CPU_FEAT_CMOV 42
#define CPU_FEAT_PAT 43
#define CPU_FEAT_PSE36 44
#define CPU_FEAT_PSN 45
#define CPU_FEAT_CLF 46
#define CPU_FEAT_DTES 47
#define CPU_FEAT_ACPI 48
#define CPU_FEAT_MMX 49
#define CPU_FEAT_FXSR 50
#define CPU_FEAT_SSE 51
#define CPU_FEAT_SSE2 52
#define CPU_FEAT_SS 53
#define CPU_FEAT_HTT 54
#define CPU_FEAT_TM1 55
#define CPU_FEAT_IA64 56
#define CPU_FEAT_PBE 57

#define CPU_FEAT_COUNT 58

/**
 * @brief Initializes the CPU
 * 
 * 
 */
void arch_x86_64_initialize_cpu();

/**
 * @brief Shuts down the CPU
 * 
 * 
 */
void arch_x86_64_shutdown_cpu();

/**
 * @brief Gets the CPU vendor.
 * 
 * @return ASCII vendor string.
 */
const char *arch_x86_64_get_cpu_vendor();

/**
 * @brief Gets the CPU brand.
 * 
 * @return ASCII brand string.
 */
const char *arch_x86_64_get_cpu_brand();

/**
 * @brief Gets the number of cache descriptors.
 * 
 * @return Number of cache descriptors.
 */
uint32_t arch_x86_64_get_cpu_cache_descriptor_count();

/**
 * @brief Checks if the CPU has been identified.
 * 
 * @return Non-zero if true.
 */
int arch_x86_64_cpu_identified();

/**
 * @brief Checks if a feature is present on the CPU
 * 
 * @param feature Feature number
 * 
 * @return Non-zero if present
 */
int arch_x86_64_cpu_query_feature(int feature);

/**
 * @brief Returns the name of the feature represented by the feature number.
 * 
 * @param feature Feature number
 * 
 * @return Pointer to statically allocated string.
 */
const char *arch_x86_64_cpu_get_feature_name(int feature);

#endif

//=============================================================================
// End of file
//=============================================================================