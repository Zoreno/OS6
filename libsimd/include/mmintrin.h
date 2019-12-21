/** \addtogroup libsimd 
 *  @{
 */

/**
 * @file mmintrin.h
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-06-30
 * 
 * @brief MMX intrinsics for x86
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

#ifndef _LIBSIMD_MMINTRIN_H
#define _LIBSIMD_MMINTRIN_H

// TODO: Find the real architecture and SIMD support
#define __LIBSIMD_X64 1

#pragma GCC target("sse2")

#include <__mm_defines.h>

//=============================================================================
// Type definitions
//=============================================================================

typedef int __m64 __ATTR((__VSIZE(8), __ALIAS));
typedef int __m64_u __ATTR((__VSIZE(8), __ALIAS, __ALIGN(1)));

typedef int __v2si __ATTR((__VSIZE(8)));
typedef short __v4hi __ATTR((__VSIZE(8)));
typedef char __v8qi __ATTR((__VSIZE(8)));
typedef long long __v1di __ATTR((__VSIZE(8)));
typedef float __v2sf __ATTR((__VSIZE(8)));

//=============================================================================
// Clear state
//=============================================================================

/**
 * @brief Empty the MMX state
 * 
 * 
 */
__always_inline(void) _mm_empty()
{
    __builtin_ia32_emms();
}

/**
 * @brief Empty the MMX state
 * 
 * 
 */
__always_inline(void) _m_empty()
{
    _mm_empty();
}

//=============================================================================
// Convert int to __m64
//=============================================================================

__always_inline(__m64) _mm_cvtsi32_si64(int __i)
{
    return (__m64)__builtin_ia32_vec_init_v2si(__i, 0);
}

__always_inline(__m64) _m_from_int(int __i)
{
    return _mm_cvtsi32_si64(__i);
}

#ifdef __LIBSIMD_X64

__always_inline(__m64) _m_from_int64(long long __i)
{
    return (__m64)__i;
}

__always_inline(__m64) _mm_cvtsi64_m64(long long __i)
{
    return (__m64)__i;
}

__always_inline(__m64) _mm_cvtsi64x_si64(long long __i)
{
    return (__m64)__i;
}

__always_inline(__m64) _mm_set_pi64x(long long __i)
{
    return (__m64)__i;
}

#endif

//=============================================================================
// Convert __m64 to int
//=============================================================================

__always_inline(int) _mm_cvtsi64_si32(__m64 __i)
{
    return __builtin_ia32_vec_ext_v2si((__v2si)__i, 0);
}

__always_inline(int) _m_to_int(__m64 __i)
{
    return _mm_cvtsi64_si32(__i);
}

#ifdef __LIBSIMD_X64

__always_inline(long long) _m_to_int64(__m64 __i)
{
    return (long long)__i;
}

__always_inline(long long) _mm_cvtm64_si64(__m64 __i)
{
    return (long long)__i;
}

__always_inline(long long) _mm_cvtsi64_si64x(__m64 __i)
{
    return (long long)__i;
}

#endif

//=============================================================================
// Data packing
//=============================================================================

__always_inline(__m64) _mm_packs_pi16(__m64 __m1, __m64 __m2)
{
    return (__m64)__builtin_ia32_packsswb((__v4hi)__m1, (__v4hi)__m2);
}

__always_inline(__m64) _m_packsswb(__m64 __m1, __m64 __m2)
{
    return _mm_packs_pi16(__m1, __m2);
}

__always_inline(__m64) _mm_packs_pi32(__m64 __m1, __m64 __m2)
{
    return (__m64)__builtin_ia32_packssdw((__v2si)__m1, (__v2si)__m2);
}

__always_inline(__m64) _m_packssdw(__m64 __m1, __m64 __m2)
{
    return _mm_packs_pi32(__m1, __m2);
}

__always_inline(__m64) _mm_packs_pu16(__m64 __m1, __m64 __m2)
{
    return (__m64)__builtin_ia32_packuswb((__v4hi)__m1, (__v4hi)__m2);
}

