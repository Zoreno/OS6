#ifndef _LIBK_LIMITS_H
#define _LIBK_LIMITS_H

#define CHAR_BIT 8

#define SCHAR_MIN -127
#define SCHAR_MAX 127
#define UCHAR_MAX 255

#define CHAR_MIN 0
#define CHAR_MAX UCHAR_MAX

#define MB_LEN_MAX 1

#define SHRT_MIN -32767
#define SHRT_MAX 32767
#define USHRT_MAX 65535

#define INT_MIN -32767
#define INT_MAX 32767
#define UINT_MAX 65535

#define LONG_MIN -2147483647
#define LONG_MAX 2147483647
#define ULONG_MAX 4294967295

#define LLONG_MIN -0x8000000000000000
#define LLONG_MAX 0x7fffffffffffffff
#define ULLONG_MAX 0xffffffffffffffff

#endif