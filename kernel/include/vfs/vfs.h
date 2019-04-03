#ifndef VFS_H
#define VFS_H

#include <stdint.h>

#define PATH_SEPARATOR '/'
#define PATH_SEPARATOR_STRING "/"
#define PATH_UP ".."
#define PATH_DOT "."

#define FS_FILE 0x01
#define FS_DIRECTORY 0x02
#define FS_CHARDEVICE 0x04
#define FS_BLOCKDEVICE 0x08
#define FS_PIPE 0x10
#define FS_SYMLINK 0x20
#define FS_MOUNTPOINT 0x40

#define _IFMT 0170000   /* type of file */
#define _IFDIR 0040000  /* directory */
#define _IFCHR 0020000  /* character special */
#define _IFBLK 0060000  /* block special */
#define _IFREG 0100000  /* regular */
#define _IFLNK 0120000  /* symbolic link */
#define _IFSOCK 0140000 /* socket */
#define _IFIFO 0010000  /* fifo */

struct fs_node;

typedef uint32_t (*read_func_t)(struct fs_node *, uint32_t, uint32_t, uint8_t *);
typedef uint32_t (*write_func_t)(struct fs_node *, uint32_t, uint32_t, uint8_t *);
typedef void (*open_func_t)(struct fs_node *, uint8_t read, uint8_t write);
typedef void (*close_func_t)(struct fs_node *);
typedef struct dirent *(*readdir_func_t)(struct fs_node *, uint32_t);
typedef struct fs_node *(*finddir_func_t)(struct fs_node *, char *name);
typedef void (*create_func_t)(struct fs_node *, char *name, uint16_t permission);
typedef void (*mkdir_func_t)(struct fs_node *, char *name, uint16_t permission);

typedef struct fs_node
{
    char name[256];

    uint32_t mask;

    uint32_t uid;
    uint32_t gid;

    uint32_t flags;
    uint32_t inode;
    uint32_t length;
    uint32_t impl;

    read_func_t read;
    write_func_t write;
    open_func_t open;
    close_func_t close;
    readdir_func_t readdir;
    finddir_func_t finddir;
    create_func_t create;
    mkdir_func_t mkdir;

    struct fs_node *ptr;

    uint32_t offset;
    int32_t shared_with;

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
};

extern fs_node_t *fs_root;
extern fs_node_t *null_device_create();

uint32_t read_fs(fs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buffer);
uint32_t write_fs(fs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buffer);
void open_fs(fs_node_t *node, uint8_t read, uint8_t write);
void close_fs(fs_node_t *node);
struct dirent *readdir_fs(fs_node_t *node, uint32_t index);
fs_node_t *finddir_fs(fs_node_t *node, char *name);
int mkdir_fs(char *name, uint16_t permission);
int create_file_fs(char *name, uint16_t permission);
fs_node_t *kopen(char *filename, uint32_t flags);
char *canonicalize_path(char *cwd, char *input);
fs_node_t *clone_fs(fs_node_t *source);

#endif