__always_inline(__m64) _m_packuswb(__m64 __m1, __m64 __m2)
{
    return _mm_packs_pu16(__m1, __m2);
}

__always_inline(__m64) _mm_unpackhi_pi8(__m64 __m1, __m64 __m2)
{
    return (__m64)__builtin_ia32_punpckhbw((__v8qi)__m1, (__v8qi)__m2);
}

__always_inline(__m64) _m_punpckhbw(__m64 __m1, __m64 __m2)
{
    return _mm_unpackhi_pi8(__m1, __m2);
}

__always_inline(__m64) _mm_unpackhi_pi16(__m64 __m1, __m64 __m2)
{
    return (__m64)__builtin_ia32_punpckhwd((__v4hi)__m1, (__v4hi)__m2);
}

__always_inline(__m64) _m_punpckhwd(__m64 __m1, __m64 __m2)
{
    return _mm_unpackhi_pi16(__m1, __m2);
}

__always_inline(__m64) _mm_unpackhi_pi32(__m64 __m1, __m64 __m2)
{
    return (__m64)__builtin_ia32_punpckhdq((__v2si)__m1, (__v2si)__m2);
}

__always_inline(__m64) _m_punpckhdq(__m64 __m1, __m64 __m2)
{
    return _mm_unpackhi_pi32(__m1, __m2);
}

__always_inline(__m64) _mm_unpacklo_pi8(__m64 __m1, __m64 __m2)
{
    return (__m64)__builtin_ia32_punpcklbw((__v8qi)__m1, (__v8qi)__m2);
}

__always_inline(__m64) _m_punpcklbw(__m64 __m1, __m64 __m2)
{
    return _mm_unpacklo_pi8(__m1, __m2);
}

__always_inline(__m64) _mm_unpacklo_pi16(__m64 __m1, __m64 __m2)
{
    return (__m64)__builtin_ia32_punpcklwd((__v4hi)__m1, (__v4hi)__m2);
}

__always_inline(__m64) _m_punpcklwd(__m64 __m1, __m64 __m2)
{
    return _mm_unpacklo_pi16(__m1, __m2);
}

__always_inline(__m64) _mm_unpacklo_pi32(__m64 __m1, __m64 __m2)
{
    return (__m64)__builtin_ia32_punpckldq((__v2si)__m1, (__v2si)__m2);
}

__always_inline(__m64) _m_punpckldq(__m64 __m1, __m64 __m2)
{
    return _mm_unpacklo_pi32(__m1, __m2);
}

//=============================================================================
// Addition
//=============================================================================

__always_inline(__m64) _mm_add_pi8(__m64 __m1, __m64 __m2)
{
    return (__m64)__builtin_ia32_paddb((__v8qi)__m1, (__v8qi)__m2);
}

__always_inline(__m64) _m_paddb(__m64 __m1, __m64 __m2)
{
    return _mm_add_pi8(__m1, __m2);
}

__always_inline(__m64) _mm_add_pi16(__m64 __m1, __m64 __m2)
{
    return (__m64)__builtin_ia32_paddw((__v4hi)__m1, (__v4hi)__m2);
}

__always_inline(__m64) _m_paddw(__m64 __m1, __m64 __m2)
{
    return _mm_add_pi16(__m1, __m2);
}

__always_inline(__m64) _mm_add_pi32(__m64 __m1, __m64 __m2)
{
    return (__m64)__builtin_ia32_paddd((__v2si)__m1, (__v2si)__m2);
}

__always_inline(__m64) _m_paddd(__m64 __m1, __m64 __m2)
{
    return _mm_add_pi32(__m1, __m2);
}

__always_inline(__m64) _mm_add_si64(__m64 __m1, __m64 __m2)
{
    return (__m64)__builtin_ia32_paddq((__v1di)__m1, (__v1di)__m2);
}

