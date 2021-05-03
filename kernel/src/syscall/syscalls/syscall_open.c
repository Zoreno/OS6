/**
 * @file syscall_open.c
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

int syscall_open(const char *file, int flags, int mode)
{
    // Start by opening the file in the VFS
    fs_node_t *node = kopen((char *)file, flags);
    int access_bits = 0;

    // If the node exists and O_CREAT is set together with O_EXCL,
    // the open file operation should fail.
    if (node && (flags & O_CREAT) && (flags & O_EXCL))
    {
        close_fs(node);
        printf("[SYSCALL] Open failed: C_CREAT and O_EXCL both set.\n");
        return -EEXIST;
    }

    // Check user have read rights
    if (!(flags & O_WRONLY) || (flags & O_RDWR))
    {
        if (node && !has_permissions(node, 04))
        {
            printf("Warning: Access denied\n");
            close_fs(node);
            return -EACCES;
        }
        else
        {
            // Set the read bit of the access rights
            access_bits |= 01;
        }
    }

    // Check if the user have write rights.
    if ((flags & O_RDWR) || (flags & O_WRONLY))
    {
        if (node && !has_permissions(node, 02))
        {
            close_fs(node);
            printf("[SYSCALL] Open failed: Write not permitted\n");
            return -EACCES;
        }

        // Cannot write to directory descriptors directly
        if (node && (node->flags & FS_DIRECTORY))
        {
            printf("[SYSCALL] Open failed: Is a directory\n");
            return -EISDIR;
        }

        if ((flags & O_RDWR) || (flags & O_WRONLY))
        {
            // Set the write bit of the access rights.
            access_bits |= 02;
        }
    }

    // Create the file if requested and the file was not found.
    if (!node && (flags & O_CREAT))
    {
        printf("Creating file\n");

        int result = create_file_fs((char *)file, mode);

        if (!result)
        {
            node = kopen((char *)file, flags);
        }
        else
        {
            return result;
        }
    }

    // If the found file is a directory, check if a directory was requested.
    if (node && (flags & O_DIRECTORY))
    {
        if (!(node->flags & FS_DIRECTORY))
        {
            printf("[SYSCALL] Open failed: File is not directory\n");
            return -ENOTDIR;
        }
    }

    // Truncate the file on open if requested
    if (node && (flags & O_TRUNC))
    {
        if (!(access_bits & 02))
        {
            close_fs(node);
            printf("[SYSCALL] Open failed: Write not permitted\n");
            return -EINVAL;
        }

        truncate_fs(node);
    }

    // Could not find the file and the file should not be created. Return error.
    if (!node)
    {
        printf("[SYSCALL] Open failed: File not found\n");
        return -ENOENT;
    }

    // If create was requested and the found node was a directory, return error.
    if (node && (flags & O_CREAT) && (node->flags & FS_DIRECTORY))
    {
        close_fs(node);

        printf("[SYSCALL] Open failed: File is a directory\n");
        return -EISDIR;
    }

    // Finally append the file to the current process and set the access bits

    int fd = process_append_fd((process_t *)process_get_current(), node);
    FILE_DESC_MODE(fd) = access_bits;

    if (flags & O_APPEND)
    {
        FILE_DESC_OFFSET(fd) = node->length;
    }
    else
    {
        FILE_DESC_OFFSET(fd) = 0;
    }

    // printf("[OPEN] Process %i opened file", process_get_pid());

    return fd;
}

//=============================================================================
// End of file
//=============================================================================
