/**
 * @file vfs.h
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-04-06
 *
 * @brief Virtual file system interface
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

#ifndef VFS_H
#define VFS_H

#include <stdint.h>
#include <stddef.h>

/**
 * @brief Maximum symbolic link depth that the VFS will try to interpret.
 *
 *
 */
#define MAX_SYMLINK_DEPTH 8

/**
 * @brief Maximum symbolic link size
 *
 *
 */
#define MAX_SYMLINK_SIZE 4096

/**
 * @brief The separator used to distinguish levels in a path
 *
 *
 */
#define PATH_SEPARATOR '/'

/**
 * @brief The separator used to distinguish levels in a path as a string
 *
 *
 */
#define PATH_SEPARATOR_STRING "/"

/**
 * @brief The path indicating the parent directory of the current folder
 *
 *
 */
#define PATH_UP ".."

/**
 * @brief The path indicating the current folder
 *
 *
 */
#define PATH_DOT "."

/**
 * @brief Open the file for reading only
 *
 *
 */
#define O_RDONLY 0x0000

/**
 * @brief Open the file for writing only
 *
 *
 */
#define O_WRONLY 0x0001

/**
 * @brief Open the file for reading and writing
 *
 *
 */
#define O_RDWR 0x0002

/**
 * @brief The file offset should be set to the end of the file before each
 * write.
 *
 *
 */
#define O_APPEND 0x0008

/**
 * @brief Create the file if it does not exist.
 *
 * If the file exists, this flag has no effect except when used in combination
 * with O_EXCL. Otherwise, the file is created.
 */
#define O_CREAT 0x0200

/**
 * @brief Truncate the file on open.
 *
 * If the file exists and it is a regular file, and the file is successfully
 * opened as O_RDWR or O_WRONLY, its length should be set to 0.
 */
#define O_TRUNC 0x0400

/**
 * @brief If O_EXCL and O_CREAT is both set, open() should fail if the fail
 * already exists.
 *
 *
 */
#define O_EXCL 0x0800

/**
 * @brief Do not follow symbolic links.
 *
 *
 */
#define O_NOFOLLOW 0x1000
#define O_PATH 0x2000
#define O_NONBLOCK 0x4000
#define O_DIRECTORY 0x8000

#define FS_FILE 0x01
#define FS_DIRECTORY 0x02
#define FS_CHARDEVICE 0x04
#define FS_BLOCKDEVICE 0x08
#define FS_PIPE 0x10
#define FS_SYMLINK 0x20
#define FS_MOUNTPOINT 0x40

/**
 * @brief Type of file
 *
 *
 */
#define _IFMT 0170000 /* type of file */

/**
 * @brief Directory
 *
 *
 */
#define _IFDIR 0040000 /* directory */

/**
 * @brief Character special
 *
 *
 */
#define _IFCHR 0020000 /* character special */

/**
 * @brief Block special
 *
 *
 */
#define _IFBLK 0060000 /* block special */

/**
 * @brief Regular file
 *
 *
 */
#define _IFREG 0100000 /* regular */

/**
 * @brief Symbolic link
 *
 *
 */
#define _IFLNK 0120000 /* symbolic link */

/**
 * @brief Socket
 *
 *
 */
#define _IFSOCK 0140000 /* socket */

/**
 * @brief FIFO
 *
 *
 */
#define _IFIFO 0010000 /* fifo */

struct fs_node;

/**
 * @brief Type of function to read from a node
 *
 *
 */
typedef uint32_t (*read_func_t)(struct fs_node *,
                                uint64_t,
                                uint32_t,
                                uint8_t *);

/**
 * @brief Type of function to write to node
 *
 *
 */
typedef uint32_t (*write_func_t)(struct fs_node *,
                                 uint64_t,
                                 uint32_t,
                                 uint8_t *);

/**
 * @brief Type of function to open a node
 *
 *
 */
typedef void (*open_func_t)(struct fs_node *, uint32_t flags);

/**
 * @brief Type of function to close a node
 *
 *
 */
typedef void (*close_func_t)(struct fs_node *);

/**
 * @brief Type of function to read from a directory node
 *
 *
 */
typedef struct dirent *(*readdir_func_t)(struct fs_node *, uint32_t);

/**
 * @brief TYpe of function to find a directory
 *
 *
 */
typedef struct fs_node *(*finddir_func_t)(struct fs_node *, char *name);

/**
 * @brief Type of function to create a file
 *
 *
 */
