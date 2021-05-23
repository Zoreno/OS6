/**
 * @file cmath.hpp
 * @author Joakim Bertils
 * @version 0.1
 * @date 2020-01-10
 * 
 * @brief 
 * 
 * @copyright Copyright (C) 2020,
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
#ifndef __CMATH_HPP_
#define __CMATH_HPP_

#pragma GCC system_header

#include <bits/c++config.hpp>
#include <bits/cpp_type_traits.hpp>
#include <bits/type_traits.hpp>

#include <math.h>

#undef abs
#undef div
#undef acos
#undef asin
#undef atan
#undef atan2
#undef ceil
#undef cos
#undef cosh
#undef exp
#undef fabs
#undef floor
#undef fmod
#undef frexp
#undef ldexp
#undef log
#undef log10
#undef modf
#undef pow
#undef sin
#undef sinh
#undef sqrt
#undef tan
#undef tanh

#undef fpclassify
#undef isfinite
#undef isinf
#undef isnan
#undef isnormal
#undef signbit
#undef isgreater
#undef isgreaterequal
#undef isless
#undef islessequal
#undef islessgreater
#undef isunordered

OS6STD_BEGIN_SYSTEM_HEADER

namespace OS6STD
{

//=============================================================================
// Detail namespace
//=============================================================================

namespace detail
{

template <typename _T, typename _Ret = int>
using __is_arith = typename __enable_if<__is_arithmetic<_T>::__value, _Ret>::__type;

template <typename _T, typename _U, typename _Ret = double>
using __is_int3 = typename __enable_if<__is_integer<_T>::__value && __is_integer<_U>::__value, _Ret>::__type;

template <typename _T, typename _Ret = double>
using __is_int2 = typename __enable_if<__is_integer<_T>::__value, _Ret>::__type;

} // namespace detail

//=============================================================================
// abs
//=============================================================================

double abs(double __x);
float abs(float __x);
long double abs(long double __x);

template <typename _T>
detail::__is_int2<_T> abs(_T __x)
{
    return abs(static_cast<double>(__x));
}

//=============================================================================
// acos
//=============================================================================

double acos(double __x);
float acos(float __x);
long double acos(long double __x);

template <typename _T>
detail::__is_int2<_T> acos(_T __x)
{
    return acos(static_cast<double>(__x));
}

//=============================================================================
// asin
//=============================================================================

double asin(double __x);
float asin(float __x);
long double asin(long double __x);

template <typename _T>
detail::__is_int2<_T> asin(_T __x)
{
    return asin(static_cast<double>(__x));
}

//=============================================================================
// atan
//=============================================================================

double atan(double __x);
float atan(float __x);
long double atan(long double __x);

template <typename _T>
detail::__is_int2<_T> atan(_T __x)
{
    return atan(static_cast<double>(__x));
}

//=============================================================================
// atan2
//=============================================================================

double atan2(double __y, double __x);
float atan2(float __y, double __x);
long double atan2(long double __y, long double __x);

template <typename _T, typename _U>
detail::__is_int3<_T, _U> atan2(_T __x, _U __y)
{
    return atan2(static_cast<double>(__x), static_cast<double>(__y));
}

//=============================================================================
// ceil
//=============================================================================

double ceil(double __x);
float ceil(float __x);
long double ceil(long double __x);

template <typename _T>
detail::__is_int2<_T> ceil(_T __x)
{
    return ceil(static_cast<double>(__x));
}

//=============================================================================
// cos
//=============================================================================

double cos(double __x);
float cos(float __x);
long double cos(long double __x);

template <typename _T>
detail::__is_int2<_T> cos(_T __x)
{
    return cos(static_cast<double>(__x));
}

//=============================================================================
// cosh
//=============================================================================

double cosh(double __x);
float cosh(float __x);
long double cosh(long double __x);

template <typename _T>
detail::__is_int2<_T> cosh(_T __x)
{
    return cosh(static_cast<double>(__x));
}

//=============================================================================
// exp
//=============================================================================

double exp(double __x);
float exp(float __x);
long double exp(long double __x);

template <typename _T>
detail::__is_int2<_T> exp(_T __x)
{
    return exp(static_cast<double>(__x));
}

//=============================================================================
// fabs
//=============================================================================

double fabs(double __x);
float fabs(float __x);
long double fabs(long double __x);

template <typename _T>
detail::__is_int2<_T> fabs(_T __x)
{
    return fabs(static_cast<double>(__x));
}

//=============================================================================
// floor
//=============================================================================

double floor(double __x);
float floor(float __x);
long double floor(long double __x);

template <typename _T>
detail::__is_int2<_T> floor(_T __x)
{
    return floor(static_cast<double>(__x));
}

//=============================================================================
// fmod
//=============================================================================

double fmod(double __x, double __y);
float fmod(float __x, float __y);
long double fmod(long double __x, long double __y);

template <typename _T, typename _U>
detail::__is_int3<_T, _U> fmod(_T __x, _U __y)
{
    return fmod(static_cast<double>(__x), static_cast<double>(__y));
}

//=============================================================================
// frexp
//=============================================================================

double frexp(double __x, int *__exp);
float frexp(float __x, int *__exp);
long double frexp(long double __x, int *__exp);

template <typename _T>
detail::__is_int2<_T> frexp(_T __x, int *__exp)
{
    return frexp(static_cast<double>(__x), __exp);
}

//=============================================================================
// ldexp
//=============================================================================

double ldexp(double __x, int __exp);
float ldexp(float __x, int __exp);
long double ldexp(long double __x, int __exp);

template <typename _T>
detail::__is_int2<_T> ldexp(_T __x, int __exp)
{
    return ldexp(static_cast<double>(__x), __exp);
}

//=============================================================================
// log
//=============================================================================

double log(double __x);
float log(float __x);
long double log(long double __x);

template <typename _T>
detail::__is_int2<_T> log(_T __x)
{
    return log(static_cast<double>(__x));
}

//=============================================================================
// log10
//=============================================================================

double log10(double __x);
float log10(float __x);
long double log10(long double __x);

template <typename _T>
detail::__is_int2<_T> log10(_T __x)
{
    return log10(static_cast<double>(__x));
}

//=============================================================================
// modf
//=============================================================================

double modf(double __x, double *__y);
float modf(float __x, float *__y);
long double modf(long double __x, long double *__y);

template <typename _T, typename _U>
detail::__is_int3<_T, _U> modf(_T __x, _U *__y)
{
    return modf(static_cast<double>(__x), static_cast<double*>(__y));
}

//=============================================================================
// pow
//=============================================================================

double pow(double __x, double __y);
double pow(double __x, int __y);
float pow(float __x, float __y);
float pow(float __x, int __y);
long double pow(long double __x, long double __y);
long double pow(long double __x, int __y);

//=============================================================================
// sin
//=============================================================================

double sin(double __x);
float sin(float __x);
long double sin(long double __x);

template <typename _T>
detail::__is_int2<_T> sin(_T __x)
{
    return sin(static_cast<double>(__x));
}

//=============================================================================
// sinh
//=============================================================================

double sinh(double __x);
float sinh(float __x);
long double sinh(long double __x);

template <typename _T>
detail::__is_int2<_T> sinh(_T __x)
{
    return sinh(static_cast<double>(__x));
}

//=============================================================================
// sqrt
//=============================================================================

double sqrt(double __x);
float sqrt(float __x);
long double sqrt(long double __x);

template <typename _T>
detail::__is_int2<_T> sqrt(_T __x)
{
    return sqrt(static_cast<double>(__x));
}

//=============================================================================
// tan
//=============================================================================

double tan(double __x);
float tan(float __x);
long double tan(long double __x);

template <typename _T>
detail::__is_int2<_T> tan(_T __x)
{
    return tan(static_cast<double>(__x));
}

//=============================================================================
// tanh
//=============================================================================

double tanh(double __x);
float tanh(float __x);
long double tanh(long double __x);

template <typename _T>
detail::__is_int2<_T> tanh(_T __x)
{
    return tanh(static_cast<double>(__x));
}

//=============================================================================
// helper macros
//=============================================================================

#define __cmath_function_unary(name)               \
template <typename _T>                             \
detail::__is_arith<_T> name(_T __f)                \
{                                                  \
    using __type = typename __promote<_T>::__type; \
    return __builtin_##name(__type(__f));          \
}

#define __cmath_function_binary(name)                      \
template <typename _T>                                     \
detail::__is_arith<_T> name(_T __f1, _T __f2)              \
{                                                          \
    using __type = typename __promote<_T>::__type;         \
    return __builtin_##name(__type(__f1), __type(__f2));   \
}

//=============================================================================
// fpclassify
//=============================================================================

template<typename _T>
detail::__is_arith<_T> fpclassify(_T __f)
{
    using __type = typename __promote<_T>::__type;
    return __builtin_fpclassify(FP_NAN,
                                FP_INFINITE,
                                FP_NORMAL,
                                FP_SUBNORMAL,
                                FP_ZERO,
                                __type(__f));
}


//=============================================================================
// isfinite
//=============================================================================

__cmath_function_unary(isfinite)

//=============================================================================
// isinf
//=============================================================================

__cmath_function_unary(isinf)

//=============================================================================
// isnan
//=============================================================================

__cmath_function_unary(isnan)

//=============================================================================
// isnormal
//=============================================================================

__cmath_function_unary(isnormal)

//=============================================================================
// signbit
//=============================================================================

__cmath_function_unary(signbit)

//=============================================================================
// isgreater
//=============================================================================

__cmath_function_binary(isgreater)

//=============================================================================
// isgreaterequal
//=============================================================================

__cmath_function_binary(isgreaterequal)

//=============================================================================
// isless
//=============================================================================

__cmath_function_binary(isless)

//=============================================================================
// islessequal
//=============================================================================

__cmath_function_binary(islessequal)

//=============================================================================
// islessgreater
//=============================================================================

__cmath_function_binary(islessgreater)

//=============================================================================
// isunordered
//=============================================================================

__cmath_function_binary(isunordered)

} // namespace OS6STD

OS6STD_END_SYSTEM_HEADER

#endif

//=============================================================================
// End of file
//=============================================================================
