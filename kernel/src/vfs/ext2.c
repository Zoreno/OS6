#include <vfs/ext2.h>

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
static int set_block_number(ext2_fs_t *this, ext2_inodetable_t *inode, uint32_t inode_no, uint32_t iblock, uint32_t rblock);
static int get_block_number(ext2_fs_t *this, ext2_inodetable_t *inode, uint32_t iblock);
static int allocate_block(ext2_fs_t *this);

//=============================================================================
// Inode
//=============================================================================

static int write_inode(ext2_fs_t *this, ext2_inodetable_t *inode, uint32_t index);
static int allocate_inode_block(ext2_fs_t *this, ext2_inodetable_t *inode, uint32_t inode_no, uint32_t block);
static uint32_t inode_read_block(ext2_fs_t *this, ext2_inodetable_t *inode, uint32_t block, uint8_t *buffer);
static uint32_t inode_write_block(ext2_fs_t *this, ext2_inodetable_t *inode, uint32_t inode_no, uint32_t block, uint8_t *buffer);
static uint32_t allocate_inode(ext2_fs_t *this);
static void refresh_inode(ext2_fs_t *this, ext2_inodetable_t *inodet, uint32_t inode);
static ext2_inodetable_t *read_inode(ext2_fs_t *this, uint32_t inode);
static uint32_t write_inode_buffer(ext2_fs_t *this, ext2_inodetable_t *inode, uint32_t inode_number, uint64_t offset, uint32_t size, uint8_t *buffer);
static int create_entry(fs_node_t *parent, char *name, uint32_t inode);
static uint32_t node_from_file(fs_node_t *this, ext2_inodetable_t *inode, ext2_dir_t *direntry, fs_node_t *fnode);

//=============================================================================
// VFS Operations
//=============================================================================

static int mkdir_ext2(fs_node_t *parent, char *name, uint16_t permission);
static int create_ext2(fs_node_t *parent, char *name, uint16_t permission);
static int chmod_ext2(fs_node_t *parent, int mode);
static ext2_dir_t *direntry_ext2(ext2_fs_t *this, ext2_inodetable_t *inode, uint32_t no, uint32_t index);
static fs_node_t *finddir_ext2(fs_node_t *node, char *name);
static int unlink_ext2(fs_node_t *node, char *name);
static uint32_t read_ext2(fs_node_t *node, uint64_t offset, uint32_t size, uint8_t *buffer);
static uint32_t write_ext2(fs_node_t *node, uint64_t offset, uint32_t size, uint8_t *buffer);
static void truncate_ext2(fs_node_t *node);
static void open_ext2(fs_node_t *node);
static void close_ext2(fs_node_t *node);
static struct dirent *readdir_ext2(fs_node_t *node, uint32_t index);
static int symlink_ext2(fs_node_t *parent, char *target, char *name);
static int readlink_ext2(fs_node_t *node, char *buffer, size_t size);

//=============================================================================
// Mount filesystem
//=============================================================================

static uint32_t ext2_root(ext2_fs_t *this, ext2_inodetable_t *inode, fs_node_t *fnode);
static fs_node_t *mount_ext2(fs_node_t *block_device, int flags);