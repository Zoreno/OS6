#ifndef _LIBK_STDDEF_H
#define _LIBK_STDDEF_H

#include <_size_t.h>
#include <_ptrdiff_t.h>
#include <_null.h>

#define offsetof(type, member) ((size_t)((char *)&(((type *)0)->member) - (char *)0))

#endif