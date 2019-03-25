#ifndef _LIBK_STDBOOL_H
#define _LIBK_STDBOOL_H

#define USE_BUILT_IN_BOOL 1

#define true 1
#define false 0

#if USE_BUILT_IN_BOOL
#define bool _Bool
#else
#define bool _int
#endif

#endif