typedef int (*create_func_t)(struct fs_node *, char *name, uint16_t permission);

/**
 * @brief Type of function to unlink a file
 *
 *
 */
typedef int (*unlink_func_t)(struct fs_node *, char *name);

/**
 * @brief Type of function to create a directory
 *
 *
 */
typedef int (*mkdir_func_t)(struct fs_node *, char *name, uint16_t permission);

/**
 * @brief Type of function to perform IO control opertations
 *
 *
 */
typedef int (*ioctl_func_t)(struct fs_node *, int request, void *argp);

/**
 * @brief Type of function to get the size of a node
 *
 *
 */
typedef int (*get_size_func_t)(struct fs_node *);

/**
 * @brief Type of function to change the permissions of a file
 *
 *
 */
typedef int (*chmod_func_t)(struct fs_node *, int mode);

/**
 * @brief Type of function to create a symbolic link
 *
 *
 */
typedef int (*symlink_func_t)(struct fs_node *, char *name, char *value);

/**
 * @brief Type of function to read a symbolic link
 *
 *
 */
typedef int (*readlink_func_t)(struct fs_node *, char *buf, size_t size);

/**
 * @brief Type of function to perform a selectcheck operation
 *
 *
 */
typedef int (*selectcheck_func_t)(struct fs_node *);

/**
 * @brief Type of function to perform a selectwait operation
 *
 *
 */
typedef int (*selectwait_func_t)(struct fs_node *, void *process);

/**
 * @brief Type of function to change owner file
 *
 *
 */
typedef int (*chown_func_t)(struct fs_node *, int, int);

/**
 * @brief Type of function to truncate a file.
 *
 *
 */
typedef int (*truncate_func_t)(struct fs_node *);

/**
 * @brief Struct representing a node in the file system
 *
 *
 */
typedef struct fs_node
{
    /**
     * @brief Name of the node
     *
     *
     */
    char name[256];

    /**
     * @brief Pointer to the device specific data
     *
     *
     */
    void *device;

    /**
     * @brief Permission mask
     *
     *
     */
    uint32_t permissions;

    /**
     * @brief User ID
     *
     *
     */
    uint32_t uid;

    /**
     * @brief Group ID
     *
     *
     */
    uint32_t gid;

    /**
     * @brief File flags
     *
     *
     */
    uint32_t flags;

    /**
     * @brief Inode index
     *
     *
     */
    uint32_t inode;

    /**
     * @brief Length of the file
     *
     *
     */
    uint32_t length;

    /**
     * @brief Used to keep track of which file system it is using.
     *
     *
     */
    uint32_t impl;

    /**
     * @brief Flags passed to open
     *
     *
     */
    uint32_t open_flags;

    /**
     * @brief Accessed time
     *
     *
     */
    uint32_t atime;

    /**
     * @brief Modified time
     *
     *
     */
    uint32_t mtime;

    /**
     * @brief Created time
     *
     *
     */
    uint32_t ctime;

    /**
     * @brief Function used to read from the node
     *
     *
     */
    read_func_t read;

    /**
     * @brief Function used to write to the node
     *
     *
     */
    write_func_t write;

    /**
     * @brief Function used to open the node
     *
     *
     */
    open_func_t open;

    /**
     * @brief Function used to close the node
     *
     *
     */
    close_func_t close;

    /**
     * @brief Function used to read from a directory
     *
     *
     */
    readdir_func_t readdir;

    /**
     * @brief Function used to find a directory
     *
     *
     */
    finddir_func_t finddir;

    /**
     * @brief Function used to create a file
     *
     *
     */
    create_func_t create;

    /**
     * @brief Function used to create a directory
     *
     *
     */
    mkdir_func_t mkdir;

    /**
     * @brief Function used by IO control options
     *
     *
     */
    ioctl_func_t ioctl;

    /**
     * @brief Function used to get the size of the node
     *
     *
     */
    get_size_func_t get_size;

    /**
     * @brief Function used to change the permissions of the node.
     *
     *
     */
    chmod_func_t chmod;

    /**
     * @brief Function used to unlink this node
     *
     *
     */
    unlink_func_t unlink;

    /**
     * @brief Function used to create a symlink
     *
     *
     */
    symlink_func_t symlink;

    /**
     * @brief Function used to read a symlink
     *
     *
     */
    readlink_func_t readlink;

    /**
     * @brief Function used to change the owner of the node
     *
     *
     */
    chown_func_t chown;

    /**
     * @brief Function used to truncate the node.
     *
     *
     */
    truncate_func_t truncate;

    /**
     * @brief Pointer used if this is a symlink/mountpoint
     *
     *
     */
    struct fs_node *ptr;

    /**
     * @brief Current amount of references to this node
     *
     *
     */
    int32_t refcount;

    /**
     * @brief Number of current links to this node.
     *
     *
     */
    uint32_t nlink;

    /**
     * @brief Function used for the selectcheck operation.
     *
     *
     */
    selectcheck_func_t selectcheck;

    /**
     * @brief Function used for the selectwait operation.
     *
     *
     */
    selectwait_func_t selectwait;

} fs_node_t;

