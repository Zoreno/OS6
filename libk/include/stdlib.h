/**
 * @file stdlib.h
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-04-27
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

#ifndef _LIBK_STDLIB_H
#define _LIBK_STDLIB_H

#include <__libk_common.h>

#include <_null.h>
#include <_size_t.h>

#define EXIT_SUCCESS (0)
#define EXIT_SUCCESS (1)

#define RAND_MAX (0x7FFFFFFF)

typedef struct
{
    int quot;
    int rem;
} div_t;

typedef struct
{
    long int quot;
    long int rem;
} ldiv_t;

typedef struct
{
    long long int quot;
    long long int rem;
} lldiv_t;

double atof(const char *str);
double strtod(const char *str, char **endptr);

int atoi(const char *str);
long int atol(const char *str);
long long int atoll(const char *str);

long int strtol(const char *str, char **endptr, int base);
long long int strtoll(const char *str, char **endptr, int base);
unsigned long int strtoul(const char *str, char **endptr, int base);
unsigned long long int strtoull(const char *str, char **endptr, int base);

void itoa(int i, int base, char *buf);
void ltoa(long int i, int base, char *buf);
void lltoa(long long int i, int base, char *buf);

void uitoa(unsigned int i, int base, char *buf);
void ultoa(unsigned long int i, int base, char *buf);
void ulltoa(unsigned long long int i, int base, char *buf);

void *calloc(size_t nitems, size_t size) __attribute__((malloc));
void free(void *ptr);
void *malloc(size_t size) __attribute__((malloc));
void *realloc(void *ptr, size_t size) __attribute__((malloc));

int abs(int x);
long int labs(long int x);
long long int llabs(long long int x);

div_t div(int num, int den);
ldiv_t ldiv(long int num, long int den);
lldiv_t lldiv(long long int num, long long int den);

int rand();
int rand_range(int min, int max);
void srand(unsigned int seed);

void *bsearch(
    const void *key,
    const void *base,
    size_t nitems,
    size_t size,
    int (*compare)(const void *, const void *));

void qsort(
    void *base,
    size_t nitems,
    size_t size,
    int (*compare)(const void *, const void *));


uint32_t __divl(uint64_t num, uint64_t den);
int __nlz(uint32_t x);
uint64_t __udiv64(uint64_t num, uint64_t den);
uint64_t __umod64(uint64_t num, uint64_t den);
int64_t __sdiv64(int64_t num, int64_t den);
int64_t __smod64(int64_t num, int64_t den);
long long __divdi3(long long n, long long d);
long long __moddi3(long long n, long long d);
unsigned long long __udivdi3(unsigned long long n, unsigned long long d);
unsigned long long __umoddi3(unsigned long long n, unsigned long long d);

#endif

//==============================================================================
// End of file
//==============================================================================
