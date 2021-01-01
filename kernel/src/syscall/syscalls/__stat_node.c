/**
 * @file __stat_node.c
 * @author Joakim Bertils
 * @version 0.1
 * @date 2020-08-08
 * 
 * @brief 
 * 
 * @copyright Copyright (C) 2020,
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

#include <syscall/syscall.h>

int __stat_node(fs_node_t *node, uintptr_t addr)
{
    struct stat *f = (struct stat *)addr;

    // TODO: Validate the pointer

    if (!node)
    {
        memset(f, 0, sizeof(struct stat));

        printf("[STAT] Tried to stat a non-existent file\n");

        return -ENOENT;
    }

    f->st_dev = (uint16_t)(((uint64_t)node->device & 0xFFFF0) >> 8);
    f->st_ino = node->inode;

    uint32_t flags = 0;

    if (node->flags & FS_FILE)
    {
        flags |= _IFREG;
    }

    if (node->flags & FS_DIRECTORY)
    {
        flags |= _IFDIR;
    }

    if (node->flags & FS_CHARDEVICE)
    {
        flags |= _IFCHR;
    }

    if (node->flags & FS_BLOCKDEVICE)
    {
        flags |= _IFBLK;
    }

    if (node->flags & FS_PIPE)
    {
        flags |= _IFIFO;
    }

    if (node->flags & FS_SYMLINK)
    {
        flags |= _IFLNK;
    }

    f->st_mode = node->permissions | flags;

    f->st_nlink = node->nlink;
    f->st_uid = node->uid;
    f->st_gid = node->gid;
    f->st_rdev = 0;

    f->st_atime = node->atime;
    f->st_mtime = node->mtime;
    f->st_ctime = node->ctime;

    f->st_blksize = 512;

    if (node->get_size)
    {
        f->st_size = node->get_size(node);
    }
    else
    {
        f->st_size = node->length;
    }

    return 0;
}

//=============================================================================
// End of file
//=============================================================================