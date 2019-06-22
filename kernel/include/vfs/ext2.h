/**
 * @file ext2.h
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-06-22
 * 
 * @brief 
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

#ifndef _EXT2_H
#define _EXT2_H

#include <stdint.h>
#include <vfs/vfs.h>
#include <sync/spinlock.h>

#define EXT2_SUPER_MAGIC 0xEF53
#define EXT2_DIRECT_BLOCKS 12

typedef struct
{
    uint32_t inodes_count;
    uint32_t blocks_count;
    uint32_t r_blocks_count;
    uint32_t free_blocks_count;
    uint32_t free_inodes_count;
    uint32_t first_data_block;
    uint32_t log_block_size;
    uint32_t log_frag_size;
    uint32_t blocks_per_group;
    uint32_t frags_per_block;
    uint32_t inodes_per_group;
    uint32_t mtime;
    uint32_t wtime;

    uint16_t mnt_count;
    uint16_t max_mnt_count;
    uint16_t magic;
    uint16_t state;
    uint16_t errors;
    uint16_t minor_rev_level;

    uint32_t lastcheck;
    uint32_t checkinterval;
    uint32_t creator_os;
    uint32_t rev_level;

    uint16_t def_resuid;
    uint16_t def_resgid;

    uint32_t first_ino;
    uint16_t inode_size;
    uint16_t block_group_nr;
    uint32_t feature_compat;
    uint32_t feature_incompat;
    uint32_t feature_ro_compat;

    uint8_t uuid[16];
    uint8_t volume_name[16];

    uint8_t last_mounted[64];

    uint32_t algo_bitmap;

    uint8_t prealloc_block;
    uint8_t prealloc_dir_blocks;
    uint16_t _padding;

    uint8_t journal_uuid[16];
    uint32_t journal_inum;
    uint32_t journal_dev;
    uint32_t last_orphan;

    uint32_t hash_seed[4];
    uint8_t def_hash_version;
    uint16_t _padding_a;
    uint8_t _padding_b;

    uint32_t default_mount_options;
    uint32_t first_meta_bg;
    uint8_t _unused[760];

} __attribute__((packed)) ext2_superblock_t;

typedef struct
{
    uint32_t block_bitmap;
    uint32_t inode_bitmap;
    uint32_t inode_table;
    uint16_t free_blocks_count;
    uint16_t free_inodes_count;
    uint16_t used_dirs_count;
    uint16_t pad;
    uint8_t reserved[12];
} __attribute__((packed)) ext2_bgdescriptor_t;

/* File Types */
#define EXT2_S_IFSOCK 0xC000
#define EXT2_S_IFLNK 0xA000
#define EXT2_S_IFREG 0x8000
#define EXT2_S_IFBLK 0x6000
#define EXT2_S_IFDIR 0x4000
#define EXT2_S_IFCHR 0x2000
#define EXT2_S_IFIFO 0x1000

/* setuid, etc. */
#define EXT2_S_ISUID 0x0800
#define EXT2_S_ISGID 0x0400
#define EXT2_S_ISVTX 0x0200

/* rights */
#define EXT2_S_IRUSR 0x0100
#define EXT2_S_IWUSR 0x0080
#define EXT2_S_IXUSR 0x0040
#define EXT2_S_IRGRP 0x0020
#define EXT2_S_IWGRP 0x0010
#define EXT2_S_IXGRP 0x0008
#define EXT2_S_IROTH 0x0004
#define EXT2_S_IWOTH 0x0002
#define EXT2_S_IXOTH 0x0001

typedef struct
{
    uint16_t mode;
    uint16_t uid;
    uint32_t size;
    uint32_t atime;
    uint32_t ctime;
    uint32_t mtime;
    uint32_t dtime;
    uint16_t gid;
    uint16_t links_count;
    uint32_t blocks;
    uint32_t flags;
    uint32_t osd1;
    uint32_t block[15];
    uint32_t generation;
    uint32_t file_acl;
    uint32_t dir_acl;
    uint32_t faddr;
    uint8_t osd2[12];
} __attribute__((packed)) ext2_inodetable_t;

typedef struct
{
    uint32_t inode;
    uint16_t rec_len;
    uint8_t name_len;
    uint8_t file_type;
    char name[];
} __attribute__((packed)) ext2_dir_t;

typedef struct
{
    uint32_t block_no;
    uint32_t last_use;
    uint8_t dirty;
    uint8_t *block;
} ext2_disk_cache_entry_t;

typedef struct
{
    ext2_superblock_t *superblock;
    ext2_bgdescriptor_t *block_groups;
    fs_node_t *root_node;

    fs_node_t *block_device;

    uint32_t block_size;
    uint32_t pointers_per_block;
    uint32_t inodes_per_group;
    uint32_t block_group_count;

    ext2_disk_cache_entry_t *disk_cache;
    uint32_t cache_entries;
    uint32_t cache_time;

    spinlock_t lock;

    uint8_t bgd_block_span;
    uint8_t bgd_offset;
    uint32_t inode_size;

    uint8_t *cache_data;

    uint32_t flags;

} ext2_fs_t;

#define EXT2_FLAG_NOCACHE 0x0001

fs_node_t *ext2_fs_mount(char *device, char *mount_path);
int ext2_initialize();
int ext2_finalize();

#endif