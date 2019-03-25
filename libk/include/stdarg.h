#ifndef _LIBK_STDARG_H
#define _LIBK_STDARG_H

#include <va_list.h>

#define STACK_ITEM long long int

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