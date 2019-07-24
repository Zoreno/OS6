#ifndef _LIBC_STDLIB_H
#define _LIBC_STDLIB_H

#include <_cheader.h>

_c_header_begin;

#include <_null.h>
#include <_size_t.h>

#define EXIT_SUCCESS (0)
#define EXIT_SUCCESS (1)

#define RAND_MAX (0x7FFFFFFF)

void *calloc(size_t nitems, size_t size);
void free(void *ptr);
void *malloc(size_t size);
void *realloc(void *ptr, size_t size);

_c_header_end;

#endif