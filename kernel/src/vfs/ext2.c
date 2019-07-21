/**
 * @file ext2.c
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-04-07
 * 
 * @brief EXT2 file system driver
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

#include <vfs/ext2.h>

#include <cmos/cmos_rtc.h>

#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <debug/backtrace.h>

#define DEBUG_EXT2 1

#if DEBUG_EXT2 == 1

#define debug_printf(...) printf(__VA_ARGS__)

#else

#define debug_printf(...)

#endif

//=============================================================================
//=============================================================================
// Forward declarations
//=============================================================================
//=============================================================================

//=============================================================================
// Block cache
//=============================================================================

static uint32_t get_cache_time(ext2_fs_t *this);
static int cache_flush_dirty(ext2_fs_t *this, uint32_t entry);

//=============================================================================
// Superblock
//=============================================================================

static int rewrite_superblock(ext2_fs_t *this);

//=============================================================================
// Block IO
//=============================================================================

static int read_block(ext2_fs_t *this, uint32_t block_no, uint8_t *buffer);
static int write_block(ext2_fs_t *this, uint32_t block_no, uint8_t *buffer);
static int ext2_sync(ext2_fs_t *this);
static int set_block_number(ext2_fs_t *this, ext2_inodetable_t *inode,
                            uint32_t inode_no, uint32_t iblock,
                            uint32_t rblock);
static int get_block_number(ext2_fs_t *this, ext2_inodetable_t *inode,
                            uint32_t iblock);
static int allocate_block(ext2_fs_t *this);

//=============================================================================
// Inode
//=============================================================================

static int write_inode(ext2_fs_t *this, ext2_inodetable_t *inode,
                       uint32_t index);
static int allocate_inode_block(ext2_fs_t *this, ext2_inodetable_t *inode,
                                uint32_t inode_no, uint32_t block);
static uint32_t inode_read_block(ext2_fs_t *this, ext2_inodetable_t *inode,
                                 uint32_t block, uint8_t *buffer);
static uint32_t inode_write_block(ext2_fs_t *this, ext2_inodetable_t *inode,
                                  uint32_t inode_no, uint32_t block,
                                  uint8_t *buffer);
static uint32_t allocate_inode(ext2_fs_t *this);
static void refresh_inode(ext2_fs_t *this, ext2_inodetable_t *inodet,
                          uint32_t inode);
static ext2_inodetable_t *read_inode(ext2_fs_t *this, uint32_t inode);
static uint32_t write_inode_buffer(ext2_fs_t *this, ext2_inodetable_t *inode,
                                   uint32_t inode_number, uint64_t offset,
                                   uint32_t size, uint8_t *buffer);
static int create_entry(fs_node_t *parent, char *name, uint32_t inode);
static uint32_t node_from_file(fs_node_t *this, ext2_inodetable_t *inode,
                               ext2_dir_t *direntry, fs_node_t *fnode);

//=============================================================================
// VFS Operations
//=============================================================================

static int mkdir_ext2(fs_node_t *parent, char *name, uint16_t permission);
static int create_ext2(fs_node_t *parent, char *name, uint16_t permission);
static int chmod_ext2(fs_node_t *parent, int mode);
static ext2_dir_t *direntry_ext2(ext2_fs_t *this, ext2_inodetable_t *inode,
                                 uint32_t no, uint32_t index);
static fs_node_t *finddir_ext2(fs_node_t *node, char *name);
static int unlink_ext2(fs_node_t *node, char *name);
static uint32_t read_ext2(fs_node_t *node, uint64_t offset, uint32_t size,
                          uint8_t *buffer);
static uint32_t write_ext2(fs_node_t *node, uint64_t offset, uint32_t size,
                           uint8_t *buffer);
static int truncate_ext2(fs_node_t *node);
static void open_ext2(fs_node_t *node, uint32_t flags);
static void close_ext2(fs_node_t *node);
static struct dirent *readdir_ext2(fs_node_t *node, uint32_t index);
static int symlink_ext2(fs_node_t *parent, char *target, char *name);
static int readlink_ext2(fs_node_t *node, char *buffer, size_t size);

//=============================================================================
// Mount filesystem
//=============================================================================

static uint32_t ext2_root(ext2_fs_t *this, ext2_inodetable_t *inode,
                          fs_node_t *fnode);
static fs_node_t *mount_ext2(fs_node_t *block_device, int flags);

//=============================================================================
//=============================================================================
// Definitions
//=============================================================================
//=============================================================================

#define BGDS (this->block_group_count)
#define SB (this->superblock)
#define BGD (this->block_groups)
#define RN (this->root_node)
#define DC (this->disk_cache)

#define BLOCKBIT(n) (bg_buffer[((n) >> 3)] & (1 << (((n) % 8))))
#define BLOCKBYTE(n) (bg_buffer[((n) >> 3)])
#define SETBIT(n) (1 << (((n) % 8)))

#undef _symlink
#define _symlink(inode) ((char *)(inode)->block)

//=============================================================================
//=============================================================================
// Implementation
//=============================================================================
//=============================================================================

//=============================================================================
// Block cache
//=============================================================================

static uint32_t get_cache_time(ext2_fs_t *this)
{
    return this->cache_time++;
}

static int cache_flush_dirty(ext2_fs_t *this, uint32_t entry)
{
    //printf("[EXT2] cache_flush_dirty\n");

    write_fs(this->block_device,
             (DC[entry].block_no) * this->block_size,
             this->block_size,
             (uint8_t *)(DC[entry].block));

    DC[entry].dirty = 0;

    return 0;
}

//=============================================================================
// Superblock
//=============================================================================

static int rewrite_superblock(ext2_fs_t *this)
{
    write_fs(this->block_device,
             1024,
             sizeof(ext2_superblock_t),
             (uint8_t *)SB);

    return 0;
}

//=============================================================================
// Block IO
//=============================================================================

static int read_block(ext2_fs_t *this, uint32_t block_no, uint8_t *buffer)
{
    //printf("[EXT2] read_block: block_no: %i, buffer: %#016x\n", block_no, buffer);

    if (!block_no)
    {
        //printf("Block_no = 0\n");
        return -1;
    }

    spinlock_lock(&this->lock);

    if (!DC)
    {
        //printf("[EXT2] No disk cache\n");

        int ret;

        ret = read_fs(this->block_device,
                      block_no * this->block_size,
                      this->block_size,
                      (uint8_t *)buffer);

        if (ret != 0)
        {
            printf("[EXT2] Error reading block from block device\n");
        }

        spinlock_unlock(&this->lock);

        return ret;
    }

    int oldest = -1;

    unsigned int oldest_age = UINT32_MAX;

    for (unsigned int i = 0; i < this->cache_entries; ++i)
    {
        if (DC[i].block_no == block_no)
        {
            DC[i].last_use = get_cache_time(this);

            memcpy(buffer, DC[i].block, this->block_size);

            spinlock_unlock(&this->lock);

            return 0;
        }

        if (DC[i].last_use < oldest_age)
        {
            oldest = i;
            oldest_age = DC[i].last_use;
        }
    }

    if (DC[oldest].dirty)
    {
        cache_flush_dirty(this, oldest);
    }

    int ret;

    ret = read_fs(this->block_device,
                  block_no * this->block_size,
                  this->block_size,
                  (uint8_t *)DC[oldest].block);

    if (ret == 0)
    {
        memcpy(buffer, DC[oldest].block, this->block_size);

        DC[oldest].block_no = block_no;
        DC[oldest].last_use = get_cache_time(this);
        DC[oldest].dirty = 0;
    }

    spinlock_unlock(&this->lock);

    return 0;
}

static int write_block(ext2_fs_t *this, uint32_t block_no, uint8_t *buffer)
{
    if (!block_no)
    {
        printf("[EXT2] Error: Invalid block number\n");
        backtrace();
        return -1;
    }

    spinlock_unlock(&this->lock);

    if (!DC)
    {
        write_fs(this->block_device,
                 block_no * this->block_size,
                 this->block_size,
                 buffer);

        spinlock_unlock(&this->lock);

        return 0;
    }

    int oldest = -1;

    unsigned int oldest_age = UINT32_MAX;

    for (unsigned int i = 0; i < this->cache_entries; ++i)
    {
        if (DC[i].block_no == block_no)
        {
            DC[i].last_use = get_cache_time(this);

            memcpy(DC[i].block, buffer, this->block_size);

            spinlock_unlock(&this->lock);

            return 0;
        }

        if (DC[i].last_use < oldest_age)
        {
            oldest = i;
            oldest_age = DC[i].last_use;
        }
    }

    if (DC[oldest].dirty)
    {
        cache_flush_dirty(this, oldest);
    }

    memcpy(DC[oldest].block, buffer, this->block_size);

    DC[oldest].block_no = block_no;
    DC[oldest].last_use = get_cache_time(this);
    DC[oldest].dirty = 0;

    spinlock_unlock(&this->lock);

    return 0;
}

static int ext2_sync(ext2_fs_t *this)
{
    if (!this->disk_cache)
    {
        return 0;
    }

    spinlock_lock(&this->lock);

    for (unsigned int i = 0; i < this->cache_entries; ++i)
    {
        if (DC[i].dirty)
        {
            cache_flush_dirty(this, i);
        }
    }

    spinlock_unlock(&this->lock);

    return 0;
}

static int set_block_number(ext2_fs_t *this, ext2_inodetable_t *inode,
                            uint32_t inode_no, uint32_t iblock,
                            uint32_t rblock)
{
    uint32_t p = this->pointers_per_block;

    uint32_t a;
    uint32_t b;
    uint32_t c;
    uint32_t d;
    uint32_t e;
    uint32_t f;
    uint32_t g;

    uint8_t *tmp;

    if (iblock < EXT2_DIRECT_BLOCKS)
    {
        inode->block[iblock] = rblock;

        return 0;
    }

    if (iblock < EXT2_DIRECT_BLOCKS + p)
    {
        if (!inode->block[EXT2_DIRECT_BLOCKS])
        {
            uint32_t block_no = allocate_block(this);

            if (!block_no)
            {
                printf("[EXT2] Error: Invalid block number\n");
                backtrace();
                return -1;
            }

            inode->block[EXT2_DIRECT_BLOCKS] = block_no;

            write_inode(this, inode, inode_no);
        }

        tmp = malloc(this->block_size);

        if (!tmp)
        {
            return -1;
        }

        read_block(this,
                   inode->block[EXT2_DIRECT_BLOCKS],
                   tmp);

        ((uint32_t *)tmp)[iblock - EXT2_DIRECT_BLOCKS] = rblock;

        write_block(this,
                    inode->block[EXT2_DIRECT_BLOCKS],
                    tmp);

        free(tmp);

        return 1;
    }

    if (iblock < EXT2_DIRECT_BLOCKS + p + p * p)
    {
        a = iblock - EXT2_DIRECT_BLOCKS;

        b = a - p;

        c = b / p;

        d = b - c * p;

        if (!inode->block[EXT2_DIRECT_BLOCKS + 1])
        {
            uint32_t block_no = allocate_block(this);

            if (!block_no)
            {
                printf("[EXT2] Error: Invalid block number\n");
                backtrace();
                return -1;
            }

            inode->block[EXT2_DIRECT_BLOCKS + 1] = block_no;

            write_inode(this, inode, inode_no);
        }

        tmp = malloc(this->block_size);

        if (!tmp)
        {
            printf("[EXT2] Error: Could not allocate memory\n");
            backtrace();
            return -1;
        }

        read_block(this,
                   inode->block[EXT2_DIRECT_BLOCKS + 1],
                   (uint8_t *)tmp);

        if (!((uint32_t *)tmp)[c])
        {
            uint32_t block_no = allocate_block(this);

            if (!block_no)
            {
                goto no_space_free;
            }

            ((uint32_t *)tmp)[c] = block_no;

            write_block(this,
                        inode->block[EXT2_DIRECT_BLOCKS + 1],
                        (uint8_t *)tmp);
        }

        uint32_t nblock = ((uint32_t *)tmp)[c];
        read_block(this, nblock, (uint8_t *)tmp);

        ((uint32_t *)tmp)[d] = rblock;
        write_block(this, nblock, (uint8_t *)tmp);

        free(tmp);

        return 0;
    }

    if (iblock < EXT2_DIRECT_BLOCKS + p + p * p + p)
    {
        a = iblock - EXT2_DIRECT_BLOCKS;

        b = a - p;

        c = b - p * p;

        d = c / (p * p);

        e = c - d * p * p;

        f = e / p;

        g = e - f * p;

        if (!inode->block[EXT2_DIRECT_BLOCKS + 2])
        {
            unsigned int block_no = allocate_block(this);

            if (!block_no)
            {
                printf("[EXT2] Error: Invalid block number\n");
                backtrace();
                return -1;
            }

            inode->block[EXT2_DIRECT_BLOCKS + 2] = block_no;

            write_inode(this, inode, inode_no);
        }

        tmp = malloc(this->block_size);

        if (!tmp)
        {
            return -1;
        }

        read_block(this,
                   inode->block[EXT2_DIRECT_BLOCKS + 2],
                   (uint8_t *)tmp);

        if (!((uint32_t *)tmp)[d])
        {
            unsigned int block_no = allocate_block(this);

            if (!block_no)
            {
                goto no_space_free;
            }

            ((uint32_t *)tmp)[d] = block_no;

            write_block(this,
                        inode->block[EXT2_DIRECT_BLOCKS + 2],
                        (uint8_t *)tmp);
        }

        uint32_t nblock = ((uint32_t *)tmp)[d];

        read_block(this, nblock, (uint8_t *)tmp);

        if (!((uint32_t *)tmp)[f])
        {
            unsigned int block_no = allocate_block(this);

            if (!block_no)
            {
                goto no_space_free;
            }

            ((uint32_t *)tmp)[f] = block_no;

            write_block(this, nblock, (uint8_t *)tmp);
        }

        nblock = ((uint32_t *)tmp)[f];

        read_block(this, nblock, (uint8_t *)tmp);

        ((uint32_t *)tmp)[g] = nblock;

        write_block(this, nblock, (uint8_t *)tmp);

        free(tmp);

        return 0;
    }

    debug_printf("[EXT2] Tried to access too high block number\n");

    return -1;

no_space_free:

    printf("[EXT2] Error: No free space\n");
    backtrace();

    free(tmp);

    return -1;
}

static int get_block_number(ext2_fs_t *this, ext2_inodetable_t *inode,
                            uint32_t iblock)
{
    uint32_t p = this->pointers_per_block;

    uint32_t a;
    uint32_t b;
    uint32_t c;
    uint32_t d;
    uint32_t e;
    uint32_t f;
    uint32_t g;

    uint8_t *tmp;

    if (iblock < EXT2_DIRECT_BLOCKS)
    {
        return inode->block[iblock];
    }
    else if (iblock < EXT2_DIRECT_BLOCKS + p)
    {
        tmp = malloc(this->block_size);

        if (!tmp)
        {
            printf("[EXT2] Error: Could not allocate memory\n");
            backtrace();
            return -1;
        }

        read_block(this, inode->block[EXT2_DIRECT_BLOCKS], (uint8_t *)tmp);

        uint32_t out = ((uint32_t *)tmp)[iblock - EXT2_DIRECT_BLOCKS];

        free(tmp);

        return out;
    }
    else if (iblock < EXT2_DIRECT_BLOCKS + p + p * p)
    {
        a = iblock - EXT2_DIRECT_BLOCKS;
        b = a - p;
        c = b / p;
        d = b - c * p;

        tmp = malloc(this->block_size);

        if (!tmp)
        {
            printf("[EXT2] Error: Could not allocate memory\n");
            backtrace();
            return -1;
        }

        read_block(this, inode->block[EXT2_DIRECT_BLOCKS + 1], (uint8_t *)tmp);

        uint32_t nblock = ((uint32_t *)tmp)[c];

        read_block(this, nblock, (uint8_t *)tmp);

        uint32_t out = ((uint32_t *)tmp)[d];

        free(tmp);

        return out;
    }
    else if (iblock < EXT2_DIRECT_BLOCKS + p + p * p + p)
    {
        a = iblock - EXT2_DIRECT_BLOCKS;
        b = a - p;
        c = b - p * p;
        d = c / (p * p);
        e = c - d * p * p;
        f = e / p;
        g = e - f * p;

        tmp = malloc(this->block_size);

        if (!tmp)
        {
            printf("[EXT2] Error: Could not allocate memory\n");
            backtrace();
            return -1;
        }

        read_block(this, inode->block[EXT2_DIRECT_BLOCKS + 2], (uint8_t *)tmp);

        uint32_t nblock = ((uint32_t *)tmp)[d];

        read_block(this, nblock, (uint8_t *)tmp);

        nblock = ((uint32_t *)tmp)[f];

        read_block(this, nblock, (uint8_t *)tmp);

        uint32_t out = ((uint32_t *)tmp)[g];

        free(tmp);

        return out;
    }

    debug_printf("[EXT2] Error: tried to access too high block number\n");
    backtrace();
    return -1;
}

static int allocate_block(ext2_fs_t *this)
{
    uint32_t block_no;
    uint32_t block_offset = 0;
    uint32_t group = 0;

    uint8_t *bg_buffer = malloc(this->block_size);

    for (uint32_t i = 0; i < BGDS; ++i)
    {
        if (BGD[i].free_blocks_count > 0)
        {
            read_block(this, BGD[i].block_bitmap, (uint8_t *)bg_buffer);

            while (BLOCKBIT(block_offset))
            {
                ++block_offset;
            }

            block_no = block_offset + SB->blocks_per_group * i;

            group = i;

            break;
        }
    }

    if (!block_no)
    {
        printf("[EXT2] Error: Invalid block number\n");
        backtrace();
        free(bg_buffer);

        return -1;
    }

    BLOCKBYTE(block_offset) |= SETBIT(block_offset);

    write_block(this,
                BGD[group].block_bitmap,
                (uint8_t *)bg_buffer);

    BGD[group].free_blocks_count--;

    for (int i = 0; i < this->bgd_block_span; ++i)
    {
        write_block(this,
                    this->bgd_offset + i,
                    (uint8_t *)((uint64_t)BGD + this->block_size * i));
    }

    SB->free_blocks_count--;
    rewrite_superblock(this);

    memset(bg_buffer, 0x00, this->block_size);

    write_block(this, block_no, bg_buffer);

    free(bg_buffer);

    return block_no;
}

//=============================================================================
// Inode
//=============================================================================

static int write_inode(ext2_fs_t *this, ext2_inodetable_t *inode,
                       uint32_t index)
{
    uint32_t group = index / this->inodes_per_group;

    if (group > BGDS)
    {
        return -1;
    }

    uint32_t inode_table_block = BGD[group].inode_table;

    index -= group * this->inodes_per_group;

    uint32_t block_offset = ((index - 1) * this->inode_size) /
                            this->block_size;
    uint32_t offset_in_block = (index - 1) -
                               block_offset *
                                   (this->block_size / this->inode_size);

    ext2_inodetable_t *inodet = malloc(this->block_size);

    read_block(this,
               inode_table_block + block_offset,
               (uint8_t *)inodet);

    memcpy((uint8_t *)((uint64_t)inodet + offset_in_block * this->inode_size),
           inode,
           this->inode_size);

    write_block(this,
                inode_table_block + block_offset,
                (uint8_t *)inodet);

    free(inodet);

    return 0;
}

static int allocate_inode_block(ext2_fs_t *this, ext2_inodetable_t *inode,
                                uint32_t inode_no, uint32_t block)
{

    uint32_t block_no = allocate_block(this);

    if (!block_no)
    {
        printf("[EXT2] Error: Could not allocate block\n");
        backtrace();
        return -1;
    }

    set_block_number(this,
                     inode,
                     inode_no,
                     block,
                     block_no);

    uint32_t t = (block + 1) * (this->block_size / 512);

    if (inode->blocks < t)
    {
        inode->blocks = t;
    }

    write_inode(this, inode, inode_no);

    return 0;
}

static uint32_t inode_read_block(ext2_fs_t *this, ext2_inodetable_t *inode,
                                 uint32_t block, uint8_t *buffer)
{
    //printf("inode_read_block: block: %i, inode->blocks: %i, this->block_size: %i\n", block, inode->blocks, this->block_size);
    if (block >= inode->blocks / (this->block_size / 512))
    {
        memset(buffer, 0x00, this->block_size);

        //debug_printf("[EXT2] Error: attempting to read beyond the allocated nodes for this inode\n");

        //backtrace();

        return 0;
    }

    unsigned int real_block = get_block_number(this, inode, block);

    read_block(this, real_block, buffer);

    return real_block;
}

static uint32_t inode_write_block(ext2_fs_t *this, ext2_inodetable_t *inode,
                                  uint32_t inode_no, uint32_t block,
                                  uint8_t *buffer)
{
    //printf("inode_write_block: block: %i, inode->blocks: %i, this->block_size: %i\n", block, inode->blocks, this->block_size);
    if (block >= inode->blocks / (this->block_size / 512))
    {
        //return 0;
    }

    char *empty = NULL;

    while (block >= inode->blocks / (this->block_size / 512))
    {
        allocate_inode_block(this,
                             inode,
                             inode_no,
                             inode->blocks / (this->block_size / 512));

        refresh_inode(this, inode, inode_no);
    }

    if (empty)
    {
        free(empty);
    }

    unsigned int real_block = get_block_number(this, inode, block);

    write_block(this, real_block, buffer);

    return real_block;
}

static uint32_t allocate_inode(ext2_fs_t *this)
{
    uint32_t node_no = 0;
    uint32_t node_offset = 0;
    uint32_t group = 0;
    uint8_t *bg_buffer = malloc(this->block_size);

    for (uint32_t i = 0; i < BGDS; ++i)
    {
        if (BGD[i].free_inodes_count > 0)
        {
            read_block(this, BGD[i].inode_bitmap, (uint8_t *)bg_buffer);

            while (BLOCKBIT(node_offset))
            {
                node_offset++;
            }

            node_no = node_offset + i * this->inodes_per_group + 1;

            group = i;

            break;
        }
    }

    if (!node_no)
    {
        printf("[EXT2] Error: Invalid node_no\n");
        backtrace();
        return -1;
    }

    BLOCKBYTE(node_offset) |= SETBIT(node_offset);

    write_block(this,
                BGD[group].inode_bitmap,
                (uint8_t *)bg_buffer);

    free(bg_buffer);

    BGD[group].free_inodes_count--;

    for (int i = 0; i < this->bgd_block_span; ++i)
    {
        write_block(this,
                    this->bgd_offset + i,
                    (uint8_t *)((uint64_t)BGD + this->block_size * i));
    }

    SB->free_inodes_count--;

    rewrite_superblock(this);

    return node_no;
}

static void refresh_inode(ext2_fs_t *this, ext2_inodetable_t *inodet, uint32_t inode)
{

    uint32_t group = inode / this->inodes_per_group;

    //printf("Group: %i, inode: %i, inodes per group: %i\n", group, inode, this->inodes_per_group);

    if (group > BGDS)
    {
        printf("[EXT2] Error: Invalid group\n");
        backtrace();
        return;
    }

    uint32_t inode_table_block = BGD[group].inode_table;

    inode -= group * this->inodes_per_group;

    uint32_t block_offset = ((inode - 1) * this->inode_size) / this->block_size;
    uint32_t offset_in_block = (inode - 1) - block_offset * (this->block_size / this->inode_size);

    uint8_t *buf = malloc(this->block_size);

    memset(buf, 0, this->block_size);

    //printf("inode_table_block: %#016x\n", inode_table_block);

    read_block(this,
               inode_table_block + block_offset,
               buf);

    ext2_inodetable_t *inodes = (ext2_inodetable_t *)buf;

    //printf("inodet: %#016x\n", inodet);
    //printf("Block offset: %i, Offset in block: %i\n", block_offset, offset_in_block);

    memcpy(inodet,
           (uint8_t *)((uint64_t)inodes + offset_in_block * this->inode_size),
           this->inode_size);

    free(buf);
}

static ext2_inodetable_t *read_inode(ext2_fs_t *this, uint32_t inode)
{
    //printf("[EXT2] read_inode: inode: %i\n", inode);

    ext2_inodetable_t *inodet = malloc(this->inode_size);

    refresh_inode(this, inodet, inode);

    return inodet;
}

static uint32_t write_inode_buffer(ext2_fs_t *this, ext2_inodetable_t *inode,
                                   uint32_t inode_number, uint64_t offset,
                                   uint32_t size, uint8_t *buffer)

{
    uint32_t end = offset + size;

    if (end > inode->size)
    {
        inode->size = end;

        write_inode(this,
                    inode,
                    inode_number);
    }

    uint32_t start_block = offset / this->block_size;
    uint32_t end_block = end / this->block_size;
    uint32_t end_size = end - end_block * this->block_size;
    uint32_t size_to_read = end - offset;

    uint8_t *buf = malloc(this->block_size);

    if (start_block == end_block)
    {
        inode_read_block(this,
                         inode,
                         start_block,
                         buf);

        memcpy((uint8_t *)(((uint64_t)buf) +
                           ((uintptr_t)offset % this->block_size)),
               buffer,
               size_to_read);

        inode_write_block(this,
                          inode,
                          inode_number,
                          start_block,
                          buf);
    }
    else
    {
        uint32_t block_offset;
        uint32_t blocks_read = 0;

        for (block_offset = start_block; block_offset < end_block;
             block_offset++, blocks_read++)
        {
            if (block_offset == start_block)
            {
                int b = inode_read_block(this,
                                         inode, block_offset,
                                         buf);

                memcpy((uint8_t *)(((uint64_t)buf) +
                                   ((uintptr_t)offset % this->block_size)),
                       buffer,
                       this->block_size - (offset % this->block_size));

                inode_write_block(this,
                                  inode,
                                  inode_number,
                                  block_offset,
                                  buf);
                if (!b)
                {
                    refresh_inode(this,
                                  inode,
                                  inode_number);
                }
            }
            else
            {
                int b = inode_read_block(
                    this,
                    inode,
                    block_offset,
                    buf);

                memcpy(buf,
                       buffer + this->block_size * blocks_read -
                           (offset % this->block_size),
                       this->block_size);

                inode_write_block(this,
                                  inode,
                                  inode_number,
                                  block_offset,
                                  buf);

                if (!b)
                {

                    refresh_inode(this,
                                  inode,
                                  inode_number);
                }
            }
        }

        if (end_size)
        {
            inode_read_block(this,
                             inode,
                             end_block,
                             buf);

            memcpy(buf,
                   buffer + this->block_size * blocks_read -
                       (offset % this->block_size),
                   end_size);

            inode_write_block(this,
                              inode,
                              inode_number,
                              end_block,
                              buf);
        }
    }
    free(buf);

    return size_to_read;
}

static int create_entry(fs_node_t *parent, char *name, uint32_t inode)
{
    ext2_fs_t *this = (ext2_fs_t *)parent->device;

    ext2_inodetable_t *pinode = read_inode(this, parent->inode);

    if (((pinode->mode & EXT2_S_IFDIR) == 0) || (name == NULL))
    {
        printf("[EXT2] Error: Invalid name\n");
        backtrace();
        return -1;
    }

    uint32_t rec_len = sizeof(ext2_dir_t) + strlen(name);

    rec_len += (rec_len % 4) ? (4 - (rec_len % 4)) : 0;

    uint8_t *block = malloc(this->block_size);
    uint8_t block_nr = 0;
    uint32_t dir_offset = 0;
    uint32_t total_offset = 0;

    int modify_or_replace = 0;

    ext2_dir_t *previous;

    inode_read_block(this,
                     pinode,
                     block_nr,
                     block);

    while (total_offset < pinode->size)
    {
        if (dir_offset >= this->block_size)
        {

            block_nr++;

            dir_offset -= this->block_size;

            inode_read_block(this, pinode, block_nr, block);
        }

        ext2_dir_t *d_ent = (ext2_dir_t *)((uintptr_t)block + dir_offset);

        unsigned int sreclen = d_ent->name_len + sizeof(ext2_dir_t);

        sreclen += (sreclen % 4) ? (4 - (sreclen % 4)) : 0;

        {
            char f[d_ent->name_len + 1];

            memcpy(f, d_ent->name, d_ent->name_len);

            f[d_ent->name_len] = 0;
        }

        if (d_ent->rec_len != sreclen &&
            total_offset + d_ent->rec_len == pinode->size)
        {

            dir_offset += sreclen;
            total_offset += sreclen;

            modify_or_replace = 1;
            previous = d_ent;

            break;
        }

        if (d_ent->inode == 0)
        {
            modify_or_replace = 2;
        }

        dir_offset += d_ent->rec_len;
        total_offset += d_ent->rec_len;
    }

    if (!modify_or_replace)
    {
        printf("[EXT2] Error: Not modify or replace\n");
        backtrace();
        return -1;
    }

    if (modify_or_replace == 1)
    {
        if (dir_offset + rec_len >= this->block_size)
        {
            printf("[EXT2] Error: Access outside block size\n");
            backtrace();
            free(block);
            return -1;
        }
        else
        {
            uint32_t sreclen = previous->name_len + sizeof(ext2_dir_t);
            sreclen += (sreclen % 4) ? (4 - (sreclen % 4)) : 0;
            previous->rec_len = sreclen;
        }
    }
    else if (modify_or_replace == 2)
    {
    }

    ext2_dir_t *d_ent = (ext2_dir_t *)((uintptr_t)block + dir_offset);

    d_ent->inode = inode;
    d_ent->rec_len = this->block_size - dir_offset;
    d_ent->name_len = strlen(name);
    d_ent->file_type = 0;
    memcpy(d_ent->name, name, strlen(name));

    inode_write_block(this, pinode, parent->inode, block_nr, block);

    free(block);
    free(pinode);

    return 0;
}

static uint32_t node_from_file(fs_node_t *this, ext2_inodetable_t *inode,
                               ext2_dir_t *direntry, fs_node_t *fnode)
{
    if (!fnode)
    {
        printf("[EXT2] node_from_file: Invalid fnode\n");
        return 0;
    }

    fnode->device = (void *)this;
    fnode->inode = direntry->inode;

    memcpy(&fnode->name, &direntry->name, direntry->name_len);
    fnode->name[direntry->name_len] = '\0';

    fnode->uid = inode->uid;
    fnode->gid = inode->gid;
    fnode->length = inode->size;
    fnode->permissions = inode->mode & 0xFFF;
    fnode->nlink = inode->links_count;

    fnode->flags = 0;

    if ((inode->mode & EXT2_S_IFREG) == EXT2_S_IFREG)
    {
        fnode->flags |= FS_FILE;
        fnode->read = read_ext2;
        fnode->write = write_ext2;
        fnode->create = NULL;
        fnode->mkdir = NULL;
        fnode->readdir = NULL;
        fnode->finddir = NULL;
        fnode->symlink = NULL;
        fnode->readlink = NULL;
        fnode->truncate = truncate_ext2;
    }

    if ((inode->mode & EXT2_S_IFDIR) == EXT2_S_IFDIR)
    {
        fnode->flags |= FS_DIRECTORY;
        fnode->create = create_ext2;
        fnode->mkdir = mkdir_ext2;
        fnode->readdir = readdir_ext2;
        fnode->finddir = finddir_ext2;
        fnode->unlink = unlink_ext2;
        fnode->write = NULL;
        fnode->symlink = symlink_ext2;
        fnode->readlink = NULL;
    }

    if ((inode->mode & EXT2_S_IFBLK) == EXT2_S_IFBLK)
    {
        fnode->flags |= FS_BLOCKDEVICE;
    }

    if ((inode->mode & EXT2_S_IFCHR) == EXT2_S_IFCHR)
    {
        fnode->flags |= FS_CHARDEVICE;
    }

    if ((inode->mode & EXT2_S_IFIFO) == EXT2_S_IFIFO)
    {
        fnode->flags |= FS_PIPE;
    }

    if ((inode->mode & EXT2_S_IFLNK) == EXT2_S_IFLNK)
    {
        fnode->flags |= FS_SYMLINK;
        fnode->read = NULL;
        fnode->write = NULL;
        fnode->create = NULL;
        fnode->mkdir = NULL;
        fnode->readdir = NULL;
        fnode->finddir = NULL;
        fnode->readlink = readlink_ext2;
    }

    fnode->atime = inode->atime;
    fnode->mtime = inode->mtime;
    fnode->ctime = inode->ctime;

    fnode->chmod = chmod_ext2;
    fnode->open = open_ext2;
    fnode->close = close_ext2;
    fnode->ioctl = NULL;

    return 1;
}

//=============================================================================
// VFS Operations
//=============================================================================

static int mkdir_ext2(fs_node_t *parent, char *name, uint16_t permission)
{
    if (!name)
    {
        return -1;
    }

    ext2_fs_t *this = parent->device;

    fs_node_t *check = finddir_ext2(parent, name);

    if (check)
    {
        free(check);

        return -1;
    }

    uint32_t inode_no = allocate_inode(this);

    ext2_inodetable_t *inode = read_inode(this, inode_no);

    ktime_t ktime;

    RTC_get_time(&ktime);

    uint32_t current_time = RTC_time_to_int(&ktime);

    inode->atime = current_time;
    inode->ctime = current_time;
    inode->mtime = current_time;
    inode->dtime = 0;

    memset(inode->block, 0, sizeof(inode->block));

    inode->blocks = 0;
    inode->size = 0;

    // TODO: UID
    inode->uid = 0;
    inode->gid = 0;

    inode->faddr = 0;
    inode->links_count = 2;
    inode->flags = 0;
    inode->osd1 = 0;
    inode->generation = 0;
    inode->file_acl = 0;
    inode->dir_acl = 0;

    inode->mode = EXT2_S_IFDIR;
    inode->mode |= 0xFFF & permission;

    memset(inode->osd2, 0x00, sizeof(inode->osd2));

    write_inode(this, inode, inode_no);

    create_entry(parent, name, inode_no);

    inode->size = this->block_size;
    write_inode(this, inode, inode_no);

    uint8_t *tmp = malloc(this->block_size);
    ext2_dir_t *t = malloc(12);

    memset(t, 0, 12);

    t->inode = inode_no;
    t->rec_len = 12;
    t->name_len = 1;
    t->name[0] = '.';

    memcpy(&tmp[0], t, 12);

    t->inode = parent->inode;
    t->name_len = 2;
    t->name[1] = '.';
    t->rec_len = this->block_size - 12;

    memcpy(&tmp[12], t, 12);
    free(t);

    inode_write_block(this, inode, inode_no, 0, tmp);

    free(inode);
    free(tmp);

    ext2_inodetable_t *pinode = read_inode(this, parent->inode);

    pinode->links_count++;

    write_inode(this, pinode, parent->inode);

    free(pinode);

    uint32_t group = inode_no / this->inodes_per_group;

    BGD[group].used_dirs_count++;

    for (int i = 0; i < this->bgd_block_span; ++i)
    {
        write_block(this,
                    this->bgd_offset + i,
                    (uint8_t *)((uint64_t)BGD + this->block_size * i));
    }

    ext2_sync(this);

    return 0;
}

static int create_ext2(fs_node_t *parent, char *name, uint16_t permission)
{
    if (!name)
    {
        printf("[EXT2] Error: Invalid name\n");
        backtrace();
        return -1;
    }

    ext2_fs_t *this = parent->device;

    fs_node_t *check = finddir_ext2(parent, name);

    if (check)
    {
        printf("[EXT2] Error: File already created\n");
        backtrace();

        free(check);

        return -1;
    }

    uint32_t inode_no = allocate_inode(this);

    ext2_inodetable_t *inode = read_inode(this, inode_no);

    ktime_t ktime;

    RTC_get_time(&ktime);

    uint32_t current_time = RTC_time_to_int(&ktime);

    inode->atime = current_time;
    inode->ctime = current_time;
    inode->mtime = current_time;
    inode->dtime = 0;

    memset(inode->block, 0x00, sizeof(inode->block));
    inode->blocks = 0;
    inode->size = 0;

    // TODO: UID
    inode->uid = 0;
    inode->gid = 0;

    inode->faddr = 0;
    inode->links_count = 1;
    inode->flags = 0;
    inode->osd1 = 0;
    inode->generation = 0;
    inode->file_acl = 0;
    inode->dir_acl = 0;

    inode->mode = EXT2_S_IFREG;
    inode->mode |= 0xFFF & permission;

    memset(inode->osd2, 0x00, sizeof(inode->osd2));

    write_inode(this, inode, inode_no);

    create_entry(parent, name, inode_no);

    free(inode);

    ext2_sync(this);

    return 0;
}

static int chmod_ext2(fs_node_t *parent, int mode)
{
    ext2_fs_t *this = parent->device;

    ext2_inodetable_t *inode = read_inode(this, parent->inode);

    inode->mode = (inode->mode & 0xFFFFF000) | mode;

    write_inode(this, inode, parent->inode);

    ext2_sync(this);

    return 0;
}

static ext2_dir_t *direntry_ext2(ext2_fs_t *this, ext2_inodetable_t *inode,
                                 uint32_t no, uint32_t index)
{
    (void)no;

    uint8_t *block = malloc(this->block_size);
    uint8_t block_nr = 0;

    inode_read_block(this,
                     inode,
                     block_nr,
                     block);

    uint32_t dir_offset = 0;
    uint32_t total_offset = 0;
    uint32_t dir_index = 0;

    while (total_offset < inode->size && dir_index <= index)
    {
        ext2_dir_t *d_ent = (ext2_dir_t *)((uintptr_t)block + dir_offset);

        if (d_ent->inode != 0 && dir_index == index)
        {
            ext2_dir_t *out = malloc(d_ent->rec_len);
            memcpy(out, d_ent, d_ent->rec_len);
            free(block);
            return out;
        }

        dir_offset += d_ent->rec_len;
        total_offset += d_ent->rec_len;

        if (d_ent->inode)
        {
            dir_index++;
        }

        if (dir_offset >= this->block_size)
        {
            block_nr++;
            dir_offset -= this->block_size;
            inode_read_block(this, inode, block_nr, block);
        }
    }

    free(block);
    return NULL;
}

static fs_node_t *finddir_ext2(fs_node_t *node, char *name)
{
    //printf("[EXT2] finddir_ext2: node->name: [%s], name: [%s]\n", node->name, name);

    ext2_fs_t *this = (ext2_fs_t *)node->device;

    ext2_inodetable_t *inode = read_inode(this, node->inode);

    uint8_t *block = malloc(this->block_size);
    ext2_dir_t *direntry = NULL;
    uint8_t block_nr = 0;

    inode_read_block(this, inode, block_nr, block);

    uint32_t dir_offset = 0;
    uint32_t total_offset = 0;

    while (total_offset < inode->size)
    {
        if (dir_offset >= this->block_size)
        {
            block_nr++;
            dir_offset -= this->block_size;
            inode_read_block(this, inode, block_nr, block);
        }

        ext2_dir_t *d_ent = (ext2_dir_t *)((uintptr_t)block + dir_offset);

        if (d_ent->inode == 0 || strlen(name) != d_ent->name_len)
        {
            dir_offset += d_ent->rec_len;
            total_offset += d_ent->rec_len;

            continue;
        }

        char *dname = malloc(sizeof(char) * (d_ent->name_len + 1));
        memcpy(dname, &(d_ent->name), d_ent->name_len);
        dname[d_ent->name_len] = '\0';

        //printf("[EXT2] finddir_ext2: Comparing [%s], [%s]\n", dname, name);

        if (!strcmp(dname, name))
        {
            //printf("[EXT2] Found!\n");
            free(dname);
            direntry = malloc(d_ent->rec_len);
            memcpy(direntry, d_ent, d_ent->rec_len);
            break;
        }

        free(dname);

        dir_offset += d_ent->rec_len;
        total_offset += d_ent->rec_len;
    }

    free(inode);

    if (!direntry)
    {
        free(block);
        return NULL;
    }

    fs_node_t *outnode = malloc(sizeof(fs_node_t));

    memset(outnode, 0, sizeof(fs_node_t));

    inode = read_inode(this, direntry->inode);

    if (!node_from_file((fs_node_t *)this, inode, direntry, outnode))
    {
        printf("[EXT2] Could not get node from file\n");

        return 0;
    }

    free(direntry);
    free(inode);
    free(block);

    return outnode;
}

static int unlink_ext2(fs_node_t *node, char *name)
{

    ext2_fs_t *this = (ext2_fs_t *)node->device;

    ext2_inodetable_t *inode = read_inode(this, node->inode);

    uint8_t *block = malloc(this->block_size);
    ext2_dir_t *direntry = NULL;
    uint8_t block_nr = 0;

    inode_read_block(this, inode, block_nr, block);

    uint32_t dir_offset = 0;
    uint32_t total_offset = 0;

    while (total_offset < inode->size)
    {
        if (dir_offset >= this->block_size)
        {
            block_nr++;
            dir_offset -= this->block_size;
            inode_read_block(this, inode, block_nr, block);
        }

        ext2_dir_t *d_ent = (ext2_dir_t *)((uintptr_t)block + dir_offset);

        if (d_ent->inode == 0 || strlen(name) != d_ent->name_len)
        {
            dir_offset += d_ent->rec_len;
            total_offset += d_ent->rec_len;

            continue;
        }

        char *dname = malloc(sizeof(char) * (d_ent->name_len + 1));
        memcpy(dname, &(d_ent->name), d_ent->name_len);

        dname[d_ent->name_len] = '\0';

        if (!strcmp(dname, name))
        {
            free(dname);
            direntry = d_ent;
            break;
        }

        free(dname);

        dir_offset += d_ent->rec_len;
        total_offset += d_ent->rec_len;
    }

    free(inode);

    if (!direntry)
    {
        free(block);
        return -1;
    }

    direntry->inode = 0;

    inode_write_block(this,
                      inode,
                      node->inode,
                      block_nr,
                      block);

    free(block);

    ext2_sync(this);

    return 0;
}

static uint32_t read_ext2(fs_node_t *node, uint64_t offset, uint32_t size,
                          uint8_t *buffer)
{
    ext2_fs_t *this = (ext2_fs_t *)node->device;

    ext2_inodetable_t *inode = read_inode(this, node->inode);

    uint32_t end;

    if (inode->size == 0)
    {
        return 0;
    }

    if (offset + size > inode->size)
    {
        end = inode->size;
    }
    else
    {
        end = offset + size;
    }

    uint32_t start_block = offset / this->block_size;
    uint32_t end_block = end / this->block_size;
    uint32_t end_size = end - end_block * this->block_size;
    uint32_t size_to_read = end - offset;

    uint8_t *buf = malloc(this->block_size);

    if (start_block == end_block)
    {
        inode_read_block(this,
                         inode,
                         start_block,
                         buf);

        memcpy(buffer,
               (uint8_t *)(((uint64_t)buf) +
                           ((uintptr_t)offset % this->block_size)),
               size_to_read);
    }
    else
    {
        uint32_t block_offset;
        uint32_t blocks_read = 0;

        for (block_offset = start_block; block_offset < end_block;
             block_offset++, blocks_read++)
        {
            if (block_offset == start_block)
            {
                inode_read_block(this,
                                 inode,
                                 block_offset,
                                 buf);

                memcpy(buffer,
                       (uint8_t *)(((uint64_t)buf) +
                                   ((uintptr_t)offset % this->block_size)),
                       this->block_size - (offset % this->block_size));
            }
            else
            {
                inode_read_block(this,
                                 inode,
                                 block_offset,
                                 buf);

                memcpy(buffer + this->block_size * blocks_read -
                           (offset % this->block_size),
                       buf,
                       this->block_size);
            }
        }
        if (end_size)
        {
            inode_read_block(this,
                             inode,
                             end_block,
                             buf);

            memcpy(buffer + this->block_size * blocks_read -
                       (offset % this->block_size),
                   buf,
                   end_size);
        }
    }

    free(inode);
    free(buf);
    return size_to_read;
}

static uint32_t write_ext2(fs_node_t *node, uint64_t offset, uint32_t size,
                           uint8_t *buffer)
{
    ext2_fs_t *this = (ext2_fs_t *)node->device;

    ext2_inodetable_t *inode = read_inode(this, node->inode);

    uint32_t rv = write_inode_buffer(this,
                                     inode,
                                     node->inode,
                                     offset,
                                     size,
                                     buffer);

    free(inode);

    return rv;
}

static int truncate_ext2(fs_node_t *node)
{
    ext2_fs_t *this = (ext2_fs_t *)node->device;

    ext2_inodetable_t *inode = read_inode(this, node->inode);

    inode->size = 0;

    write_inode(this, inode, node->inode);

    return 0;
}

static void open_ext2(fs_node_t *node, uint32_t flags)
{
    (void)node;
    (void)flags;
}

static void close_ext2(fs_node_t *node)
{
    (void)node;
}

static struct dirent *readdir_ext2(fs_node_t *node, uint32_t index)
{
    ext2_fs_t *this = (ext2_fs_t *)node->device;

    ext2_inodetable_t *inode = read_inode(this, node->inode);

    ext2_dir_t *direntry = direntry_ext2(this, inode, node->inode, index);

    if (!direntry)
    {
        free(inode);
        return NULL;
    }

    struct dirent *dirent = malloc(sizeof(struct dirent));

    memcpy(&dirent->name, &direntry->name, direntry->name_len);

    dirent->name[direntry->name_len] = '\0';
    dirent->ino = direntry->inode;

    free(direntry);
    free(inode);

    return dirent;
}

static int symlink_ext2(fs_node_t *parent, char *target, char *name)
{
    if (!name)
    {
        return -1;
    }

    ext2_fs_t *this = parent->device;

    /* first off, check if it exists */
    fs_node_t *check = finddir_ext2(parent, name);

    if (check)
    {

        free(check);
        return -1;
    }

    unsigned int inode_no = allocate_inode(this);
    ext2_inodetable_t *inode = read_inode(this, inode_no);

    ktime_t ktime;

    RTC_get_time(&ktime);

    uint32_t current_time = RTC_time_to_int(&ktime);

    inode->atime = current_time;
    inode->ctime = current_time;
    inode->mtime = current_time;
    inode->dtime = 0;

    memset(inode->block, 0x00, sizeof(inode->block));
    inode->blocks = 0;
    inode->size = 0;

    // TODO: UID
    inode->uid = 0;
    inode->gid = 0;

    inode->faddr = 0;
    inode->links_count = 1;
    inode->flags = 0;
    inode->osd1 = 0;
    inode->generation = 0;
    inode->file_acl = 0;
    inode->dir_acl = 0;

    inode->mode = EXT2_S_IFLNK;

    inode->mode |= 0777;

    memset(inode->osd2, 0x00, sizeof(inode->osd2));

    size_t target_len = strlen(target);

    int embedded = target_len <= 60;

    if (embedded)
    {
        memcpy(_symlink(inode), target, target_len);
        inode->size = target_len;
    }

    write_inode(this, inode, inode_no);

    create_entry(parent, name, inode_no);

    if (!embedded)
    {
        write_inode_buffer(parent->device, inode, inode_no, 0, target_len, (uint8_t *)target);
    }

    free(inode);

    ext2_sync(this);

    return 0;
}

