#include <drivers/blockdev.h>

#include <arch/arch.h>

#include <util/list.h>

#include <string.h>
#include <stdlib.h>

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

    return 0;
}

int reg_blockdev_instance(uint32_t major, uint32_t minor, const char *desc, size_t block_size, size_t capacity)
{
    blockdev_class_t *class;
    blockdev_instance_t *instance;

    if (major > NUM_BLOCKDEV_CLASSES || !block_size || !capacity)
    {
        // Invalid argument
        return -1;
    }

    class = blockdev_classes[major];

    if (!class)
    {
        // Class not registered
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
        // No memory
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

    return 0;
}

int unreg_blockdev_instance(uint32_t major, uint32_t minor)
{
    blockdev_class_t *class;
    blockdev_instance_t *instance;

    if (major > NUM_BLOCKDEV_CLASSES)
    {
        // Invalid class
        return -1;
    }

    class = blockdev_classes[major];

    if (!class)
    {
        // Class not registered
        return -1;
    }

    instance = get_blockdev_instance(class, minor);

    if (!instance)
    {
        // Invalid instance requested
        return -1;
    }

    if (instance->ref_count > 1)
    {
        release_blockdev_instance(instance);

        // Device is busy
        return -1;
    }

    list_node_t *list_node = list_find(class->instance_list, instance);

    if (list_node)
    {
        list_delete(class->instance_list, list_node);
    }
    else
    {
        // Consisitency error
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
            release_blockdev_instance(instance);
            return -1;
        }

        if (!class->read(minor, block, 1, tmp))
        {
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
            release_blockdev_instance(instance);
            return -1;
        }

        if (!class->read(minor, block, 1, tmp))
        {
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
            release_blockdev_instance(instance);
            return -1;
        }

        if (!class->read(minor, block, 1, tmp))
        {
            free(tmp);
            release_blockdev_instance(instance);
            return -1;
        }

        memcpy(src, tmp + delta, block_size - delta);

        if (!class->write(minor, block, 1, tmp))
        {
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
            release_blockdev_instance(instance);
            return -1;
        }

        if (!class->read(minor, block, 1, tmp))
        {
            free(tmp);
            release_blockdev_instance(instance);
            return -1;
        }

        memcpy(tmp, src, delta);

        if (!class->write(minor, block, 1, tmp))
        {
            free(tmp);
            release_blockdev_instance(instance);
            return -1;
        }

        free(tmp);
    }

    release_blockdev_instance(instance);

    return 0;
}
