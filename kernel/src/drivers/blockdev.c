/**
 * @file blockdev.c
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-06-22
 * 
 * @brief Block device driver
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

#include <drivers/blockdev.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>

#include <arch/arch.h>
#include <debug/backtrace.h>
#include <logging/logging.h>
#include <util/list.h>
#include <vfs/vfs.h>

#define MAX_DESCRIPTION_LENGTH 256

typedef struct _blockdev_class
{
    uint32_t major;

    char description[MAX_DESCRIPTION_LENGTH];

    blockdev_read_func_t read;
    blockdev_write_func_t write;

    list_t *instance_list;
} blockdev_class_t;

typedef struct _blockdev_instance
{
    blockdev_class_t *class;

    unsigned int minor;

    char description[MAX_DESCRIPTION_LENGTH];

    size_t block_size;
    size_t capacity;

    uint32_t ref_count;
} blockdev_instance_t;

#define NUM_BLOCKDEV_CLASSES 2

static blockdev_class_t *blockdev_classes[NUM_BLOCKDEV_CLASSES] = {NULL};

static blockdev_instance_t *get_blockdev_instance(blockdev_class_t *class, uint32_t minor)
{
    for (list_node_t *lnode = class->instance_list->head; lnode != NULL; lnode = lnode->next)
    {
        blockdev_instance_t *instance = lnode->payload;

        if (instance->minor == minor)
        {
            instance->ref_count++;

            return instance;
        }
    }

    return NULL;
}

static void release_blockdev_instance(blockdev_instance_t *instance)
{
    instance->ref_count--;
}

int reg_blockdev_class(uint64_t major, const char *desc, blockdev_read_func_t read, blockdev_write_func_t write)
{

    blockdev_class_t *class;
    if (major > NUM_BLOCKDEV_CLASSES)
    {
        // Invalid blockdev class
        return -1;
    }

    if (blockdev_classes[major])
    {
        // Already registered
        return -1;
    }

    class = malloc(sizeof(blockdev_class_t));

    if (!class)
    {
        // No memory
        return -1;
    }

    class->major = major;
    class->read = read;
    class->write = write;
    class->instance_list = list_create();

    strncpy(class->description, desc, MAX_DESCRIPTION_LENGTH);
    class->description[MAX_DESCRIPTION_LENGTH - 1] = 0;

    blockdev_classes[major] = class;

    log_info("[Blockdev] Class %i registered [%s]", major, class->description);

    return 0;
}

static uint32_t read_blockdev_fs(fs_node_t *node, uint64_t offset, uint32_t size, uint8_t *buffer)
{
    blockdev_instance_t *instance = (blockdev_instance_t *)node->device;
    blockdev_class_t *class = instance->class;

    return blockdev_read(class->major, instance->minor, offset, size, buffer);
}

static uint32_t write_blockdev_fs(fs_node_t *node, uint64_t offset, uint32_t size, uint8_t *buffer)
{
    (void)node;

    return blockdev_write(0, 0, offset, size, buffer);
}

static void open_blockdev_fs(fs_node_t *node, uint32_t flags)
{
    blockdev_instance_t *instance = (blockdev_instance_t *)node->device;
    blockdev_class_t *class = instance->class;
}

int reg_blockdev_instance(uint32_t major, uint32_t minor, const char *desc, size_t block_size, size_t capacity)
{
    blockdev_class_t *class;
    blockdev_instance_t *instance;

    if (major > NUM_BLOCKDEV_CLASSES || !block_size || !capacity)
    {
        log_error("[BLOCKDEV] Invalid argument");
        return -1;
    }

    class = blockdev_classes[major];

    if (!class)
    {
        log_error("[BLOCKDEV] Blockdev class not registred");
        return -1;
    }

    instance = get_blockdev_instance(class, minor);

    if (instance)
    {
        release_blockdev_instance(instance);

        return 0;
    }

    instance = malloc(sizeof(blockdev_instance_t));

    if (!instance)
    {
        log_error("[BLOCKDEV] Failed to allocate memory for blockdev instance");
        return -1;
    }

    instance->class = class;
    instance->minor = minor;
    instance->block_size = block_size;
    instance->capacity = capacity;
    instance->ref_count = 0;

    strncpy(instance->description, desc, MAX_DESCRIPTION_LENGTH);
    instance->description[MAX_DESCRIPTION_LENGTH - 1] = 0;

    list_insert(class->instance_list, instance);

    fs_node_t *fnode = malloc(sizeof(fs_node_t));

    memset(fnode, 0, sizeof(fs_node_t));

    fnode->inode = 0;
    sprintf(fnode->name, "atadev%i%i", minor, major);
    fnode->device = instance;
    fnode->uid = 0;
    fnode->gid = 0;
    fnode->permissions = 0660;
    fnode->length = instance->capacity * instance->block_size;
    fnode->flags = FS_BLOCKDEVICE;
    fnode->read = read_blockdev_fs;
    fnode->write = write_blockdev_fs;
    fnode->open = open_blockdev_fs;
    fnode->close = NULL;
    fnode->readdir = NULL;
    fnode->finddir = NULL;
    fnode->ioctl = NULL;

    // TODO: This should map to a unique name like /dev/hda1 and /dev/hda2
    vfs_mount("/dev/hda", fnode);

    return 0;
}

int unreg_blockdev_instance(uint32_t major, uint32_t minor)
{
    blockdev_class_t *class;
    blockdev_instance_t *instance;

    if (major > NUM_BLOCKDEV_CLASSES)
    {
        log_error("[BLOCKDEV] Invalid class");
        return -1;
    }

    class = blockdev_classes[major];

    if (!class)
    {
        log_error("[BLOCKDEV] Class not registred");
        return -1;
    }

    instance = get_blockdev_instance(class, minor);

    if (!instance)
    {
        log_error("[BLOCKDEV] Invalid instance");
        return -1;
    }

    if (instance->ref_count > 1)
    {
        release_blockdev_instance(instance);

        log_error("[BLOCKDEV] Device is busy");
        return -1;
    }

    list_node_t *list_node = list_find(class->instance_list, instance);

    if (list_node)
    {
        list_delete(class->instance_list, list_node);
    }
    else
    {
        log_error("[BLOCKDEV] Consistency error");
        return -1;
    }

    free(instance);

    return 0;
}

uint32_t blockdev_read(unsigned int major, unsigned int minor, uint32_t offset, size_t len, void *buffer)
{
    blockdev_class_t *class;
    blockdev_instance_t *instance;

    size_t block_size;
    size_t delta;

    uint32_t n;
    uint32_t block;
    uint32_t nblocks;

    uint8_t *tmp;
    uint8_t *dest = (uint8_t *)buffer;

    if (major > NUM_BLOCKDEV_CLASSES)
    {
        log_error("[BLOCKDEV] Major class invalid");
        return -1;
    }

    class = blockdev_classes[major];

    if (!class)
    {
        log_error("[BLOCKDEV] Could not find class");
        return -1;
    }

    instance = get_blockdev_instance(class, minor);

    if (!instance)
    {
        log_error("[BLOCKDEV] Could not find instance");
        return -1;
    }

    block_size = instance->block_size;

    block = offset / block_size;
    delta = offset % block_size;

    if (delta)
    {
        tmp = malloc(block_size);

        if (!tmp)
        {
            log_error("[BLOCKDEV] Failed to allocate memory");
            release_blockdev_instance(instance);
            return -1;
        }

        if (!class->read(minor, block, 1, tmp))
        {
            log_error("[BLOCKDEV] Error reading from device");

            free(tmp);
            release_blockdev_instance(instance);
            return -1;
        }

        memcpy(dest, tmp + delta, block_size - delta);

        free(tmp);
        dest += block_size - delta;
        ++block;
    }

    nblocks = (offset + len) / block_size - block;
    delta = (offset + len) % block_size;

    while (nblocks)
    {
        if (!(n = class->read(minor, block, nblocks, dest)))
        {
            log_error("[BLOCKDEV] Read zero blocks");

            release_blockdev_instance(instance);

            return -1;
        }

        dest += n * block_size;
        nblocks -= n;
        block += n;
    }

    if (delta)
    {
        tmp = malloc(block_size);

        if (!tmp)
        {
            log_error("[BLOCKDEV] Error allocating memory");
            release_blockdev_instance(instance);
            return -1;
        }

        if (!class->read(minor, block, 1, tmp))
        {
            log_error("[BLOCKDEV] Error reading from device");

            free(tmp);
            release_blockdev_instance(instance);
            return -1;
        }

        memcpy(dest, tmp, delta);

        free(tmp);
    }

    release_blockdev_instance(instance);

    return 0;
}

uint32_t blockdev_write(unsigned int major, unsigned int minor, uint32_t offset, size_t len, void *buffer)
{
    blockdev_class_t *class;
    blockdev_instance_t *instance;

    size_t block_size;
    size_t delta;

    uint32_t n;
    uint32_t block;
    uint32_t nblocks;

    uint8_t *tmp;
    uint8_t *src = (uint8_t *)buffer;

    if (major > NUM_BLOCKDEV_CLASSES)
    {
        return -1;
    }

    class = blockdev_classes[major];

    if (!class)
    {
        return -1;
    }

    instance = get_blockdev_instance(class, minor);

    if (!instance)
    {
        return -1;
    }

    block_size = instance->block_size;

    block = offset / block_size;
    delta = offset % block_size;

    if (delta)
    {
        tmp = malloc(block_size);

        if (!tmp)
        {
            log_error("[BLOCKDEV] Failed to allocate memory");
            release_blockdev_instance(instance);
            return -1;
        }

        if (!class->read(minor, block, 1, tmp))
        {
            log_error("[BLOCKDEV] Failed to read from device");
            free(tmp);
            release_blockdev_instance(instance);
            return -1;
        }

        memcpy(src, tmp + delta, block_size - delta);

        if (!class->write(minor, block, 1, tmp))
        {
            log_error("[BLOCKDEV] Failed to write to device");
            free(tmp);
            release_blockdev_instance(instance);
            return -1;
        }

        free(tmp);
        src += block_size - delta;
        ++block;
    }

    nblocks = (offset + len) / block_size - block;
    delta = (offset + len) % block_size;

    while (nblocks)
    {
        if (!(n = class->write(minor, block, nblocks, src)))
        {
            log_error("[BLOCKDEV] Error: Could not write to blockdev");
            backtrace();

            release_blockdev_instance(instance);

            return -1;
        }

        src += n * block_size;
        nblocks -= n;
        block += n;
    }

    if (delta)
    {
        tmp = malloc(block_size);

        if (!tmp)
        {
            log_error("[BLOCKDEV] Failed to allocate memory");
            release_blockdev_instance(instance);
            return -1;
        }

        if (!class->read(minor, block, 1, tmp))
        {
            log_error("[BLOCKDEV] Failed to read from device");
            free(tmp);
            release_blockdev_instance(instance);
            return -1;
        }

        memcpy(tmp, src, delta);

        if (!class->write(minor, block, 1, tmp))
        {
            log_error("[BLOCKDEV] Failed to write to device");
            free(tmp);
            release_blockdev_instance(instance);
            return -1;
        }

        free(tmp);
    }

    release_blockdev_instance(instance);

    return 0;
}

//=============================================================================
// End of file
//=============================================================================