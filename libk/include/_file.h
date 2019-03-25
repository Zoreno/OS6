#ifndef _LIBK__FILE_H
#define _LIBK__FILE_H

#include <stdint.h>

typedef char (*read_function)(void);
typedef void (*write_function)(void);

typedef struct
{
    char name[32];
    uint32_t flags;
    uint32_t fileLength;

    read_function read;
    write_function write;
} FILE;

#endif