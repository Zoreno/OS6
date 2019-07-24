#ifndef _LIBUNWIND_ABORT_H
#define _LIBUNWIND_ABORT_H

#include <stdio.h>

// TODO: Proper include
extern "C" void _exit(int ret);

inline void abort(const char *msg, const char *func)
{
    puts("ABORT: [");
    puts(func);
    puts("]\n");
    puts(msg);
    puts("\n");

    _exit(-1);
}

#define ABORT(msg) abort(msg, __func__);

#endif