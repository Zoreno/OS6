/**
 * @file div.c
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-06-22
 * 
 * @brief 
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

#include <stdlib.h>
#include <stdint.h>

static uint32_t divl(uint64_t num, uint64_t den)
{
    uint32_t num_high = num >> 32;
    uint32_t num_low = num;
    uint32_t quot;
    uint32_t rem;

    // TODO: Check if num_high and num_low needs to change place
    __asm__("divq %4"
            : "=d"(rem), "=a"(quot)
            : "0"(num_high), "1"(num_low), "rm"(den));

    return quot;
}

static int n_leading_zeroes(uint32_t x)
{
    int n = 0;
    if (x <= 0x0000FFFF)
    {
        n += 16;
        x <<= 16;
    }
    if (x <= 0x00FFFFFF)
    {
        n += 8;
        x <<= 8;
    }
    if (x <= 0x0FFFFFFF)
    {
        n += 4;
        x <<= 4;
    }
    if (x <= 0x7FFFFFFF)
    {
        n++;
    }

    return n;
}

static uint64_t udiv64(uint64_t num, uint64_t den)
{
    if ((den >> 32) == 0)
    {
        uint64_t b = 1ULL << 32;
        uint32_t num_high = num >> 32;
        uint32_t num_low = num;

        return divl(b * (num_high % den) + num_low, den) + b * num_high / den;
    }

    if (num < den)
    {
        return 0;
    }

    uint32_t den_high = den >> 32;

    int s = n_leading_zeroes(den_high);

    uint64_t quot = divl(num >> 1, (den << s) >> 32) >> (31 - s);

    return num - (quot - 1) * den < den ? quot - 1 : quot;
}

static uint64_t umod64(uint64_t num, uint64_t den)
{
    return num - den * udiv64(num, den);
}

static int64_t sdiv64(int64_t num, int64_t den)
{
    uint64_t num_abs = (uint64_t)(num >= 0 ? num : -num);
    uint64_t den_abs = (uint64_t)(den >= 0 ? den : -den);
    uint64_t quot_abs = udiv64(num_abs, den_abs);

    return (num < 0) == (den < 0) ? (int64_t)quot_abs : -(int64_t)quot_abs;
}

static int64_t smod64(int64_t num, int64_t den)
{
    return num - den * sdiv64(num, den);
}

long long __divdi3(long long n, long long d);
long long __moddi3(long long n, long long d);

unsigned long long __udivdi3(unsigned long long n, unsigned long long d);
unsigned long long __umoddi3(unsigned long long n, unsigned long long d);

long long __divdi3(long long n, long long d)
{
    return sdiv64(n, d);
}

long long __moddi3(long long n, long long d)
{
    return smod64(n, d);
}

unsigned long long __udivdi3(unsigned long long n, unsigned long long d)
{
    return udiv64(n, d);
}

unsigned long long __umoddi3(unsigned long long n, unsigned long long d)
{
    return umod64(n, d);
}

div_t div(int number, int denom)
{
    div_t ret;

    ret.quot = number / denom;
    ret.rem = number % denom;

    return ret;
}

ldiv_t ldiv(long int number, long int denom)
{
    ldiv_t ret;

    ret.quot = number / denom;
    ret.rem = number % denom;

    return ret;
}

lldiv_t lldiv(long long int number, long long int denom)
{
    lldiv_t ret;

    ret.quot = number / denom;
    ret.rem = number % denom;

    return ret;
}