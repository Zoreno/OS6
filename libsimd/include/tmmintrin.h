/** \addtogroup libsimd 
 *  @{
 */

/**
 * @file tmmintrin.h
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

#ifndef _LIBSIMD_TMMINTRIN_H
#define _LIBSIMD_TMMINTRIN_H

#include <pmmintrin.h>

#include <__mm_defines.h>

#pragma GCC push_options
#pragma GCC target("ssse3")

//=============================================================================
// Functions
//=============================================================================

/**
 * @brief Horizontally add adjacent pairs of 16 bit integers in a and b
 * 
 * @param __X First operand.
 * @param __Y Second operand.
 * 
 * @return Result of the operation.
 */
__always_inline(__m128i)
    _mm_hadd_epi16(__m128i __X, __m128i __Y)
{
    return (__m128i)__builtin_ia32_phaddw128((__v8hi)__X, (__v8hi)__Y);
}

/**
 * @brief Horizontally add adjacent pairs of 32 bit integers in a and b
 * 
 * @param __X First operand.
 * @param __Y Second operand.
 * 
 * @return Result of the operation.
 */
__always_inline(__m128i)
    _mm_hadd_epi32(__m128i __X, __m128i __Y)
{
    return (__m128i)__builtin_ia32_phaddd128((__v4si)__X, (__v4si)__Y);
}

/**
 * @brief Horizontally add adjacent pairs of 16 bit integers in a and b.
 * 
 * With saturation.
 * 
 * @param __X First operand.
 * @param __Y Second operand.
 * 
 * @return Result of the operation.
 */
__always_inline(__m128i)
    _mm_hadds_epi16(__m128i __X, __m128i __Y)
{
    return (__m128i)__builtin_ia32_phaddsw128((__v8hi)__X, (__v8hi)__Y);
}

/**
 * @brief Horizontally add adjacent pairs of 16 bit integers in a and b
 * 
 * @param __X First operand.
 * @param __Y Second operand.
 * 
 * @return Result of the operation.
 */
__always_inline(__m64)
    _mm_hadd_pi16(__m64 __X, __m64 __Y)
{
    return (__m64)__builtin_ia32_phaddw((__v4hi)__X, (__v4hi)__Y);
}

/**
 * @brief Horizontally add adjacent pairs of 32 bit integers in a and b
 * 
 * @param __X First operand.
 * @param __Y Second operand.
 * 
 * @return Result of the operation.
 */
__always_inline(__m64)
    _mm_hadd_pi32(__m64 __X, __m64 __Y)
{
    return (__m64)__builtin_ia32_phaddd((__v2si)__X, (__v2si)__Y);
}

/**
 * @brief Horizontally add adjacent pairs of 16 bit integers in a and b.
 * 
 * With saturation.
 * 
 * @param __X First operand.
 * @param __Y Second operand.
 * 
 * @return Result of the operation.
 */
__always_inline(__m64)
    _mm_hadds_pi16(__m64 __X, __m64 __Y)
{
    return (__m64)__builtin_ia32_phaddsw((__v4hi)__X, (__v4hi)__Y);
}

/**
 * @brief Horizontally subtract adjacent pairs of 16 bit integers in a and b
 * 
 * @param __X First operand.
 * @param __Y Second operand.
 * 
 * @return Result of the operation.
 */
__always_inline(__m128i)
    _mm_hsub_epi16(__m128i __X, __m128i __Y)
{
    return (__m128i)__builtin_ia32_phsubw128((__v8hi)__X, (__v8hi)__Y);
}

/**
 * @brief Horizontally subtract adjacent pairs of 32 bit integers in a and b
 * 
 * @param __X First operand.
 * @param __Y Second operand.
 * 
 * @return Result of the operation.
 */
__always_inline(__m128i)
    _mm_hsub_epi32(__m128i __X, __m128i __Y)
{
    return (__m128i)__builtin_ia32_phsubd128((__v4si)__X, (__v4si)__Y);
}

/**
 * @brief Horizontally subtract adjacent pairs of 16 bit integers in a and b.
 * 
 * With saturation.
 * 
 * @param __X First operand.
 * @param __Y Second operand.
 * 
 * @return Result of the operation.
 */
__always_inline(__m128i)
    _mm_hsubs_epi16(__m128i __X, __m128i __Y)
{
    return (__m128i)__builtin_ia32_phsubsw128((__v8hi)__X, (__v8hi)__Y);
}