__always_inline(__m64) _mm_adds_pi8(__m64 __m1, __m64 __m2)
{
    return (__m64)__builtin_ia32_paddsb((__v8qi)__m1, (__v8qi)__m2);
}

__always_inline(__m64) _m_paddsb(__m64 __m1, __m64 __m2)
{
    return _mm_adds_pi8(__m1, __m2);
}

__always_inline(__m64) _mm_adds_pi16(__m64 __m1, __m64 __m2)
{
    return (__m64)__builtin_ia32_paddsw((__v4hi)__m1, (__v4hi)__m2);
}

__always_inline(__m64) _m_paddsw(__m64 __m1, __m64 __m2)
{
    return _mm_adds_pi16(__m1, __m2);
}

__always_inline(__m64) _mm_adds_pu8(__m64 __m1, __m64 __m2)
{
    return (__m64)__builtin_ia32_paddusb((__v8qi)__m1, (__v8qi)__m2);
}

__always_inline(__m64) _m_paddusb(__m64 __m1, __m64 __m2)
{
    return _mm_adds_pu8(__m1, __m2);
}

__always_inline(__m64) _mm_adds_pu16(__m64 __m1, __m64 __m2)
{
    return (__m64)__builtin_ia32_paddusw((__v4hi)__m1, (__v4hi)__m2);
}

__always_inline(__m64) _m_paddusw(__m64 __m1, __m64 __m2)
{
    return _mm_adds_pu16(__m1, __m2);
}

//=============================================================================
// Subtraction
//=============================================================================

__always_inline(__m64) _mm_sub_pi16(__m64 __m1, __m64 __m2)
{
    return (__m64)__builtin_ia32_psubw((__v4hi)__m1, (__v4hi)__m2);
}

__always_inline(__m64) _m_psubw(__m64 __m1, __m64 __m2)
{
    return _mm_sub_pi16(__m1, __m2);
}

__always_inline(__m64) _mm_sub_pi32(__m64 __m1, __m64 __m2)
{
    return (__m64)__builtin_ia32_psubd((__v2si)__m1, (__v2si)__m2);
}

__always_inline(__m64) _m_psubd(__m64 __m1, __m64 __m2)
{
    return _mm_sub_pi32(__m1, __m2);
}

__always_inline(__m64) _mm_sub_si64(__m64 __m1, __m64 __m2)
{
    return (__m64)__builtin_ia32_psubq((__v1di)__m1, (__v1di)__m2);
}

__always_inline(__m64) _mm_subs_pi8(__m64 __m1, __m64 __m2)
{
    return (__m64)__builtin_ia32_psubsb((__v8qi)__m1, (__v8qi)__m2);
}

__always_inline(__m64) _m_psubsb(__m64 __m1, __m64 __m2)
{
    return _mm_subs_pi8(__m1, __m2);
}

__always_inline(__m64) _mm_subs_pi16(__m64 __m1, __m64 __m2)
{
    return (__m64)__builtin_ia32_psubsw((__v4hi)__m1, (__v4hi)__m2);
}

__always_inline(__m64) _m_psubsw(__m64 __m1, __m64 __m2)
{
    return _mm_subs_pi16(__m1, __m2);
}

__always_inline(__m64) _mm_subs_pu8(__m64 __m1, __m64 __m2)
{
    return (__m64)__builtin_ia32_psubusb((__v8qi)__m1, (__v8qi)__m2);
}

__always_inline(__m64) _m_psubusb(__m64 __m1, __m64 __m2)
{
    return _mm_subs_pu8(__m1, __m2);
}

__always_inline(__m64) _mm_subs_pu16(__m64 __m1, __m64 __m2)
{
    return (__m64)__builtin_ia32_psubusw((__v4hi)__m1, (__v4hi)__m2);
}

__always_inline(__m64) _m_psubusw(__m64 __m1, __m64 __m2)
{
    return _mm_subs_pu16(__m1, __m2);
}