struct dirent
{
    char name[256];
    uint32_t ino;
};

struct stat
{
    uint16_t st_dev;
    uint16_t st_ino;
    uint32_t st_mode;
    uint16_t st_nlink;
    uint16_t st_uid;
    uint16_t st_gid;
    uint16_t st_rdev;
    uint32_t st_size;

    uint32_t st_atime;
    uint32_t st_mtime;
    uint32_t st_ctime;

    uint32_t st_blksize;
    uint32_t st_blocks;
};

typedef struct
{
    char *name;
    fs_node_t *file;
    char *device;
    char *fs_type;
} vfs_entry_t;

extern fs_node_t *fs_root;

int has_permissions(fs_node_t *node, int permission_bit);

//=============================================================================
// VFS operations
//=============================================================================

/**
 * @brief Reads some selected number of bytes from the VFS.
 *
 * @param node Node describing where to read from. See @ref kopen.
 * @param offset Offset to start reading from.
 * @param size Number of bytes to read.
 * @param buffer Buffer to place the read bytes.
 *
 * @return Number of bytes read.
 */
uint32_t read_fs(fs_node_t *node,
                 uint64_t offset,
                 uint32_t size,
                 uint8_t *buffer);
uint32_t write_fs(fs_node_t *node,
                  uint64_t offset,
                  uint32_t size,
                  uint8_t *buffer);
void open_fs(fs_node_t *node, uint32_t flags);
void close_fs(fs_node_t *node);
struct dirent *readdir_fs(fs_node_t *node, uint32_t index);
fs_node_t *finddir_fs(fs_node_t *node, char *name);
int mkdir_fs(char *name, uint16_t permission);
int create_file_fs(char *name, uint16_t permission);
fs_node_t *clone_fs(fs_node_t *source);

int ioctl_fs(fs_node_t *node, int request, void *argp);
int chmod_fs(fs_node_t *node, int mode);
int chown_fs(fs_node_t *node, int uid, int gid);
int unlink_fs(char *name);
int symlink_fs(char *target, char *name);
int readlink_fs(fs_node_t *node, char *buf, size_t size);
int selectcheck_fs(fs_node_t *node);
int selectwait_fs(fs_node_t *node, void *process);
void truncate_fs(fs_node_t *node);

//=============================================================================
// VFS interface
//=============================================================================

void vfs_install(void);
void *vfs_mount(char *path, fs_node_t *local_root);
int vfs_register(char *name,
                 fs_node_t *(*vfs_mount_callback)(char *arg,
                                                  char *mount_point));
int vfs_mount_type(char *type, char *arg, char *mountpoint);
void vfs_lock(fs_node_t *node);
int vfs_is_root(const fs_node_t *node);
fs_node_t *vfs_get_root(void);

/**
 * @brief Opens the file at @a path.
 *
 * @param filename Path to the file. Can be given as absolute value or relative
 * to the current work directory.
 * @param flags Open flags. See macros prefixed with O_.
 *
 * @return Pointer to heap-allocated node. Remember to free!
 */
fs_node_t *kopen(char *filename, uint32_t flags);

/**
 * @brief Expands a path.
 *
 * Relative paths will be prepended with the current working directory.
 * Instances of "/./" will be replaced with "/". Instances of "/<any dir>/../"
 * will be replaced with "/".
 *
 * @param cwd Current working directory, usually the process wd.
 * @param input The path to expand.
 *
 * @return Pointer to heap-allocated string with the expanded path.
 */
char *canonicalize_path(char *cwd, char *input);

/**
 * @brief Prints the tree to stdout. Mainly used for debug.
 *
 *
 */
void print_vfs_tree(void);

#endif

//=============================================================================
// End of file
//=============================================================================