/**
 * @brief Horizontally subtract adjacent pairs of 16 bit integers in a and b
 * 
 * @param __X First operand.
 * @param __Y Second operand.
 * 
 * @return Result of the operation.
 */
__always_inline(__m64)
    _mm_hsub_pi16(__m64 __X, __m64 __Y)
{
    return (__m64)__builtin_ia32_phsubw((__v4hi)__X, (__v4hi)__Y);
}

/**
 * @brief Horizontally subtract adjacent pairs of 32 bit integers in a and b
 * 
 * @param __X First operand.
 * @param __Y Second operand.
 * 
 * @return Result of the operation.
 */
__always_inline(__m64)
    _mm_hsub_pi32(__m64 __X, __m64 __Y)
{
    return (__m64)__builtin_ia32_phsubd((__v2si)__X, (__v2si)__Y);
}

/**
 * @brief Horizontally subtract adjacent pairs of 16 bit integers in a and b.
 * 
 * With saturation.
 * 
 * @param __X First operand.
 * @param __Y Second operand.
 * 
 * @return Result of the operation.
 */
__always_inline(__m64)
    _mm_hsubs_pi16(__m64 __X, __m64 __Y)
{
    return (__m64)__builtin_ia32_phsubsw((__v4hi)__X, (__v4hi)__Y);
}

/**
 * @brief Unsigned-Signed multiplication, horizontal addition and saturation.
 * 
 * Vertically multiply each unsigned 8-bit integer from @__X  with the 
 * corresponding signed 8 bit integer in @__Y, producing intermediate signed 
 * 16 bit integers. Horizontally add adjacent pairs of intermediate signed 
 * 16 bit integers, and pack the saturated results in result.
 * 
 * @param __X First operand.
 * @param __Y Second operand.
 * 
 * @return Result of the operation.
 */
__always_inline(__m128i)
    _mm_maddubs_epi16(__m128i __X, __m128i __Y)
{
    return (__m128i)__builtin_ia32_pmaddubsw128((__v16qi)__X, (__v16qi)__Y);
}

/**
 * @brief Unsigned-Signed multiplication, horizontal addition and saturation.
 * 
 * Vertically multiply each unsigned 8-bit integer from @__X  with the 
 * corresponding signed 8 bit integer in @__Y, producing intermediate signed 
 * 16 bit integers. Horizontally add adjacent pairs of intermediate signed 
 * 16 bit integers, and pack the saturated results in result.
 * 
 * @param __X First operand.
 * @param __Y Second operand.
 * 
 * @return Result of the operation.
 */
__always_inline(__m64)
    _mm_maddubs_pi16(__m64 __X, __m64 __Y)
{
    return (__m64)__builtin_ia32_pmaddubsw((__v8qi)__X, (__v8qi)__Y);
}

/**
 * @brief Multiplicate, truncate and round
 * 
 * Multiply packed 16 bit integers in @__X and @__Y, producing intermediate 
 * signed 32 bit integers. Truncate each intermediate integer to the 18 most 
 * significatont bits, round by adding 1, and store bits[16:1] to result.
 * 
 * @param __X First operand.
 * @param __Y Second operand.
 * 
 * @return Result of the operation.
 */
__always_inline(__m128i)
    _mm_mulhrs_epi16(__m128i __X, __m128i __Y)
{
    return (__m128i)__builtin_ia32_pmulhrsw128((__v8hi)__X, (__v8hi)__Y);
}

/**
 * @brief Multiplicate, truncate and round
 * 
 * Multiply packed 16 bit integers in @__X and @__Y, producing intermediate 
 * signed 32 bit integers. Truncate each intermediate integer to the 18 most 
 * significatont bits, round by adding 1, and store bits[16:1] to result.
 * 
 * @param __X First operand.
 * @param __Y Second operand.
 * 
 * @return Result of the operation.
 */
__always_inline(__m64)
    _mm_mulhrs_pi16(__m64 __X, __m64 __Y)
{
    return (__m64)__builtin_ia32_pmulhrsw((__v4hi)__X, (__v4hi)__Y);
}

/**
 * @brief Shuffle
 * 
 * Shuffle packed 8-bit integers in @__X according to shuffle control mask in 
 * the corresponding 8-bit element of @__Y, and store the results in result.
 * 
 * @param __X First operand.
 * @param __Y Second operand.
 * 
 * @return Result of the operation.
 */
__always_inline(__m128i)
    _mm_shuffle_epi8(__m128i __X, __m128i __Y)
{
    return (__m128i)__builtin_ia32_pshufb128((__v16qi)__X, (__v16qi)__Y);
}

