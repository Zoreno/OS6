/**
 * @file syscall_readdir.c
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

int syscall_readdir(int fd, int index, struct dirent *entry)
{
	if (!FILE_DESC_CHECK(fd))
	{
		return -EBADF;
	}

	struct dirent *dir_entry = readdir_fs(FILE_DESC_ENTRY(fd), (uint32_t)index);

	if (dir_entry)
	{
		memcpy(entry, dir_entry, sizeof(*entry));
		free(dir_entry);
		return 1;
	}

	return 0;
}