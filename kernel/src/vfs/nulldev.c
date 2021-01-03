/**
 * @file nulldev.c
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

#include <vfs/nulldev.h>

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <logging/logging.h>
#include <vfs/vfs.h>

static uint32_t read_null(fs_node_t* node, uint64_t offset, uint32_t size, uint8_t* buffer)
{
    return 0;
}

static uint32_t write_null(fs_node_t* node, uint64_t offset, uint32_t size, uint8_t* buffer)
{
    return 0;
}

static void open_null(fs_node_t* node, uint32_t flags)
{
    return;
}

static void close_null(fs_node_t* node)
{
    return;
}

void null_dev_init()
{
    log_info("[NULLDEV] Installing null device");

    fs_node_t* node = malloc(sizeof(fs_node_t));

    if (!node)
    {
        return;
    }

    memset(node, 0, sizeof(fs_node_t));

    node->inode = 0;
    strcpy(node->name, "null");

    node->uid = 0;
    node->gid = 0;

    node->permissions = 0666;
    node->flags = FS_CHARDEVICE | FS_FILE;
    node->read = read_null;
    node->write = write_null;
    node->open = open_null;
    node->close = close_null;
    node->readdir = NULL;
    node->finddir = NULL;
    node->ioctl = NULL;

    vfs_mount("/dev/null", node);

    log_info("[NULLDEV] Done");
}

//=============================================================================
// End of file
//=============================================================================