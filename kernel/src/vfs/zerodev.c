#include <vfs/zerodev.h>

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include <vfs/vfs.h>

static uint32_t read_zero(fs_node_t *node, uint64_t offset, uint32_t size, uint8_t *buffer)
{
    memset(buffer, 0, size);
    return size;
}

static uint32_t write_zero(fs_node_t *node, uint64_t offset, uint32_t size, uint8_t *buffer)
{
    return 0;
}

static void open_zero(fs_node_t *node, uint32_t flags)
{
    return;
}

static void close_zero(fs_node_t *node)
{
    return;
}

void zero_dev_init()
{
    fs_node_t *node = malloc(sizeof(fs_node_t));

    if (!node)
    {
        return;
    }

    memset(node, 0, sizeof(fs_node_t));

    node->inode = 0;
    strcpy(node->name, "zero");

    node->uid = 0;
    node->gid = 0;

    node->permissions = 0666;
    node->flags = FS_CHARDEVICE | FS_FILE;
    node->read = read_zero;
    node->write = write_zero;
    node->open = open_zero;
    node->close = close_zero;
    node->readdir = NULL;
    node->finddir = NULL;
    node->ioctl = NULL;

    vfs_mount("/dev/zero", node);
}