/**
 * @file ramdisk.h
 * @author Joakim Bertils
 * @version 0.1
 * @date 2021-05-02
 *
 * @brief
 *
 * @copyright Copyright (C) 2021,
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

#ifndef _RAMDISK_H
#define _RAMDISK_H

#include <vfs/vfs.h>

#include <stddef.h>

/**
 * @brief Mount a ramdisk in the virtual file system.
 *
 * The disk will appear at /dev/ram<number> where number is an integer starting
 * from zero.
 *
 * @param location The beginning of the ramdisk buffer.
 * @param size Size of the buffer in bytes.
 *
 * @return Pointer to vfs node entry.
 */
fs_node_t *ramdisk_mount(uintptr_t location, size_t size);

#endif

//=============================================================================
// End of file
//=============================================================================
