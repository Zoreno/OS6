/**
 * @file syscall_debug_print.c
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

#include <string.h>

#include <logging/logging.h>

int syscall_debug_print(char *str)
{
    // No string
    if (!str)
    {
        return -1;
    }

    // Empty string
    if (!str[0])
    {
        return -1;
    }

    // Remove trailing newline
    char *last_char = &str[strlen(str) - 1];
    if (*last_char == '\n')
    {
        *last_char = '\0';
    }

    // Do the printing
    log_debug("[APP] %s", str);

    return 0;
}

//=============================================================================
// End of file
//=============================================================================
