#ifndef _LIBK_VA_LIST_H
#define _LIBK_VA_LIST_H

#if 1

typedef __builtin_va_list va_list;

#else

typedef unsigned char *va_list;

#endif

#endif