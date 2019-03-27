#ifndef _LIBK_STDARG_H
#define _LIBK_STDARG_H

#include <va_list.h>

#if 1

#define va_arg(ap, l) __builtin_va_arg(ap, l)
#define va_end(ap) __builtin_va_end(ap)
#define va_start(ap, last_arg) __builtin_va_start((ap), (last_arg))

#else

#define STACKITEM int

#define VA_SIZE(TYPE) \
    ((sizeof(TYPE) + sizeof(STACKITEM) - 1) & ~(sizeof(STACKITEM) - 1))

/**
 * Begins a variable argument expansion
 */
#define va_start(AP, LASTARG) \
    (AP = ((va_list) & (LASTARG) + VA_SIZE(LASTARG)))

/**
 * Ends a variable argument expansion
 */
#define va_end(AP)

/**
 * Retrieves the next variable interpreted as TYPE
 */
#define va_arg(AP, TYPE) \
    (AP += VA_SIZE(TYPE), *((TYPE *)(AP - VA_SIZE(TYPE))))

#endif

#endif