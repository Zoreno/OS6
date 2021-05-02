/**
 * @file launch_program.c
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

#include <process/launch_program.h>

#include <stdio.h>

#include <exec/elf64.h>
#include <logging/logging.h>
#include <process/process.h>
#include <syscall/syscall.h>

int launch_program(char *path)
{
    pid_t pid;
    int status;
    int retval;

    // TODO: This needs to be fixed
    static char *programPath;
    programPath = path;

    pid = process_fork();

    if (pid < 0)  // Error
    {
        log_error("[LAUNCH] Error: Could not fork");
        return -1;  // TODO: This should probably be something else
    }
    else if (pid == 0)  // Child
    {
        exec_elf(programPath, 0, NULL, NULL, 0);
        log_error("[LAUNCH] Exec elf returned");

        return -1;
    }
    else
    {
        int status;

        retval = waitpid(pid, &status, 0);
    }

    return retval;
}

//=============================================================================
// End of file
//=============================================================================
