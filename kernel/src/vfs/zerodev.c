/**
 * @file zerodev.c
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-06-22
 *
 * @brief
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

#include <vfs/zerodev.h>

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <logging/logging.h>
#include <vfs/vfs.h>

//=============================================================================
// Private functions
//=============================================================================

static uint32_t read_zero(fs_node_t *node,
                          uint64_t offset,
                          uint32_t size,
                          uint8_t *buffer)
{
    memset(buffer, 0, size);
    return size;
}

static uint32_t write_zero(fs_node_t *node,
                           uint64_t offset,
                           uint32_t size,
                           uint8_t *buffer)
{
    return 0;
}

static void open_zero(fs_node_t *node, uint32_t flags)
{
    return;
}

static void close_zero(fs_node_t *node)
{
    return;
}

//=============================================================================
// Interface functions
//=============================================================================

void zero_dev_init()
{
    log_info("[ZERODEV] Installing zero device");

    fs_node_t *node = malloc(sizeof(fs_node_t));

    if (!node)
    {
        return;
    }

    memset(node, 0, sizeof(fs_node_t));

    node->inode = 0;
    strcpy(node->name, "zero");

    node->uid = 0;
    node->gid = 0;

    node->permissions = 0666;
    node->flags = FS_CHARDEVICE | FS_FILE;
    node->read = read_zero;
    node->write = write_zero;
    node->open = open_zero;
    node->close = close_zero;
    node->readdir = NULL;
    node->finddir = NULL;
    node->ioctl = NULL;

    vfs_mount("/dev/zero", node);

    log_info("[ZERODEV] Done!");
}

//=============================================================================
// End of file
//=============================================================================
