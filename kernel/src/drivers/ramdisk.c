/**
 * @file ramdisk.c
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

#include <drivers/ramdisk.h>

#include <vfs/vfs.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//=============================================================================
// Private function forward declarations
//=============================================================================

static uint32_t ramdisk_read(fs_node_t *node,
                             uint64_t offset,
                             uint32_t size,
                             uint8_t *buffer);

static uint32_t ramdisk_write(fs_node_t *node,
                              uint64_t offset,
                              uint32_t size,
                              uint8_t *buffer);

static void ramdisk_open(fs_node_t *node, unsigned int flags);
static void ramdisk_close(fs_node_t *node);

static fs_node_t *ramdisk_create(int number, uintptr_t location, size_t size);

//=============================================================================
// Private function implementations
//=============================================================================

static uint32_t ramdisk_read(fs_node_t *node,
                             uint64_t offset,
                             uint32_t size,
                             uint8_t *buffer)
{
    // If requested offset is outside disk.
    if (offset > node->length)
    {
        return 0;
    }

    // If read would be past end of disk.
    if (offset + size > node->length)
    {
        unsigned int i = node->length - offset;
        size = i;
    }

    memcpy(buffer, (void *)(node->inode + (uintptr_t)offset), size);

    return size;
}

static uint32_t ramdisk_write(fs_node_t *node,
                              uint64_t offset,
                              uint32_t size,
                              uint8_t *buffer)
{
    // If requested offset is outside disk.
    if (offset > node->length)
    {
        return 0;
    }

    // If read would be past end of disk.
    if (offset + size > node->length)
    {
        unsigned int i = node->length - offset;
        size = i;
    }

    memcpy((void *)(node->inode + (uintptr_t)offset), buffer, size);

    return size;
}

static void ramdisk_open(fs_node_t *node, unsigned int flags)
{
    (void)node;
    (void)flags;
    return;
}

static void ramdisk_close(fs_node_t *node)
{
    (void)node;
    return;
}

static fs_node_t *ramdisk_create(int number, uintptr_t location, size_t size)
{
    fs_node_t *fnode = malloc(sizeof(fs_node_t));
    memset(fnode, 0, sizeof(fs_node_t));

    fnode->inode = location;

    // TODO: This should use snprintf when possible.
    sprintf(fnode->name, "ram%d", number);
    fnode->uid = 0;
    fnode->gid = 0;
    fnode->length = size;
    fnode->flags = FS_BLOCKDEVICE;
    fnode->read = ramdisk_read;
    fnode->write = ramdisk_write;
    fnode->open = ramdisk_open;
    fnode->close = ramdisk_close;

    return fnode;
}

static int get_next_ramdisk_number()
{
    static int current = 0;

    return current++;
}

//=============================================================================
// Interface functions
//=============================================================================

fs_node_t *ramdisk_mount(uintptr_t location, size_t size)
{
    int number = get_next_ramdisk_number();

    fs_node_t *ramdisk = ramdisk_create(number, location, size);

    if (ramdisk)
    {
        char path[64];
        sprintf(path, "/dev/%s", ramdisk->name);
        vfs_mount(path, ramdisk);
        return ramdisk;
    }

    return NULL;
}

//=============================================================================
// End of file
//=============================================================================