__always_inline(__m64) _mm_madd_pi16(__m64 __m1, __m64 __m2)
{
    return (__m64)__builtin_ia32_pmaddwd((__v4hi)__m1, (__v4hi)__m2);
}

__always_inline(__m64) _m_pmaddwd(__m64 __m1, __m64 __m2)
{
    return _mm_madd_pi16(__m1, __m2);
}

__always_inline(__m64) _mm_mulhi_pi16(__m64 __m1, __m64 __m2)
{
    return (__m64)__builtin_ia32_pmulhw((__v4hi)__m1, (__v4hi)__m2);
}

__always_inline(__m64) _m_pmulhw(__m64 __m1, __m64 __m2)
{
    return _mm_mulhi_pi16(__m1, __m2);
}

__always_inline(__m64) _mm_mullo_pi16(__m64 __m1, __m64 __m2)
{
    return (__m64)__builtin_ia32_pmullw((__v4hi)__m1, (__v4hi)__m2);
}

__always_inline(__m64) _m_pmullw(__m64 __m1, __m64 __m2)
{
    return _mm_mullo_pi16(__m1, __m2);
}

__always_inline(__m64) _mm_sll_pi16(__m64 __m, __m64 __count)
{
    return (__m64)__builtin_ia32_psllw((__v4hi)__m, (__v4hi)__count);
}

__always_inline(__m64) _m_psllw(__m64 __m, __m64 __count)
{
    return _mm_sll_pi16(__m, __count);
}

__always_inline(__m64) _mm_slli_pi16(__m64 __m, int __count)
{
    return (__m64)__builtin_ia32_psllwi((__v4hi)__m, __count);
}

__always_inline(__m64) _m_psllwi(__m64 __m, int __count)
{
    return _mm_slli_pi16(__m, __count);
}

__always_inline(__m64) _mm_sll_pi32(__m64 __m, __m64 __count)
{
    return (__m64)__builtin_ia32_pslld((__v2si)__m, (__v2si)__count);
}

__always_inline(__m64) _m_pslld(__m64 __m, __m64 __count)
{
    return _mm_sll_pi32(__m, __count);
}

__always_inline(__m64) _mm_slli_pi32(__m64 __m, int __count)
{
    return (__m64)__builtin_ia32_pslldi((__v2si)__m, __count);
}

__always_inline(__m64) _m_pslldi(__m64 __m, int __count)
{
    return _mm_slli_pi32(__m, __count);
}

__always_inline(__m64) _mm_sll_si64(__m64 __m, __m64 __count)
{
    return (__m64)__builtin_ia32_psllq((__v1di)__m, (__v1di)__count);
}

__always_inline(__m64) _m_psllq(__m64 __m, __m64 __count)
{
    return _mm_sll_si64(__m, __count);
}

__always_inline(__m64) _mm_slli_si64(__m64 __m, int __count)
{
    return (__m64)__builtin_ia32_psllqi((__v1di)__m, __count);
}

__always_inline(__m64) _m_psllqi(__m64 __m, int __count)
{
    return _mm_slli_si64(__m, __count);
}

__always_inline(__m64) _mm_sra_pi16(__m64 __m, __m64 __count)
{
    return (__m64)__builtin_ia32_psraw((__v4hi)__m, (__v4hi)__count);
}

__always_inline(__m64) _m_psraw(__m64 __m, __m64 __count)
{
    return _mm_sra_pi16(__m, __count);
}

__always_inline(__m64) _mm_srai_pi16(__m64 __m, int __count)
{
    return (__m64)__builtin_ia32_psrawi((__v4hi)__m, __count);
}

__always_inline(__m64) _m_psrawi(__m64 __m, int __count)
{
    return _mm_srai_pi16(__m, __count);
}

__always_inline(__m64) _mm_sra_pi32(__m64 __m, __m64 __count)
{
    return (__m64)__builtin_ia32_psrad((__v2si)__m, (__v2si)__count);
}

__always_inline(__m64) _m_psrad(__m64 __m, __m64 __count)
{
    return _mm_sra_pi32(__m, __count);
}