static int readlink_ext2(fs_node_t *node, char *buffer, size_t size)
{
    ext2_fs_t *this = (ext2_fs_t *)node->device;

    ext2_inodetable_t *inode = read_inode(this, node->inode);

    size_t read_size = inode->size < size ? inode->size : size;

    if (inode->size > 60)
    {
        read_ext2(node, 0, read_size, (uint8_t *)buffer);
    }
    else
    {
        memcpy(buffer, _symlink(inode), read_size);
    }

    if (read_size < size)
    {
        buffer[read_size] = '\0';
    }

    free(inode);

    return read_size;
}

//=============================================================================
// Mount filesystem
//=============================================================================

static uint32_t ext2_root(ext2_fs_t *this, ext2_inodetable_t *inode,
                          fs_node_t *fnode)
{
    //printf("[EXT2] ext2_root\n");

    if (!fnode)
    {
        printf("[EXT2] ext2_root: Invalid fnode\n");
        return -1;
    }

    fnode->device = (void *)this;
    fnode->inode = 2;
    fnode->name[0] = '/';
    fnode->name[1] = '\0';

    fnode->uid = inode->uid;
    fnode->gid = inode->gid;
    fnode->length = inode->size;
    fnode->permissions = inode->mode & 0xFFF;
    fnode->nlink = inode->links_count;

    fnode->flags = 0;

    if ((inode->mode & EXT2_S_IFREG) == EXT2_S_IFREG)
    {
        printf("[EXT2] ext2_root: Root is regular fule\n");
        return -1;
    }

    if (!((inode->mode & EXT2_S_IFDIR) == EXT2_S_IFDIR))
    {
        printf("[EXT2] ext2_root: Root is not directory\n");
        return -1;
    }

    if ((inode->mode & EXT2_S_IFBLK) == EXT2_S_IFBLK)
    {
        fnode->flags |= FS_BLOCKDEVICE;
    }

    if ((inode->mode & EXT2_S_IFCHR) == EXT2_S_IFCHR)
    {
        fnode->flags |= FS_CHARDEVICE;
    }

    if ((inode->mode & EXT2_S_IFIFO) == EXT2_S_IFIFO)
    {
        fnode->flags |= FS_PIPE;
    }

    if ((inode->mode & EXT2_S_IFLNK) == EXT2_S_IFLNK)
    {
        fnode->flags |= FS_SYMLINK;
    }

    fnode->atime = inode->atime;
    fnode->mtime = inode->mtime;
    fnode->ctime = inode->ctime;

    fnode->flags |= FS_DIRECTORY;
    fnode->read = NULL;
    fnode->write = NULL;
    fnode->chmod = chmod_ext2;
    fnode->open = open_ext2;
    fnode->close = close_ext2;
    fnode->readdir = readdir_ext2;
    fnode->finddir = finddir_ext2;
    fnode->ioctl = NULL;
    fnode->create = create_ext2;
    fnode->mkdir = mkdir_ext2;
    fnode->unlink = unlink_ext2;

    return 0;
}

