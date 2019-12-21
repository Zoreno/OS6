/** \addtogroup libsimd 
 *  @{
 */

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

/**
 * @brief Mask for the denormals mode
 * 
 * 
 */
#define _MM_DENORMALS_ZERO_MASK 0x0040

/**
 * @brief Set denormals zero mode on
 * 
 * 
 */
#define _MM_DENORMALS_ZERO_ON 0x0040

/**
 * @brief Set denormals zero mode off
 * 
 * 
 */
#define _MM_DENORMALS_ZERO_OFF 0x0000

/**
 * @brief Sets the current handling of denormal numbers
 * 
 * @param mode The value to set denormals to
 */
#define _MM_SET_DENORMALS_ZERO_MODE(mode) \
    _mm_setcsr((_mm_getcsr() & ~_MM_DENORMALS_ZERO_MASK) | (mode))

/**
 * @brief Gets the current handling of denormal numbers
 * 
 * @return The current denormal number handling mode.
 */
#define _MM_GET_DENORMALS_ZERO_MODE() \
    (_mm_getcsr() & _MM_DENORMALS_ZERO_MASK)

//=============================================================================
// Functions
//=============================================================================

/**
 * @brief Alternatively add and subtract single precision floating point values.
 * 
 * @param __X Left hand side value.
 * @param __Y Right hand side value.
 * 
 * @return Result of the operation.
 */
__always_inline(__m128)
    _mm_addsub_ps(__m128 __X, __m128 __Y)
{
    return (__m128)__builtin_ia32_addsubps((__v4sf)__X, (__v4sf)__Y);
}

/**
 * @brief Horizontally add single precision floating point values.
 * 
 * @param __X Left hand side value.
 * @param __Y Right hand side value.
 * 
 * @return Result of the operation.
 */
__always_inline(__m128)
    _mm_hadd_ps(__m128 __X, __m128 __Y)
{
    return (__m128)__builtin_ia32_haddps((__v4sf)__X, (__v4sf)__Y);
}

/**
 * @brief Horizontally subtract single precision floating point values.
 * 
 * @param __X Left hand side value.
 * @param __Y Right hand side value.
 * 
 * @return Result of the operation.
 */
__always_inline(__m128)
    _mm_hsub_ps(__m128 __X, __m128 __Y)
{
    return (__m128)__builtin_ia32_hsubps((__v4sf)__X, (__v4sf)__Y);
}

/**
 * @brief Duplicate odd-indexed single precision floating point values.
 * 
 * @param __X Source operand.
 * 
 * @return Result of the operation.
 */
__always_inline(__m128)
    _mm_movehdup_ps(__m128 __X)
{
    return (__m128)__builtin_ia32_movshdup((__v4sf)__X);
}

/**
 * @brief Duplicate even-indexed single precision floating point values.
 * 
 * @param __X Source operand.
 * 
 * @return Result of the operation.
 */
__always_inline(__m128)
    _mm_moveldup_ps(__m128 __X)
{
    return (__m128)__builtin_ia32_movsldup((__v4sf)__X);
}

/**
 * @brief Alternatively add and subtract double precision floating point values.
 * 
 * @param __X Left hand side value.
 * @param __Y Right hand side value.
 * 
 * @return Result of the operation.
 */
__always_inline(__m128d)
    _mm_addsub_pd(__m128d __X, __m128d __Y)
{
    return (__m128d)__builtin_ia32_addsubpd((__v2df)__X, (__v2df)__Y);
}

/**
 * @brief Horizontally add double precision floating point values.
 * 
 * @param __X Left hand side value.
 * @param __Y Right hand side value.
 * 
 * @return Result of the operation.
 */
__always_inline(__m128d)
    _mm_hadd_pd(__m128d __X, __m128d __Y)
{
    return (__m128d)__builtin_ia32_haddpd((__v2df)__X, (__v2df)__Y);
}

/**
 * @brief Horizontally subtract double precision floating point values.
 * 
 * @param __X Left hand side value.
 * @param __Y Right hand side value.
 * 
 * @return Result of the operation.
 */
__always_inline(__m128d)
    _mm_hsub_pd(__m128d __X, __m128d __Y)
{
    return (__m128d)__builtin_ia32_hsubpd((__v2df)__X, (__v2df)__Y);
}

/**
 * @brief Load a double precision floating point element from memory into both lanes of result.
 * 
 * @param __P Pointer to memory location.
 * 
 * @return Result of the operation.
 * 
 */
__always_inline(__m128d)
    _mm_loaddup_pd(double const *__P)
{
    return _mm_load1_pd(__P);
}

/**
 * @brief Duplicate odd-indexed double precision floating point values.
 * 
 * @param __X Source operand.
 * 
 * @return Result of the operation.
 */
__always_inline(__m128d)
    _mm_movedup_pd(__m128d __X)
{
    return _mm_shuffle_pd(__X, __X, _MM_SHUFFLE2(0, 0));
}

/**
 * @brief Load 128 bits of integer data from unaligned memory into result.
 * 
 * @param __P Pointer to memory location.
 * 
 * @return Result of the operation.
 */
__always_inline(__m128i)
    _mm_lddqu_si128(__m128i const *__P)
{
    return (__m128i)__builtin_ia32_lddqu((char const *)__P);
}

/**
 * @brief Arm address monitoring hardware unsing the address specified.
 * 
 * @param __P Pointer to store the monitor.
 * @param __E Optional extensions
 * @param __H Optional hints
 */
__always_inline(void)
    _mm_monitor(void const *__P, unsigned int __E, unsigned int __H)
{
    __builtin_ia32_monitor(__P, __E, __H);
}

/**
 * @brief Hint to the processor that it can enter an implementation-dependend optimized state while waiting for an event loop.
 * 
 * @param __E Optional extensions.
 * @param __H Optional hints
 */
__always_inline(void)
    _mm_mwait(unsigned int __E, unsigned int __H)
{
    __builtin_ia32_mwait(__E, __H);
}

#pragma GCC pop_options

#endif

/** @}*/

//=============================================================================
// End of file
//=============================================================================