__always_inline(__m64) _mm_srai_pi32(__m64 __m, int __count)
{
    return (__m64)__builtin_ia32_psradi((__v2si)__m, __count);
}

__always_inline(__m64) _m_psradi(__m64 __m, int __count)
{
    return _mm_srai_pi32(__m, __count);
}

__always_inline(__m64) _mm_srl_pi16(__m64 __m, __m64 __count)
{
    return (__m64)__builtin_ia32_psrlw((__v4hi)__m, (__v4hi)__count);
}

__always_inline(__m64) _m_psrlw(__m64 __m, __m64 __count)
{
    return _mm_srl_pi16(__m, __count);
}

__always_inline(__m64) _mm_srli_pi16(__m64 __m, int __count)
{
    return (__m64)__builtin_ia32_psrlwi((__v4hi)__m, __count);
}

__always_inline(__m64) _m_psrlwi(__m64 __m, int __count)
{
    return _mm_srli_pi16(__m, __count);
}

__always_inline(__m64) _mm_srl_pi32(__m64 __m, __m64 __count)
{
    return (__m64)__builtin_ia32_psrld((__v2si)__m, (__v2si)__count);
}

__always_inline(__m64) _m_psrld(__m64 __m, __m64 __count)
{
    return _mm_srl_pi32(__m, __count);
}

__always_inline(__m64) _mm_srli_pi32(__m64 __m, int __count)
{
    return (__m64)__builtin_ia32_psrldi((__v2si)__m, __count);
}

__always_inline(__m64) _m_psrldi(__m64 __m, int __count)
{
    return _mm_srli_pi32(__m, __count);
}

__always_inline(__m64) _mm_srl_si64(__m64 __m, __m64 __count)
{
    return (__m64)__builtin_ia32_psrlq((__v1di)__m, (__v1di)__count);
}

__always_inline(__m64) _m_psrlq(__m64 __m, __m64 __count)
{
    return _mm_srl_si64(__m, __count);
}

__always_inline(__m64) _mm_srli_si64(__m64 __m, int __count)
{
    return (__m64)__builtin_ia32_psrlqi((__v1di)__m, __count);
}

__always_inline(__m64) _m_psrlqi(__m64 __m, int __count)
{
    return _mm_srli_si64(__m, __count);
}

__always_inline(__m64) _mm_and_si64(__m64 __m1, __m64 __m2)
{
    return __builtin_ia32_pand(__m1, __m2);
}

__always_inline(__m64) _m_pand(__m64 __m1, __m64 __m2)
{
    return _mm_and_si64(__m1, __m2);
}

__always_inline(__m64) _mm_andnot_si64(__m64 __m1, __m64 __m2)
{
    return __builtin_ia32_pandn(__m1, __m2);
}

__always_inline(__m64) _m_pandn(__m64 __m1, __m64 __m2)
{
    return _mm_andnot_si64(__m1, __m2);
}

__always_inline(__m64) _mm_or_si64(__m64 __m1, __m64 __m2)
{
    return __builtin_ia32_por(__m1, __m2);
}

__always_inline(__m64) _m_por(__m64 __m1, __m64 __m2)
{
    return _mm_or_si64(__m1, __m2);
}

__always_inline(__m64) _mm_xor_si64(__m64 __m1, __m64 __m2)
{
    return __builtin_ia32_pxor(__m1, __m2);
}

__always_inline(__m64) _m_pxor(__m64 __m1, __m64 __m2)
{
    return _mm_xor_si64(__m1, __m2);
}

__always_inline(__m64) _mm_cmpeq_pi8(__m64 __m1, __m64 __m2)
{
    return (__m64)__builtin_ia32_pcmpeqb((__v8qi)__m1, (__v8qi)__m2);
}

__always_inline(__m64) _m_pcmpeqb(__m64 __m1, __m64 __m2)
{
    return _mm_cmpeq_pi8(__m1, __m2);
}

