/**
 * @file mkdir_command.c
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

#include <simple_cli/simple_cli.h>
#include <syscall/syscall.h>

#include <stdio.h>

int mkdir_command(int argc, const char **argv)
{
    if (argc <= 1)
    {
        printf("Argument expected\n");
    }

    for (int i = 1; i < argc; ++i)
    {
        int ret = syscall_mkdir((char *)argv[i], 0777);

        if (ret < 0)
        {
            return ret;
        }
    }

    return 0;
}

//=============================================================================
// End of file
//=============================================================================
