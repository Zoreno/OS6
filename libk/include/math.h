/**
 * @file math.h
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-04-26
 * 
 * @brief Floating point math functions
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

#ifndef _LIBK_MATH_H
#define _LIBK_MATH_H

//==============================================================================
// Types
//==============================================================================

typedef float float_t;
typedef double double_t;

//==============================================================================
// Builtin constants and configuration
//==============================================================================

#define HAVE_BUILTIN_HUGE_VAL 1
#define HAVE_BUILTIN_HUGE_VALF 1
#define HAVE_BUILTIN_INFINITY 1
#define HAVE_BUILTIN_NAN 1
#define HAVE_BUILTIN_NANS 1

#if HAVE_BUILTIN_HUGE_VAL
#define HUGE_VAL (__builtin_huge_val())
#else
#define HUGE_VAL (1e10000);
#endif

#if HAVE_BUILTIN_HUGE_VALF
#define HUGE_VALF (__builtin_huge_valf())
#else
#define HUGE_VALF (1e10000f);
#endif

#if HAVE_BUILTIN_INFINITY
#define INFINITY (__builtin_huge_inff())
#else
#define INFINITY (HUGE_VALF);
#endif

#if HAVE_BUILTIN_NAN
#define NAN (__builtin_huge_nanf())
#else
#define NAN (0.0f / 0.0f);
#endif

#if HAVE_BUILTIN_NANS
#define SNANF (__builtin_nansf(""))
#define SNAN (__builtin_nans(""))
#else
#define SNANF (__nansf(""))
#define SNAN (__nans(""))
#endif

//==============================================================================
// Number classification
//==============================================================================

/**
 * Number categories returned by fpclassify
 */
enum FP_CATEGORIES
{
    /**
     * NAN Category
     */
    FP_NAN = 0,

    /**
     * Infinite Category
     */
    FP_INFINITE = 1,

    /**
     * Zero Category
     */
    FP_ZERO = 2,

    /**
     * Subnormal Category
     */
    FP_SUBNORMAL = 3,

    /**
     * Normal Category
     */
    FP_NORMAL = 4
};

/**
 * @brief Categorizes a number.
 * @param x Number to categorize.
 * @return Number from @FP_CATEGORIES describing the number.
 */
#define fpclassify(x) (__builtin_fpclassify(FP_NAN,       \
                                            FP_INFINITE,  \
                                            FP_NORMAL,    \
                                            FP_SUBNORMAL, \
                                            FP_ZERO,      \
                                            x))

/**
 * @brief Checks the sign of the number.
 * @param x Input number.
 * @return Non-zero if number is negative.
 */
#define signbit(x) (__builtin_signbit(x))

/**
 * @brief Checks if the number is finite.
 * @param x Input number.
 * @return Non-zero if number is not Inf or NaN.
 */
#define isfinite(x) (__builtin_isfinite(x))

/**
 * @brief Checks if the number is normal.
 * @param x Input number.
 * @return Zero if number is zero, subnormal, Inf or NaN.
 */
#define isnormal(x) (__builtin_isnormal(x))

/**
 * @brief Checks if number is NaN.
 * @param x Input number.
 * @return Non-zero if number is NaN.
 */
#define isnan(x) (__builtin_isnan(x))

/**
 * @brief Checks if number is infinite.
 * @param x Input number.
 * @return Non-zero if true.
 */
#define isinf(x) (__builtin_isinf_sign(x))

/**
 * @brief Checks if number is canonical.
 * @param x Input number.
 * @return Non-zero if true.
 */
#define iscanonical(x) ((void)(__typeof(x))(x), 1)

/**
 * @brief Checks if number is signaling NaN.
 * @param x Input number.
 * @return Non-zero if true.
 */
#define issignaling(x) (sizeof(x) == sizeof(float) ? __issignalingf(x) : __issignaling(x))

/**
 * @brief Checks if number is subnormal.
 * @param x Input number.
 * @return Non-zero if true.
 */
#define issubnormal(x) (fpclassify(x) == FP_SUBNORMAL)

/**
 * @brief Checks if number is zero.
 * @param x Input number.
 * @return Non-zero if true.
 */
#define iszero(x) (fpclassify(x) == FP_ZERO)

/**
 * @brief Returns nonzero if x is greater than y
 * @param x First value
 * @param y Second value
 * @return Nonzero if true
 */
#define isgreater(x, y) (__builtin_isgreater(x, y))

/**
 * @brief Returns nonzero if x is greater than or equal to y
 * @param x First value
 * @param y Second value
 * @return Nonzero if true
 */
#define isgreaterequal(x, y) (__builtin_isgreaterequal(x, y))

/**
 * @brief Returns nonzero if x is less than y
 * @param x First value
 * @param y Second value
 * @return Nonzero if true
 */
#define isless(x, y) (__builtin_isless(x, y))

/**
 * @brief Returns nonzero if x is less than or equal to y
 * @param x First value
 * @param y Second value
 * @return Nonzero if true
 */
#define islessequal(x, y) (__builtin_islessequal(x, y))

