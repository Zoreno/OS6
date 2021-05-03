/**
 * @file ls_command.c
 * @author Joakim Bertils
 * @version 0.1
 * @date 2020-08-09
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
#include <simple_cli/simple_cli.h>
#include <syscall/syscall.h>

#include <stdio.h>

static void ls_command_format_permissions(uint32_t permissions, char *buffer)
{
    sprintf(buffer,
            "%c%c%c%c%c%c%c%c%c",
            permissions & S_IRUSR ? 'r' : '-',
            permissions & S_IWUSR ? 'w' : '-',
            permissions & S_IXUSR ? 'x' : '-',

            permissions & S_IRGRP ? 'r' : '-',
            permissions & S_IWGRP ? 'w' : '-',
            permissions & S_IXGRP ? 'x' : '-',

            permissions & S_IROTH ? 'r' : '-',
            permissions & S_IWOTH ? 'w' : '-',
            permissions & S_IXOTH ? 'x' : '-');
}

static void ls_command_format_size(int human_readable,
                                   size_t size,
                                   char *buffer)
{
    if (human_readable)
    {
        if (size >= (1 << 20))
        {
            size_t t = size / (1 << 20);
            size_t td = (size - t * (1 << 20)) / ((1 << 20) / 10);
            sprintf(buffer, "%d.%1dM", (int)t, (int)td);
            return;
        }
        else if (size >= (1 << 10))
        {
            size_t t = size / (1 << 10);
            size_t td = (size - t * (1 << 10)) / ((1 << 10) / 10);
            sprintf(buffer, "%d.%1dK", (int)t, (int)td);
            return;
        }
    }

    sprintf(buffer, "%d", (int)size);
}

int ls_command(int argc, const char **argv)
{
    int fd =
        syscall_open(simple_cli_get_working_directory(), O_DIRECTORY, O_RDONLY);

    if (fd < 0)
    {
        return fd;
    }

    int i = 0;

    struct dirent dirent;

    while (syscall_readdir(fd, i, &dirent) == 1)
    {
        struct stat _stat;

        syscall_lstat(dirent.name, (uintptr_t)&_stat);

        char mode_char = '-';

        if (S_ISDIR(_stat.st_mode))
        {
            mode_char = 'd';
        }

        if (S_ISCHR(_stat.st_mode))
        {
            mode_char = 'c';
        }

        if (S_ISBLK(_stat.st_mode))
        {
            mode_char = 'b';
        }

        if (S_ISFIFO(_stat.st_mode))
        {
            mode_char = 'p';
        }

        char permissons_string[10] = {0};

        ls_command_format_permissions(_stat.st_mode, permissons_string);

        char size_string[32] = {0};

        ls_command_format_size(1, _stat.st_size, size_string);

        printf("%c%s %i %s %s %s %s\n",
               mode_char,
               permissons_string,
               _stat.st_nlink,
               simple_cli_get_user_name(),
               simple_cli_get_user_group(),
               size_string,
               dirent.name);

        ++i;
    }

    syscall_close(fd);

    return 0;
}

//=============================================================================
// End of file
//=============================================================================
