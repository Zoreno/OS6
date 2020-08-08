/**
 * @file syscall_chdir.c
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

int syscall_chdir(char *newdir)
{
	char *path = canonicalize_path(process_get_current()->wd_path, newdir);

	fs_node_t *chd = kopen(path, 0);

	if (chd)
	{
		if ((chd->flags & FS_DIRECTORY) == 0)
		{
			close_fs(chd);
			return -ENOTDIR;
		}

		if (!has_permissions(chd, 01))
		{
			close_fs(chd);
			return -EACCES;
		}

		close_fs(chd);

		process_t *current_process = process_get_current();

		free(current_process->wd_path);
		current_process->wd_path = malloc(strlen(path) + 1);
		memcpy(current_process->wd_path, path, strlen(path) + 1);

		return 0;
	}

	return -ENOENT;
}
