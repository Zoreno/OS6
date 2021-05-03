/**
 * @file blockdev.h
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-06-22
 *
 * @brief Block device driver
 *
 * @copyright Copyright (C) 2019,
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https: //www.gnu.org/licenses/>.
 *
 */

#ifndef _BLOCKDEV_H
#define _BLOCKDEV_H

#include <stdint.h>
#include <stddef.h>

typedef uint32_t (*blockdev_read_func_t)(uint32_t, uint32_t, uint32_t, void *);
typedef uint32_t (*blockdev_write_func_t)(uint32_t, uint32_t, uint32_t, void *);

int reg_blockdev_class(uint64_t major,
                       const char *desc,
                       blockdev_read_func_t read,
                       blockdev_write_func_t write);
int reg_blockdev_instance(uint32_t major,
                          uint32_t minor,
                          const char *desc,
                          size_t block_size,
                          size_t capacity);
int unreg_blockdev_instance(uint32_t major, uint32_t minor);

uint32_t blockdev_read(unsigned int major,
                       unsigned int minor,
                       uint32_t offset,
                       size_t len,
                       void *buffer);
uint32_t blockdev_write(unsigned int major,
                        unsigned int minor,
                        uint32_t offset,
                        size_t len,
                        void *buffer);

#endif

//=============================================================================
// End of file
//=============================================================================