/**
 * @brief Shuffle
 * 
 * Shuffle packed 8-bit integers in @__X according to shuffle control mask in 
 * the corresponding 8-bit element of @__Y, and store the results in result.
 * 
 * @param __X First operand.
 * @param __Y Second operand.
 * 
 * @return Result of the operation.
 */
__always_inline(__m64)
    _mm_shuffle_pi8(__m64 __X, __m64 __Y)
{
    return (__m64)__builtin_ia32_pshufb((__v8qi)__X, (__v8qi)__Y);
}

/**
 * @brief Negate
 * 
 * Negate packed 8-bit integers in @__X when the corresponding signed 8-bit 
 * integer in @__Y is negative, and store the results in result. Element in 
 * result are zeroed out when the corresponding element in @__Y is zero.
 * 
 * @param __X First operand.
 * @param __Y Second operand.
 * 
 * @return Result of the operation.
 */
__always_inline(__m128i)
    _mm_sign_epi8(__m128i __X, __m128i __Y)
{
    return (__m128i)__builtin_ia32_psignb128((__v16qi)__X, (__v16qi)__Y);
}

/**
 * @brief Negate
 * 
 * Negate packed 16-bit integers in @__X when the corresponding signed 16-bit 
 * integer in @__Y is negative, and store the results in result. Element in 
 * result are zeroed out when the corresponding element in @__Y is zero.
 * 
 * @param __X First operand.
 * @param __Y Second operand.
 * 
 * @return Result of the operation.
 */
__always_inline(__m128i)
    _mm_sign_epi16(__m128i __X, __m128i __Y)
{
    return (__m128i)__builtin_ia32_psignw128((__v8hi)__X, (__v8hi)__Y);
}

/**
 * @brief Negate
 * 
 * Negate packed 32-bit integers in @__X when the corresponding signed 32-bit 
 * integer in @__Y is negative, and store the results in result. Element in 
 * result are zeroed out when the corresponding element in @__Y is zero.
 * 
 * @param __X First operand.
 * @param __Y Second operand.
 * 
 * @return Result of the operation.
 */
__always_inline(__m128i)
    _mm_sign_epi32(__m128i __X, __m128i __Y)
{
    return (__m128i)__builtin_ia32_psignd128((__v4si)__X, (__v4si)__Y);
}

/**
 * @brief Negate
 * 
 * Negate packed 8-bit integers in @__X when the corresponding signed 8-bit 
 * integer in @__Y is negative, and store the results in result. Element in 
 * result are zeroed out when the corresponding element in @__Y is zero.
 * 
 * @param __X First operand.
 * @param __Y Second operand.
 * 
 * @return Result of the operation.
 */
__always_inline(__m64)
    _mm_sign_pi8(__m64 __X, __m64 __Y)
{
    return (__m64)__builtin_ia32_psignb((__v8qi)__X, (__v8qi)__Y);
}

/**
 * @brief Negate
 * 
 * Negate packed 16-bit integers in @__X when the corresponding signed 16-bit 
 * integer in @__Y is negative, and store the results in result. Element in 
 * result are zeroed out when the corresponding element in @__Y is zero.
 * 
 * @param __X First operand.
 * @param __Y Second operand.
 * 
 * @return Result of the operation.
 */
__always_inline(__m64)
    _mm_sign_pi16(__m64 __X, __m64 __Y)
{
    return (__m64)__builtin_ia32_psignw((__v4hi)__X, (__v4hi)__Y);
}

/**
 * @brief Negate
 * 
 * Negate packed 32-bit integers in @__X when the corresponding signed 32-bit 
 * integer in @__Y is negative, and store the results in result. Element in 
 * result are zeroed out when the corresponding element in @__Y is zero.
 * 
 * @param __X First operand.
 * @param __Y Second operand.
 * 
 * @return Result of the operation.
 */
__always_inline(__m64)
    _mm_sign_pi32(__m64 __X, __m64 __Y)
{
    return (__m64)__builtin_ia32_psignd((__v2si)__X, (__v2si)__Y);
}

#ifdef __LIBSIMD_OPTIMIZE__

/**
 * @brief Align
 * 
 * Concatenate 16-byte blocks in @__X and @__Y into a 32-byte temporary result, 
 * shift the result right by @__N bytes, and store the low 16 bytes in result.
 * 
 * @param __X First operand.
 * @param __Y Second operand.
 * @param __N Shift count.
 * 
 * @return Result of the operation.
 */
