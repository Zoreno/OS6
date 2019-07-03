#ifndef ___LIBK_SYMBOLS_H
#define ___LIBK_SYMBOLS_H

#ifdef _LIBK
#define __typeof(x) __typeof__(x)
#define weak_alias(name, aliasname) _weak_alias(name, aliasname)
#define _weak_alias(name, aliasname) \
	extern __typeof(name) aliasname __attribute__((weak, alias(#name)));
#else
#define weak_alias(name, aliasname)
#endif

#endif