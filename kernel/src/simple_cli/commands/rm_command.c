/**
 * @file rm_command.c
 * @author Joakim Bertils
 * @version 0.1
 * @date 2020-12-31
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

#include <simple_cli/commands.h>

#include <stdio.h>

#include <syscall/syscall.h>

static int rm_command_remove_item(int recursive, const char *path);
static int rm_command_remove_directory(int recursive, const char *dir);

static int rm_command_remove_item(int recursive, const char *path)
{
    struct stat statbuf;
    syscall_lstat(path, (uintptr_t)&statbuf);

    if (S_ISDIR(statbuf.st_mode))
    {
        if (!recursive)
        {
            printf("[RM]: %s is a directory\n", path);
            return 1;
        }
        return rm_command_remove_directory(recursive, path);
    }

    syscall_unlink(path);

    return 0;
}

static int rm_command_remove_directory(int recursive, const char *dir)
{
    int fd = syscall_open(dir, O_DIRECTORY, O_RDWR);

    if (fd < 0)
    {
        printf("Could not open %s\n", dir);
        return fd;
    }

    struct dirent dirent;
    int i = 0;

    while (syscall_readdir(fd, i, &dirent) == 1)
    {
        if (!strcmp(dirent.name, ".") || !strcmp(dirent.name, ".."))
        {
            ++i;
            continue;
        }

        char tmp[128] = {0};

        sprintf(tmp, "%s/%s", dir, dirent.name);

        int status = rm_command_remove_item(recursive, tmp);

        if (status)
        {
            return status;
        }

        ++i;
    }

    syscall_close(fd);

    syscall_unlink(dir);

    return 0;
}

int rm_command(int argc, const char **argv)
{
    int recursive = 0;

    if (argc <= 1)
    {
        printf("Argument expected\n");
    }

    if (strcmp(argv[1], "-r") == 0)
    {
        recursive = 1;
    }

    for (int i = recursive ? 2 : 1; i < argc; ++i)
    {
        rm_command_remove_item(recursive, argv[i]);
    }

    return 0;
}

//=============================================================================
// End of file
//=============================================================================
