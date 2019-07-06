/**
 * @file pmmintrin.h
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-06-30
 * 
 * @brief SSE3 intrinsics for x86
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

#ifndef _LIBSIMD_PMMINTRIN_H
#define _LIBSIMD_PMMINTRIN_H

#include <emmintrin.h>

#include <__mm_defines.h>

#pragma GCC push_options
#pragma GCC target("sse3")

//=============================================================================
// Type and macros
//=============================================================================

#define _MM_DENORMALS_ZERO_MASK 0x0040
#define _MM_DENORMALS_ZERO_ON 0x0040
#define _MM_DENORMALS_ZERO_OFF 0x0000

#define _MM_SET_DENORMALS_ZERO_MODE(mode) \
    _mm_setcsr((_mm_getcsr() & ~_MM_DENORMALS_ZERO_MASK) | (mode))
#define _MM_GET_DENORMALS_ZERO_MODE() \
    (_mm_getcsr() & _MM_DENORMALS_ZERO_MASK)

//=============================================================================
// Functions
//=============================================================================

__always_inline(__m128)
    _mm_addsub_ps(__m128 __X, __m128 __Y)
{
    return (__m128)__builtin_ia32_addsubps((__v4sf)__X, (__v4sf)__Y);
}

__always_inline(__m128)
    _mm_hadd_ps(__m128 __X, __m128 __Y)
{
    return (__m128)__builtin_ia32_haddps((__v4sf)__X, (__v4sf)__Y);
}

__always_inline(__m128)
    _mm_hsub_ps(__m128 __X, __m128 __Y)
{
    return (__m128)__builtin_ia32_hsubps((__v4sf)__X, (__v4sf)__Y);
}

__always_inline(__m128)
    _mm_movehdup_ps(__m128 __X)
{
    return (__m128)__builtin_ia32_movshdup((__v4sf)__X);
}

__always_inline(__m128)
    _mm_moveldup_ps(__m128 __X)
{
    return (__m128)__builtin_ia32_movsldup((__v4sf)__X);
}

__always_inline(__m128d)
    _mm_addsub_pd(__m128d __X, __m128d __Y)
{
    return (__m128d)__builtin_ia32_addsubpd((__v2df)__X, (__v2df)__Y);
}

__always_inline(__m128d)
    _mm_hadd_pd(__m128d __X, __m128d __Y)
{
    return (__m128d)__builtin_ia32_haddpd((__v2df)__X, (__v2df)__Y);
}

__always_inline(__m128d)
    _mm_hsub_pd(__m128d __X, __m128d __Y)
{
    return (__m128d)__builtin_ia32_hsubpd((__v2df)__X, (__v2df)__Y);
}

__always_inline(__m128d)
    _mm_loaddup_pd(double const *__P)
{
    return _mm_load1_pd(__P);
}

__always_inline(__m128d)
    _mm_movedup_pd(__m128d __X)
{
    return _mm_shuffle_pd(__X, __X, _MM_SHUFFLE2(0, 0));
}

__always_inline(__m128i)
    _mm_lddqu_si128(__m128i const *__P)
{
    return (__m128i)__builtin_ia32_lddqu((char const *)__P);
}

__always_inline(void)
    _mm_monitor(void const *__P, unsigned int __E, unsigned int __H)
{
    __builtin_ia32_monitor(__P, __E, __H);
}

__always_inline(void)
    _mm_mwait(unsigned int __E, unsigned int __H)
{
    __builtin_ia32_mwait(__E, __H);
}

#pragma GCC pop_options

#endif

//=============================================================================
// End of file
//=============================================================================