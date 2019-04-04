#ifndef _BLOCKDEV_H
#define _BLOCKDEV_H

#include <stdint.h>

typedef uint32_t (*blockdev_read_func_t)(uint32_t, uint32_t, uint32_t, void *);
typedef uint32_t (*blockdev_write_func_t)(uint32_t, uint32_t, uint32_t, void *);

int reg_blockdev_class(uint64_t major, const char *desc, blockdev_read_func_t read, blockdev_write_func_t write);
int reg_blockdev_instance(uint32_t major, uint32_t minor, const char *desc, size_t block_size, size_t capacity);
int unreg_blockdev_instance(uint32_t major, uint32_t minor);

uint32_t blockdev_read(unsigned int major, unsigned int minor, uint32_t offset, size_t len, void *buffer);
uint32_t blockdev_write(unsigned int major, unsigned int minor, uint32_t offset, size_t len, void *buffer);

#endif