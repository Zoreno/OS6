/**
 * @file syscall_ioctl.c
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

int syscall_ioctl(int fd, int request, void *argp)
{
    if (!FILE_DESC_CHECK(fd))
    {
        return -EBADF;
    }

    return ioctl_fs(FILE_DESC_ENTRY(fd), request, argp);
}

//=============================================================================
// End of file
//=============================================================================