__always_inline(__m128i)
    _mm_alignr_epi8(__m128i __X, __m128i __Y, const int __N)
{
    return (__m128i)__builtin_ia32_palignr128((__v2di)__X,
                                              (__v2di)__Y, __N * 8);
}

/**
 * @brief Align
 * 
 * Concatenate 16-byte blocks in @__X and @__Y into a 32-byte temporary result, 
 * shift the result right by @__N bytes, and store the low 16 bytes in result.
 * 
 * @param __X First operand.
 * @param __Y Second operand.
 * @param __N Shift count.
 * 
 * @return Result of the operation.
 */
__always_inline(__m64)
    _mm_alignr_pi8(__m64 __X, __m64 __Y, const int __N)
{
    return (__m64)__builtin_ia32_palignr((__v1di)__X,
                                         (__v1di)__Y, __N * 8);
}

#else

/**
 * @brief Align
 * 
 * Concatenate 16-byte blocks in @__X and @__Y into a 32-byte temporary result, 
 * shift the result right by @__N bytes, and store the low 16 bytes in result.
 * 
 * @param __X First operand.
 * @param __Y Second operand.
 * @param __N Shift count.
 * 
 * @return Result of the operation.
 */
#define _mm_alignr_epi8(X, Y, N)                              \
    ((__m128i)__builtin_ia32_palignr128((__v2di)(__m128i)(X), \
                                        (__v2di)(__m128i)(Y), \
                                        (int)(N)*8))

/**
 * @brief Align
 * 
 * Concatenate 16-byte blocks in @__X and @__Y into a 32-byte temporary result, 
 * shift the result right by @__N bytes, and store the low 16 bytes in result.
 * 
 * @param __X First operand.
 * @param __Y Second operand.
 * @param __N Shift count.
 * 
 * @return Result of the operation.
 */
#define _mm_alignr_pi8(X, Y, N)                        \
    ((__m64)__builtin_ia32_palignr((__v1di)(__m64)(X), \
                                   (__v1di)(__m64)(Y), \
                                   (int)(N)*8))
#endif

/**
 * @brief Absolute value
 * 
 * Compute the absolute value of packed 8-bit integers in @__X, and store the 
 * unsigned results in result.
 * 
 * @param __X First operand.
 * 
 * @return Result of the operation.
 */
__always_inline(__m128i)
    _mm_abs_epi8(__m128i __X)
{
    return (__m128i)__builtin_ia32_pabsb128((__v16qi)__X);
}

/**
 * @brief Absolute value
 * 
 * Compute the absolute value of packed 16-bit integers in @__X, and store the 
 * unsigned results in result.
 * 
 * @param __X First operand.
 * 
 * @return Result of the operation.
 */
__always_inline(__m128i)
    _mm_abs_epi16(__m128i __X)
{
    return (__m128i)__builtin_ia32_pabsw128((__v8hi)__X);
}

/**
 * @brief Absolute value
 * 
 * Compute the absolute value of packed 32-bit integers in @__X, and store the 
 * unsigned results in result.
 * 
 * @param __X First operand.
 * 
 * @return Result of the operation.
 */
__always_inline(__m128i)
    _mm_abs_epi32(__m128i __X)
{
    return (__m128i)__builtin_ia32_pabsd128((__v4si)__X);
}

/**
 * @brief Absolute value
 * 
 * Compute the absolute value of packed 8-bit integers in @__X, and store the 
 * unsigned results in result.
 * 
 * @param __X First operand.
 * 
 * @return Result of the operation.
 */
__always_inline(__m64)
    _mm_abs_pi8(__m64 __X)
{
    return (__m64)__builtin_ia32_pabsb((__v8qi)__X);
}

/**
 * @brief Absolute value
 * 
 * Compute the absolute value of packed 16-bit integers in @__X, and store the 
 * unsigned results in result.
 * 
 * @param __X First operand.
 * 
 * @return Result of the operation.
 */
__always_inline(__m64)
    _mm_abs_pi16(__m64 __X)
{
    return (__m64)__builtin_ia32_pabsw((__v4hi)__X);
}

/**
 * @brief Absolute value
 * 
 * Compute the absolute value of packed 32-bit integers in @__X, and store the 
 * unsigned results in result.
 * 
 * @param __X First operand.
 * 
 * @return Result of the operation.
 */
__always_inline(__m64)
    _mm_abs_pi32(__m64 __X)
{
    return (__m64)__builtin_ia32_pabsd((__v2si)__X);
}

#pragma GCC pop_options

#endif

/** @}*/

//=============================================================================
// End of file
//=============================================================================