static fs_node_t *mount_ext2(fs_node_t *block_device, int flags)
{
    //printf("[EXT2] mount_ext2\n");

    ext2_fs_t *this = malloc(sizeof(ext2_fs_t));

    memset(this, 0x00, sizeof(ext2_fs_t));

    this->flags = flags;

    this->block_device = block_device;
    this->block_size = 1024;

    vfs_lock(this->block_device);

    SB = malloc(this->block_size);

    read_block(this, 1, (uint8_t *)SB);

    if (SB->magic != EXT2_SUPER_MAGIC)
    {
        return NULL;
    }

    this->inode_size = SB->inode_size;

    if (SB->inode_size == 0)
    {
        this->inode_size = 128;
    }

    this->block_size = 1024 << SB->log_block_size;

    this->cache_entries = 1024;

    if (this->block_size > 2048)
    {
        this->cache_entries /= 4;
    }

    this->pointers_per_block = this->block_size / 4;

    BGDS = SB->blocks_count / SB->blocks_per_group;

    if (SB->blocks_per_group * BGDS < SB->blocks_count)
    {
        BGDS += 1;
    }

    this->inodes_per_group = SB->inodes_count / BGDS;

    //printf("Block size: %i\n", this->block_size);
    //printf("Inode size: %i\n", this->inode_size);
    //printf("Cache entries: %i\n", this->cache_entries);

    if (!(this->flags & EXT2_FLAG_NOCACHE))
    {
        printf("[EXT2] Allocating cache!\n");
        DC = malloc(sizeof(ext2_disk_cache_entry_t) * this->cache_entries);

        if (!DC)
        {
            printf("[EXT2] Could not allocate cache\n");

            for (;;)
                ;
        }

        //printf("[EXT2] DC: %#016x\n", DC);

        this->cache_data = malloc(this->block_size * this->cache_entries);

        if (!this->cache_data)
        {
            printf("[EXT2] Could not allocate cache data\n");

            for (;;)
                ;
        }

        //printf("[EXT2] cache_data: %#016x\n", this->cache_data);

        //printf("[EXT2] Cache size: %i\n", this->block_size * this->cache_entries);

        memset(this->cache_data, 0, this->block_size * this->cache_entries);

        //printf("[EXT2] Memory cleared!\n");

        for (uint32_t i = 0; i < this->cache_entries; ++i)
        {
            DC[i].block_no = 0;
            DC[i].dirty = 0;
            DC[i].last_use = 0;
            DC[i].block = this->cache_data + i * this->block_size;
        }
    }
    else
    {
        DC = NULL;
    }

    this->bgd_block_span = sizeof(ext2_bgdescriptor_t) * BGDS / this->block_size + 1;

    //printf("BGD block span: %i\n", this->bgd_block_span);

    BGD = malloc(this->block_size * this->bgd_block_span);

    this->bgd_offset = 2;

    if (this->block_size > 1024)
    {
        this->bgd_offset = 1;
    }

    //printf("[BGD] bgd: %#016x\n", BGD);

    for (int i = 0; i < this->bgd_block_span; ++i)
    {
        read_block(this, this->bgd_offset + i, (uint8_t *)((uint64_t)BGD + this->block_size * i));
    }

#if 0

    char *bg_buffer = malloc(sizeof(this->block_size));

    for (uint32_t i = 0; i < BGDS; ++i)
    {
        printf("[BGD] %i at %i\n", i, this->bgd_offset + i * SB->blocks_per_group);

        printf("[BGD] Block bitmap at %i\n", BGD[i].block_bitmap);
        printf("[BGD] Inode bitmap at %i\n", BGD[i].inode_bitmap);
        printf("[BGD] Inode table: %i\n", BGD[i].inode_table);
        printf("[BGD] Free blocks count: %i\n", BGD[i].free_blocks_count);
        printf("[BGD] Free inodes count: %i\n", BGD[i].free_inodes_count);
    }

#endif

    ext2_inodetable_t *root_inode = read_inode(this, 2);

    RN = (fs_node_t *)malloc(sizeof(fs_node_t));

    if (ext2_root(this, root_inode, RN))
    {
        printf("[EXT2] mount_ext2: ext2_root returned non-zero\n");
        return NULL;
    }

    return RN;
}

//=============================================================================
// Interface functions
//=============================================================================

int ext2_initialize()
{

    fs_node_t *dev = kopen("/dev/hda", 0);

    if (!dev)
    {
        printf("[EXT2] Could not open device\n");

        for (;;)
            ;

        return -1;
    }

    int flags = EXT2_FLAG_NOCACHE;

    fs_node_t *fs = mount_ext2(dev, flags);

    if (!fs)
    {
        for (;;)
            ;
    }

    vfs_mount("/", fs);

    return 0;
}

int ext2_finalize()
{
    return 0;
}

//=============================================================================
// End of file
//=============================================================================