__always_inline(__m64) _mm_cmpgt_pi8(__m64 __m1, __m64 __m2)
{
    return (__m64)__builtin_ia32_pcmpgtb((__v8qi)__m1, (__v8qi)__m2);
}

__always_inline(__m64) _m_pcmpgtb(__m64 __m1, __m64 __m2)
{
    return _mm_cmpgt_pi8(__m1, __m2);
}

__always_inline(__m64) _mm_cmpeq_pi16(__m64 __m1, __m64 __m2)
{
    return (__m64)__builtin_ia32_pcmpeqw((__v4hi)__m1, (__v4hi)__m2);
}

__always_inline(__m64) _m_pcmpeqw(__m64 __m1, __m64 __m2)
{
    return _mm_cmpeq_pi16(__m1, __m2);
}

__always_inline(__m64) _mm_cmpgt_pi16(__m64 __m1, __m64 __m2)
{
    return (__m64)__builtin_ia32_pcmpgtw((__v4hi)__m1, (__v4hi)__m2);
}

__always_inline(__m64) _m_pcmpgtw(__m64 __m1, __m64 __m2)
{
    return _mm_cmpgt_pi16(__m1, __m2);
}

__always_inline(__m64) _mm_cmpeq_pi32(__m64 __m1, __m64 __m2)
{
    return (__m64)__builtin_ia32_pcmpeqd((__v2si)__m1, (__v2si)__m2);
}

__always_inline(__m64) _m_pcmpeqd(__m64 __m1, __m64 __m2)
{
    return _mm_cmpeq_pi32(__m1, __m2);
}

__always_inline(__m64) _mm_cmpgt_pi32(__m64 __m1, __m64 __m2)
{
    return (__m64)__builtin_ia32_pcmpgtd((__v2si)__m1, (__v2si)__m2);
}

__always_inline(__m64) _m_pcmpgtd(__m64 __m1, __m64 __m2)
{
    return _mm_cmpgt_pi32(__m1, __m2);
}

__always_inline(__m64) _mm_setzero_si64(void)
{
    return (__m64)0LL;
}

__always_inline(__m64) _mm_set_pi32(int __i1, int __i0)
{
    return (__m64)__builtin_ia32_vec_init_v2si(__i0, __i1);
}

__always_inline(__m64) _mm_set_pi16(short __w3, short __w2, short __w1, short __w0)
{
    return (__m64)__builtin_ia32_vec_init_v4hi(__w0, __w1, __w2, __w3);
}

__always_inline(__m64) _mm_set_pi8(char __b7, char __b6, char __b5, char __b4,
                                   char __b3, char __b2, char __b1, char __b0)
{
    return (__m64)__builtin_ia32_vec_init_v8qi(__b0, __b1, __b2, __b3,
                                               __b4, __b5, __b6, __b7);
}

__always_inline(__m64) _mm_setr_pi32(int __i0, int __i1)
{
    return _mm_set_pi32(__i1, __i0);
}

__always_inline(__m64) _mm_setr_pi16(short __w0, short __w1, short __w2, short __w3)
{
    return _mm_set_pi16(__w3, __w2, __w1, __w0);
}

__always_inline(__m64) _mm_setr_pi8(char __b0, char __b1, char __b2, char __b3,
                                    char __b4, char __b5, char __b6, char __b7)
{
    return _mm_set_pi8(__b7, __b6, __b5, __b4, __b3, __b2, __b1, __b0);
}

__always_inline(__m64) _mm_set1_pi32(int __i)
{
    return _mm_set_pi32(__i, __i);
}

__always_inline(__m64) _mm_set1_pi16(short __w)
{
    return _mm_set_pi16(__w, __w, __w, __w);
}

__always_inline(__m64) _mm_set1_pi8(char __b)
{
    return _mm_set_pi8(__b, __b, __b, __b, __b, __b, __b, __b);
}

#endif

/** @}*/

//=============================================================================
// End of file
//=============================================================================