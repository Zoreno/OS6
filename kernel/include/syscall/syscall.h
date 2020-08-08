/**
 * @file syscall.h
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

#ifndef _SYSCALL_H
#define _SYSCALL_H

#include <stdint.h>
#include <vfs/vfs.h>
#include <process/process.h>

/**
 * @brief The system call interrupt number
 * 
 * 
 */
#define SYSCALL_INTNO 0x80

#define SYSCALL_DEBUG_PRINT 0
#define SYSCALL_EXIT 1
#define SYSCALL_YIELD 2
#define SYSCALL_SLEEP 3

#define SYSCALL_FORK 4
#define SYSCALL_CLONE 5
#define SYSCALL_KILL 6

#define SYSCALL_OPEN 7
#define SYSCALL_CLOSE 8
#define SYSCALL_READ 9
#define SYSCALL_WRITE 10
#define SYSCALL_SEEK 11
#define SYSCALL_READDIR 12
#define SYSCALL_ACCESS 13
#define SYSCALL_IOCTL 14
#define SYSCALL_STATF 15
#define SYSCALL_STAT 16
#define SYSCALL_MKDIR 17
#define SYSCALL_UNLINK 18
#define SYSCALL_SYMLINK 19
#define SYSCALL_READLINK 20
#define SYSCALL_LSTAT 21

#define SYSCALL_CHMOD 22
#define SYSCALL_CHOWN 23

#define SYSCALL_SBRK 24

#define SYSCALL_GETPID 25
#define SYSCALL_GETTID 26

#define SYSCALL_CHDIR 27
#define SYSCALL_GETCWD 28

#define _IFMT 0170000 /* type of file */
#define S_ISBLK(m) (((m)&_IFMT) == _IFBLK)
#define S_ISCHR(m) (((m)&_IFMT) == _IFCHR)
#define S_ISDIR(m) (((m)&_IFMT) == _IFDIR)
#define S_ISFIFO(m) (((m)&_IFMT) == _IFIFO)
#define S_ISREG(m) (((m)&_IFMT) == _IFREG)
#define S_ISLNK(m) (((m)&_IFMT) == _IFLNK)
#define S_ISSOCK(m) (((m)&_IFMT) == _IFSOCK)

#define S_ISUID 0004000  /* set user id on execution */
#define S_ISGID 0002000  /* set group id on execution */
#define S_ISVTX 0001000  /* save swapped text even after use */
#define S_IREAD 0000400  /* read permission, owner */
#define S_IWRITE 0000200 /* write permission, owner */
#define S_IEXEC 0000100  /* execute/search permission, owner */
#define S_ENFMT 0002000  /* enforcement-mode locking */

#define S_IRWXU (S_IRUSR | S_IWUSR | S_IXUSR)
#define S_IRUSR 0000400 /* read permission, owner */
#define S_IWUSR 0000200 /* write permission, owner */
#define S_IXUSR 0000100 /* execute/search permission, owner */
#define S_IRWXG (S_IRGRP | S_IWGRP | S_IXGRP)
#define S_IRGRP 0000040 /* read permission, group */
#define S_IWGRP 0000020 /* write permission, grougroup */
#define S_IXGRP 0000010 /* execute/search permission, group */
#define S_IRWXO (S_IROTH | S_IWOTH | S_IXOTH)
#define S_IROTH 0000004 /* read permission, other */
#define S_IWOTH 0000002 /* write permission, other */
#define S_IXOTH 0000001 /* execute/search permission, other */

int syscall_debug_print(char *str); // DONE
int syscall_exit(int retval);       // DONE
int syscall_yield(int reschedule);  // DONE
int syscall_sleep(uint64_t ms);     // DONE

int syscall_fork();
int syscall_clone();
int syscall_kill(pid_t process, uint32_t signal);

int syscall_open(const char *file, int flags, int mode);      // DONE
int syscall_close(int fd);                                    // DONE
int syscall_read(int fd, char *ptr, uint64_t len);            // DONE
int syscall_write(int fd, char *ptr, uint64_t len);           // DONE
int syscall_seek(int fd, int offset, int whence);             // DONE
int syscall_readdir(int fd, int index, struct dirent *entry); // DONE
int syscall_access(const char *file, int flags);              // DONE
int syscall_ioctl(int fd, int request, void *argp);           // DONE
int syscall_statf(char *file, uintptr_t st);                  // DONE
int syscall_stat(int fd, uintptr_t st);                       // DONE
int syscall_mkdir(char *path, int mode);                      // DONE
int syscall_unlink(const char *file);                         // DONE
int syscall_symlink(char *target, char *name);                // DONE
int syscall_readlink(const char *file, char *ptr, int len);   // DONE
int syscall_lstat(const char *file, uintptr_t st);            // DONE

int syscall_chmod(char *file, int mode);
int syscall_chown(char *file, int uid, int gid);

int syscall_sbrk(uint64_t size);

int syscall_getpid(); // DONE
int syscall_gettid(); // DONE

int syscall_chdir(char *newdir);            // DONE
int syscall_getcwd(char *buf, size_t size); // DONE

void syscall_install();

#endif

//=============================================================================
// End of file
//=============================================================================