/**
 * @brief Returns nonzero if x is less than or greater than y
 * @param x First value
 * @param y Second value
 * @return Nonzero if true
 */
#define islessgreater(x, y) (__builtin_islessgreater(x, y))

/**
 * @brief Returns nonzero if u and v are unordered
 * @param u First value
 * @param v Second value
 * @return Nonzero if true
 */
#define isunordered(u, v) (__builtin_isunordered(u, v))

//==============================================================================
// Mathematical constants
//==============================================================================

/**
 * Value of e.
 */
#define M_E 2.7182818284590452354 /* e */

/**
 * Value of log_2(e).
 */
#define M_LOG2E 1.4426950408889634074 /* log_2 e */

/**
 * Value of log_10(e).
 */
#define M_LOG10E 0.43429448190325182765 /* log_10 e */

/**
 * Value of log_e(2).
 */
#define M_LN2 0.69314718055994530942 /* log_e 2 */

/**
 * Value of log_e(10).
 */
#define M_LN10 2.30258509299404568402 /* log_e 10 */

/**
 * Value of pi.
 */
#define M_PI 3.14159265358979323846 /* pi */

/**
 * Value of pi/2.
 */
#define M_PI_2 1.57079632679489661923 /* pi/2 */

/**
 * Value of pi/4.
 */
#define M_PI_4 0.78539816339744830962 /* pi/4 */

/**
 * Value of 1/pi.
 */
#define M_1_PI 0.31830988618379067154 /* 1/pi */

/**
 * Value of 2/pi.
 */
#define M_2_PI 0.63661977236758134308 /* 2/pi */

/**
 * Value of 2/sqrt(pi).
 */
#define M_2_SQRTPI 1.12837916709551257390 /* 2/sqrt(pi) */

/**
 * Value of sqrt(2).
 */
#define M_SQRT2 1.41421356237309504880 /* sqrt(2) */

/**
 * Value of 1/sqrt(2).
 */
#define M_SQRT1_2 0.70710678118654752440 /* 1/sqrt(2) */

/**
 * Long double value of e.
 */
#define M_El 2.718281828459045235360287471352662498L /* e */

/**
 * Long double value of log_2(e).
 */
#define M_LOG2El 1.442695040888963407359924681001892137L /* log_2 e */

/**
 * Long double value of log_10(e).
 */
#define M_LOG10El 0.434294481903251827651128918916605082L /* log_10 e */

/**
 * Long double value of log_e(2).
 */
#define M_LN2l 0.693147180559945309417232121458176568L /* log_e 2 */

/**
 * Long double value of log_e(10).
 */
#define M_LN10l 2.302585092994045684017991454684364208L /* log_e 10 */

/**
 * Long double value of pi.
 */
#define M_PIl 3.141592653589793238462643383279502884L /* pi */

/**
 * Long double value of pi/2.
 */
#define M_PI_2l 1.570796326794896619231321691639751442L /* pi/2 */

/**
 * Long double value of pi/4.
 */
#define M_PI_4l 0.785398163397448309615660845819875721L /* pi/4 */

/**
 * Long double value of 1/pi.
 */
#define M_1_PIl 0.318309886183790671537767526745028724L /* 1/pi */

/**
 * Long double value of 2/pi.
 */
#define M_2_PIl 0.636619772367581343075535053490057448L /* 2/pi */

/**
 * Long double value of 2/sqrt(pi).
 */
#define M_2_SQRTPIl 1.128379167095512573896158903121545172L /* 2/sqrt(pi) */

/**
 * Long double value of sqrt(2).
 */
#define M_SQRT2l 1.414213562373095048801688724209698079L /* sqrt(2) */

/**
 * Long double value of 1/sqrt(2).
 */
#define M_SQRT1_2l 0.707106781186547524400844362104849039L /* 1/sqrt(2) */

//==============================================================================
// Single precision functions
//==============================================================================

float fmaxf(float a, float b);
float fminf(float a, float b);
float ceilf(float a);
float floorf(float a);
float fmodf(float a, float b);
float fabsf(float a);
float fdimf(float a, float b);
float nanf();

float rcpf(float a);
float sqrtf(float a);
float rsqrtf(float a);

float logf(float a);
float log2f(float a);
float log10f(float a);
float logbf(float a, float b);
float powf(float a, float b);
float pow2f(float a);
float expf(float a);

float sinf(float a);
float cosf(float a);
float tanf(float a);
float sinhf(float a);
float coshf(float a);
float tanhf(float a);

float asinf(float a);
float acosf(float a);
float atanf(float a);
float asinhf(float a);
float acoshf(float a);
float atanhf(float a);

float cscf(float a);
float secf(float a);
float cotf(float a);
float cschf(float a);
float sechf(float a);
float cothf(float a);

float acscf(float a);
float asecf(float a);
float acotf(float a);
float acschf(float a);
float asechf(float a);
float acothf(float a);

float lgammaf(float a);
float digammaf(float a);
float sigmoid(float a);

//==============================================================================
// Double precision functions
//==============================================================================

#endif

//==============================================================================
// End of file
//==============================================================================