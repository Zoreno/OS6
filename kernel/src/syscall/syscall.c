/**
 * @file syscall.c
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-07-19
 * 
 * @brief System call interface
 * 
 * @copyright Copyright (C) 2019,
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

#include <arch/arch.h>

#include <stdio.h>

#include <debug/backtrace.h>

#include <process/process.h>
#include <errno.h>

#include <string.h>
#include <stdlib.h>

#define FILE_DESC_IN_RANGE(FILE) ((FILE) < (int)process_get_current()->file_descriptors->length && (FILE) >= 0)
#define FILE_DESC_ENTRY(FILE) (process_get_current()->file_descriptors->entries[(FILE)])
#define FILE_DESC_CHECK(FILE) (FILE_DESC_IN_RANGE(FILE) && FILE_DESC_ENTRY(FILE))
#define FILE_DESC_OFFSET(FILE) (process_get_current()->file_descriptors->offsets[(FILE)])
#define FILE_DESC_MODE(FILE) (process_get_current()->file_descriptors->modes[(FILE)])

int syscall_debug_print(char *str)
{
	printf("%s", str);

	return 0;
}

int syscall_exit(int retval)
{
	process_exit(retval);

	__builtin_unreachable();
}

int syscall_yield(int reschedule)
{
	process_yield(reschedule);

	return 0;
}

int syscall_sleep(uint64_t ms)
{
	process_sleep(ms);

	return 0;
}

int syscall_fork();
int syscall_clone();
int syscall_kill(pid_t process, uint32_t signal);

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
	if ((flags && O_RDWR) || (flags & O_WRONLY))
	{
		if (node && !has_permissions(node, 02))
		{
			close_fs(node);
			return -EACCES;
		}

		// Cannot write to directory descriptors directly
		if (node && (node->flags & FS_DIRECTORY))
		{
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
			return -ENOTDIR;
		}
	}

	// Truncate the file on open if requested
	if (node && (flags & O_TRUNC))
	{
		if (!(access_bits & 02))
		{
			close_fs(node);
			return -EINVAL;
		}

		truncate_fs(node);
	}

	// Could not find the file and the file should not be created. Return error.
	if (!node)
	{
		return -ENOENT;
	}

	// If create was requested and the found node was a directory, return error.
	if (node && (flags & O_CREAT) && (node->flags & FS_DIRECTORY))
	{
		close_fs(node);

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

	printf("[OPEN] Process %i opened file", process_get_pid());

	return fd;
}

int syscall_close(int fd)
{
	if (FILE_DESC_CHECK(fd))
	{
		close_fs(FILE_DESC_ENTRY(fd));
		FILE_DESC_ENTRY(fd) = NULL;

		return 0;
	}

	return -EBADF;
}

int syscall_read(int fd, char *ptr, uint64_t len)
{
	if (!FILE_DESC_CHECK(fd))
	{
		return -EBADF;
	}

	fs_node_t *node = FILE_DESC_ENTRY(fd);

	if (!(FILE_DESC_MODE(fd) & 01))
	{
		printf("[SYSCALL] Access denied\n");
		return -EACCES;
	}

	uint32_t out = read_fs(node, FILE_DESC_OFFSET(fd), len, (uint8_t *)ptr);
	FILE_DESC_OFFSET(fd) += out;

	return (int)out;
}

int syscall_write(int fd, char *ptr, uint64_t len)
{
	if (!FILE_DESC_CHECK(fd))
	{
		return -EBADF;
	}

	fs_node_t *node = FILE_DESC_ENTRY(fd);

	if (!(FILE_DESC_MODE(fd) & 02))
	{
		printf("[SYSCALL] Access denied\n");
		return -EACCES;
	}

	uint32_t out = write_fs(node, FILE_DESC_OFFSET(fd), len, (uint8_t *)ptr);
	FILE_DESC_OFFSET(fd) += out;

	return (int)out;
}

int syscall_seek(int fd, int offset, int whence)
{
	if (FILE_DESC_CHECK(fd))
	{
		// Cannot seek in stdin, stdout and stderr
		if (fd < 3)
		{
			return 0;
		}

		switch (whence)
		{
		case 0:
			FILE_DESC_OFFSET(fd) = offset;
			break;
		case 1:
			FILE_DESC_OFFSET(fd) += offset;
			break;
		case 2:
			FILE_DESC_OFFSET(fd) = FILE_DESC_ENTRY(fd)->length + offset;
			break;

		default:
			// TODO: Maybe return an error or something
			break;
		}

		return FILE_DESC_OFFSET(fd);
	}

	return -EBADF;
}

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

int syscall_access(const char *file, int flags)
{
	// TODO: Fill out this debug message with more info.
	printf("[ACCESS]\n");

	// Try to open the file with the given flags
	fs_node_t *node = kopen((char *)file, flags);

	if (!node)
	{
		return -ENOENT;
	}

	close_fs(node);

	return 0;
}

int syscall_ioctl(int fd, int request, void *argp)
{
	if (!FILE_DESC_CHECK(fd))
	{
		return -EBADF;
	}

	return ioctl_fs(FILE_DESC_ENTRY(fd), request, argp);
}

static int stat_node(fs_node_t *node, uintptr_t addr)
{
	struct stat *f = (struct stat *)addr;

	// TODO: Validate the pointer

	if (!node)
	{
		memset(f, 0, sizeof(struct stat));

		printf("[STAT] Tried to stat a non-existent file\n");

		return -ENOENT;
	}

	f->st_dev = (uint16_t)(((uint32_t)node->device & 0xFFFF0) >> 8);
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

int syscall_statf(char *file, uintptr_t addr)
{
	int result;

	fs_node_t *node = kopen(file, 0);

	result = stat_node(node, addr);

	if (node)
	{
		close_fs(node);
	}

	return result;
}

int syscall_stat(int fd, uintptr_t addr)
{
	if (FILE_DESC_CHECK(fd))
	{
		return stat_node(FILE_DESC_ENTRY(fd), addr);
	}

	return -EBADF;
}

int syscall_mkdir(char *path, int mode)
{
	return mkdir_fs(path, mode);
}

int syscall_unlink(char *file)
{
	return unlink_fs(file);
}

int syscall_symlink(char *target, char *name)
{
	return symlink_fs(target, name);
}

int syscall_readlink(const char *file, char *ptr, int len)
{
	fs_node_t *node = kopen((char *)file, O_PATH | O_NOFOLLOW);

	if (!node)
	{
		return -ENOENT;
	}

	int rv = readlink_fs(node, ptr, len);

	close_fs(node);

	return rv;
}

int syscall_lstat(char *file, uintptr_t addr)
{
	int result;

	fs_node_t *node = kopen(file, O_PATH | O_NOFOLLOW);

	result = stat_node(node, addr);

	if (node)
	{
		close_fs(node);
	}

	return result;
}

int syscall_chmod(char *file, int mode);
int syscall_chown(char *file, int uid, int gid);

int syscall_sbrk(uint64_t size);

int syscall_getpid()
{
	// TODO: This should return the group ID if the process is in a group
	return process_get_current()->id;
}

int syscall_gettid()
{
	return syscall_getpid();
}

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

int syscall_getcwd(char *buf, size_t size)
{
	if (!buf)
	{
		return -EFAULT;
	}

	if (buf && (size == 0))
	{
		return -EINVAL;
	}

	size_t len = strlen(process_get_current()->wd_path) + 1;

	if (len > size)
	{
		return -ERANGE;
	}

	memcpy(buf, process_get_current()->wd_path, len);

	return 0;
}

// int syscall_gettimeofday(struct timeval *tv, void *tz);

typedef int (*syscall_func_t)(uint64_t, ...);

syscall_func_t _syscalls[128] = {0};

void syscall_handler(system_stack_t *stack)
{
	// printf("Got system call: %i\n", stack->rax);

	// TODO: All syscall function handlers should be in a separate file

	syscall_func_t syscall_func = _syscalls[stack->rax];

	if (!syscall_func)
	{
		printf("[SYSCALL] Invalid system call: %i\n", stack->rax);
		return;
	}

	// TODO: Add more parameters
	int retval = syscall_func(stack->rbx, stack->rcx, stack->rdx);

	stack->rax = retval;
}

#define DECLARE_SYSCALL(NAME, FUNC) _syscalls[SYSCALL_##NAME] = (syscall_func_t)syscall_##FUNC

void syscall_install()
{
	// Sticks head in the sand...
#pragma GCC diagnostic ignored "-Wcast-function-type"
	DECLARE_SYSCALL(DEBUG_PRINT, debug_print);
	DECLARE_SYSCALL(EXIT, exit);
	DECLARE_SYSCALL(YIELD, yield);
	DECLARE_SYSCALL(SLEEP, sleep);

	DECLARE_SYSCALL(OPEN, open);
	DECLARE_SYSCALL(CLOSE, close);
	DECLARE_SYSCALL(READ, read);
	DECLARE_SYSCALL(WRITE, write);
	DECLARE_SYSCALL(SEEK, seek);
	DECLARE_SYSCALL(READDIR, readdir);
	DECLARE_SYSCALL(ACCESS, access);
	DECLARE_SYSCALL(IOCTL, ioctl);
	DECLARE_SYSCALL(STATF, statf);
	DECLARE_SYSCALL(STAT, stat);
	DECLARE_SYSCALL(MKDIR, mkdir);
	DECLARE_SYSCALL(UNLINK, unlink);
	DECLARE_SYSCALL(SYMLINK, symlink);
	DECLARE_SYSCALL(READLINK, readlink);
	DECLARE_SYSCALL(LSTAT, lstat);

	DECLARE_SYSCALL(GETPID, getpid);
	DECLARE_SYSCALL(GETTID, gettid);

	DECLARE_SYSCALL(CHDIR, chdir);
	DECLARE_SYSCALL(GETCWD, getcwd);
#pragma GCC diagnostic pop

	set_irq_handler(SYSCALL_INTNO, syscall_handler);
}

//=============================================================================
// End of file
//=============================================================================