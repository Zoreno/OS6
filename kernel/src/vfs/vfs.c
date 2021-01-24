/**
 * @file vfs.c
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-04-07
 *
 * @brief Virtual file system implementation
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

#include <vfs/vfs.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <arch/arch.h>
#include <debug/backtrace.h>
#include <logging/logging.h>
#include <process/process.h>
#include <sync/spinlock.h>
#include <util/list.h>
#include <util/tree.h>
#include <vfs/nulldev.h>
#include <vfs/zerodev.h>

fs_node_t *fs_root = 0;
tree_t *fs_tree = 0;

static spinlock_t tmp_lock = {0, 0};

int has_permissions(fs_node_t *node, int permission_bit)
{
    (void)node;
    (void)permission_bit;

    // TODO: Implement
    return 1;
}

uint32_t read_fs(fs_node_t *node,
                 uint64_t offset,
                 uint32_t size,
                 uint8_t *buffer)
{
    if (node && node->read)
    {
        uint32_t ret = node->read(node, offset, size, buffer);

        return ret;
    }

    return 0;
}

uint32_t write_fs(fs_node_t *node,
                  uint64_t offset,
                  uint32_t size,
                  uint8_t *buffer)
{
    if (node && node->write)
    {
        uint32_t ret = node->write(node, offset, size, buffer);
        return ret;
    }

    return 0;
}

void open_fs(fs_node_t *node, uint32_t flags)
{
    if (!node)
    {
        return;
    }

    if (node->refcount >= 0)
    {
        spinlock_lock(&tmp_lock);
        node->refcount++;
        spinlock_unlock(&tmp_lock);
    }

    if (node->open)
    {
        node->open(node, flags);

        if (flags & O_TRUNC)
        {
            truncate_fs(node);
        }
    }
}

void close_fs(fs_node_t *node)
{
    if (node == fs_root)
    {
        return;
    }

    if (!node)
    {
        return;
    }

    spinlock_lock(&tmp_lock);

    node->refcount--;

    if (node->refcount == 0)
    {
        if (node->close)
        {
            node->close(node);
        }
    }

    spinlock_unlock(&tmp_lock);
}

struct dirent *readdir_fs(fs_node_t *node, uint32_t index)
{
    if (node && (node->flags & FS_DIRECTORY) && node->readdir)
    {
        struct dirent *ret = node->readdir(node, index);
        return ret;
    }

    return (struct dirent *)NULL;
}

fs_node_t *finddir_fs(fs_node_t *node, char *name)
{
    if (!node)
    {
        return NULL;
    }

    if ((node->flags & FS_DIRECTORY) && node->finddir)
    {
        fs_node_t *ret = node->finddir(node, name);

        return ret;
    }

    return (fs_node_t *)NULL;
}

int mkdir_fs(char *name, uint16_t permission)
{
    fs_node_t *parent;

    char *cwd = process_get_current()->wd_path;

    char *path = canonicalize_path(cwd, name);

    if (!name || !strlen(name))
    {
        return -1;
    }

    char *parent_path = malloc(strlen(path) + 4);
    sprintf(parent_path, "%s/..", path);

    char *f_path = path + strlen(path) - 1;
    while (f_path > path)
    {
        if (*f_path == PATH_SEPARATOR)
        {
            f_path += 1;
            break;
        }
        f_path--;
    }

    while (*f_path == PATH_SEPARATOR)
    {
        f_path++;
    }

    parent = kopen(parent_path, 0);
    free(parent_path);

    if (!parent)
    {
        free(path);
        return -1;
    }

    if (!f_path || !strlen(f_path))
    {
        return -1;
    }

    fs_node_t *this = kopen(path, 0);

    int exists = 0;

    if (this)
    {
        close_fs(this);
        exists = 1;
    }

    if (!has_permissions(parent, 02))
    {
        free(path);
        close_fs(parent);

        return -1;
    }

    int ret;

    if (parent->mkdir)
    {
        ret = parent->mkdir(parent, f_path, permission);
    }
    else
    {
        ret = -1;
    }

    free(path);
    close_fs(parent);

    return ret;
}

int create_file_fs(char *name, uint16_t permission)
{
    int32_t i = strlen(name);

    char *dir_name = malloc(i + 1);

    memcpy(dir_name, name, i);

    dir_name[i] = '\0';

    if (dir_name[i - 1] == '/')
    {
        dir_name[i - 1] = '\0';
    }

    if (strlen(dir_name) == 0)
    {
        free(dir_name);
        log_error("[VFS] Error: Invalid file name");
        backtrace();
        return 1;
    }

    for (i = strlen(dir_name) - 1; i >= 0; i--)
    {
        if (dir_name[i] == '/')
        {
            dir_name[i] = '\0';
            break;
        }
    }

    // get the parent dir node.
    fs_node_t *node;
    if (i >= 0)
    {
        node = kopen(dir_name, 0);
    }
    else
    {
        node = kopen(".", 0);
    }

    if (node == NULL)
    {
        free(dir_name);
        log_error("[VFS] Error: Could not find parent directory");
        backtrace();
        return 2;
    }

    if (!has_permissions(node, 02))
    {
        log_error("[VFS] Error: No permission to create file");
        backtrace();
        return 3;
    }

    i++;
    if ((node->flags & FS_DIRECTORY) && node->mkdir)
    {
        node->create(node, dir_name + i, permission);
    }

    free(node);
    free(dir_name);
    return 0;
}

fs_node_t *clone_fs(fs_node_t *source)
{
    if (!source)
    {
        return NULL;
    }

    if (source->refcount >= 0)
    {
        spinlock_lock(&tmp_lock);
        source->refcount++;
        spinlock_unlock(&tmp_lock);
    }

    return source;
}

char *canonicalize_path(char *cwd, char *input)
{
    list_t *out = list_create();

    if (strlen(input) && input[0] != PATH_SEPARATOR)
    {
        char *path = malloc((strlen(cwd) + 1) * sizeof(char));
        memcpy(path, cwd, strlen(cwd) + 1);

        char *pch;
        char *save;
        pch = strtok_r(path, PATH_SEPARATOR_STRING, &save);

        while (pch != NULL)
        {
            char *s = malloc(sizeof(char) * (strlen(pch) + 1));
            memcpy(s, pch, strlen(pch) + 1);
            list_insert(out, s);

            pch = strtok_r(NULL, PATH_SEPARATOR_STRING, &save);
        }
        free(path);
    }

    char *path = malloc((strlen(input) + 1) * sizeof(char));
    memcpy(path, input, strlen(input) + 1);

    char *pch;
    char *save = malloc((strlen(input) + 1) * sizeof(char));
    pch = strtok_r(path, PATH_SEPARATOR_STRING, &save);

    while (pch != NULL)
    {
        // The case where we have "folder/../file"
        // Remove the .. and the folder before
        if (!strcmp(pch, PATH_UP))
        {
            list_node_t *n = list_pop(out);

            if (n)
            {
                free(n->payload);
                free(n);
            }
        }
        else if (!strcmp(pch, PATH_DOT))
        {
        }
        else
        {
            char *s = malloc(sizeof(char) * (strlen(pch) + 1));
            memcpy(s, pch, strlen(pch) + 1);
            list_insert(out, s);
        }
        pch = strtok_r(NULL, PATH_SEPARATOR_STRING, &save);
    }

    free(save);
    free(path);

    size_t size = 0;

    for (list_node_t *lnode = out->head; lnode != NULL; lnode = lnode->next)
    {
        size += strlen(lnode->payload) + 1;
    }

    char *output;

    if (size == 0)
    {
        output = malloc(2 * sizeof(char));
        output[0] = PATH_SEPARATOR;
        output[1] = '\0';
    }
    else
    {
        output = malloc(sizeof(char) * (size + 1));
        char *output_offset = output;

        for (list_node_t *lnode = out->head; lnode != NULL; lnode = lnode->next)
        {
            output_offset[0] = PATH_SEPARATOR;
            output_offset++;
            memcpy(output_offset, lnode->payload, strlen(lnode->payload) + 1);
            output_offset += strlen(lnode->payload);
        }
    }

    list_destroy(out);
    list_free(out);

    free(out);

    return output;
}

fs_node_t *get_mount_point(char *path,
                           size_t path_depth,
                           char **outpath,
                           uint32_t *outdepth)
{
    size_t depth;

    for (depth = 0; depth <= path_depth; ++depth)
    {
        path += strlen(path) + 1;
    }

    /* Last available node */
    fs_node_t *last = fs_root;
    tree_node_t *node = fs_tree->root;

    char *at = *outpath;
    int _depth = 1;
    int _tree_depth = 0;

    while (1)
    {
        if (at >= path)
        {
            break;
        }

        int found = 0;

        for (list_node_t *child = node->children->head; child != NULL;
             child = child->next)
        {
            tree_node_t *tchild = (tree_node_t *)child->payload;

            vfs_entry_t *ent = (vfs_entry_t *)tchild->value;

            if (!strcmp(ent->name, at))
            {
                found = 1;
                node = tchild;
                at = at + strlen(at) + 1;

                if (ent->file)
                {
                    _tree_depth = _depth;
                    last = ent->file;
                    *outpath = at;
                }

                break;
            }
        }

        if (!found)
        {
            break;
        }

        _depth++;
    }

    *outdepth = _tree_depth;

    if (last)
    {
        fs_node_t *last_clone = malloc(sizeof(fs_node_t));
        memcpy(last_clone, last, sizeof(fs_node_t));
        return last_clone;
    }

    return last;
}

int ioctl_fs(fs_node_t *node, int request, void *argp)
{
    if (!node)
    {
        return -1;
    }

    if (node->ioctl)
    {
        return node->ioctl(node, request, argp);
    }

    return -1;
}

int chmod_fs(fs_node_t *node, int mode)
{
    if (!node)
    {
        return 0;
    }

    if (node->chmod)
    {
        return node->chmod(node, mode);
    }

    return 0;
}

int chown_fs(fs_node_t *node, int uid, int gid)
{
    if (!node)
    {
        return 0;
    }

    if (node->chown)
    {
        return node->chown(node, uid, gid);
    }

    return 0;
}

int unlink_fs(char *name)
{
    fs_node_t *parent;

    char *cwd = process_get_current()->wd_path;
    char *path = canonicalize_path(cwd, name);

    char *parent_path = malloc(strlen(path) + 4);
    sprintf(parent_path, "%s/..", path);

    char *f_path = path + strlen(path) - 1;

    while (f_path > path)
    {
        if (*f_path == PATH_SEPARATOR)
        {
            f_path += 1;
            break;
        }
        f_path--;
    }

    while (*f_path == PATH_SEPARATOR)
    {
        f_path++;
    }

    parent = kopen(parent_path, 0);
    free(parent_path);

    if (!parent)
    {
        free(path);
        return -1;
    }

    if (!has_permissions(parent, 02))
    {
        free(path);
        close_fs(parent);
        return -1;
    }

    int ret = 0;

    if (parent->unlink)
    {
        ret = parent->unlink(parent, f_path);
    }
    else
    {
        ret = -1;
    }

    free(path);
    close_fs(parent);

    return ret;
}

int symlink_fs(char *target, char *name)
{
    fs_node_t *parent;

    char *cwd = process_get_current()->wd_path;
    char *path = canonicalize_path(cwd, name);

    char *parent_path = malloc(strlen(path) + 4);
    sprintf(parent_path, "%s/..", path);

    char *f_path = path + strlen(path) - 1;

    while (f_path > path)
    {
        if (*f_path == PATH_SEPARATOR)
        {
            f_path += 1;
            break;
        }
        f_path--;
    }

    while (*f_path == PATH_SEPARATOR)
    {
        f_path++;
    }

    parent = kopen(parent_path, 0);
    free(parent_path);

    if (!parent)
    {
        free(path);
        return -1;
    }

    int ret = 0;

    if (parent->symlink)
    {
        ret = parent->symlink(parent, target, f_path);
    }
    else
    {
        ret = -1;
    }

    free(path);
    close_fs(parent);

    return ret;
}

int readlink_fs(fs_node_t *node, char *buf, size_t size)
{
    if (!node)
    {
        return -1;
    }

    if (node->readlink)
    {
        return node->readlink(node, buf, size);
    }

    return -1;
}

int selectcheck_fs(fs_node_t *node)
{
    if (!node)
    {
        return -1;
    }

    if (node->selectcheck)
    {
        int ret = node->selectcheck(node);

        return ret;
    }

    return -1;
}

int selectwait_fs(fs_node_t *node, void *process)
{
    if (!node)
    {
        return -1;
    }

    if (node->selectwait)
    {
        int ret = node->selectwait(node, process);

        return ret;
    }

    return -1;
}

void truncate_fs(fs_node_t *node)
{
    if (!node)
    {
        return;
    }

    if (node->truncate)
    {
        node->truncate(node);
    }
}

void vfs_install(void)
{
    log_info("[VFS] Installing VFS...");
    cli();

    fs_tree = tree_create();

    vfs_entry_t *root = malloc(sizeof(vfs_entry_t));

    root->name = strdup("[root]");
    root->file = NULL;
    root->fs_type = NULL;
    root->device = NULL;

    tree_set_root(fs_tree, root);

    null_dev_init();
    zero_dev_init();

    sti();
    log_info("[VFS] Installed!");
}

static spinlock_t vfs_slock = {0, 0};

void *vfs_mount(char *path, fs_node_t *local_root)
{
    log_info("[VFS] Mounting %s to [%s]", local_root->name, path);

    if (!fs_tree)
    {
        return NULL;
    }

    if (!path || path[0] != PATH_SEPARATOR)
    {
        return NULL;
    }

    spinlock_lock(&vfs_slock);

    local_root->refcount = -1;

    tree_node_t *ret_val = NULL;

    char *p = strdup(path);
    char *i = p;

    int path_len = strlen(p);

    while (i < p + path_len)
    {
        if (*i == PATH_SEPARATOR)
        {
            *i = '\0';
        }

        ++i;
    }

    p[path_len] = '\0';
    i = p + 1;

    tree_node_t *root_node = fs_tree->root;

    if (*i == '\0')
    {
        vfs_entry_t *root = (vfs_entry_t *)root_node->value;

        if (root->file)
        {
            log_warn("[VFS] vfs_mount: Path is already mounted");
            return NULL;
        }

        root->file = local_root;
        fs_root = local_root;
        ret_val = root_node;
    }
    else
    {
        tree_node_t *node = root_node;
        char *at = i;

        while (1)
        {
            if (at >= p + path_len)
            {
                break;
            }

            int found = 0;

            for (list_node_t *child = node->children->head; child != NULL;
                 child = child->next)
            {
                tree_node_t *tchild = (tree_node_t *)child->payload;
                vfs_entry_t *ent = (vfs_entry_t *)tchild->value;

                if (!strcmp(ent->name, at))
                {
                    found = 1;
                    node = tchild;
                    ret_val = node;
                    break;
                }
            }

            if (!found)
            {
                vfs_entry_t *ent = malloc(sizeof(vfs_entry_t));

                ent->name = strdup(at);
                ent->file = NULL;
                ent->device = NULL;
                ent->fs_type = NULL;
                node = tree_node_insert_child(fs_tree, node, ent);
            }

            at = at + strlen(at) + 1;
        }

        vfs_entry_t *ent = (vfs_entry_t *)node->value;

        if (ent->file)
        {
            return NULL;
        }

        ent->file = local_root;
        ret_val = node;
    }

    free(p);
    spinlock_unlock(&vfs_slock);

    log_info("[VFS] Mounting Done!");

    return NULL;
}

void vfs_lock(fs_node_t *node)
{
    spinlock_lock(&tmp_lock);

    node->refcount = -1;

    spinlock_unlock(&tmp_lock);
}

static fs_node_t *kopen_recur(char *filename,
                              uint32_t flags,
                              uint32_t symlink_depth,
                              char *relative_to)
{
    if (!filename)
    {
        log_error("[VFS] kopen_recur: Invalid file name");
        return NULL;
    }

    char *path = canonicalize_path(relative_to, filename);

    size_t path_len = strlen(path);

    if (path_len == 1)
    {
        fs_node_t *root_clone = malloc(sizeof(fs_node_t));

        if (!root_clone)
        {
            free(path);
            log_error("[VFS] Could not allocate memory for root clone");
            return NULL;
        }

        memcpy(root_clone, fs_root, sizeof(fs_node_t));

        free(path);

        open_fs(root_clone, flags);

        return root_clone;
    }

    char *path_offset = path;

    uint32_t path_depth = 0;

    while (path_offset < path + path_len)
    {
        if (*path_offset == PATH_SEPARATOR)
        {
            *path_offset = '\0';
            path_depth++;
        }

        path_offset++;
    }

    path[path_len] = '\0';
    path_offset = path + 1;

    uint32_t depth = 0;

    fs_node_t *node_ptr =
        get_mount_point(path, path_depth, &path_offset, &depth);

    if (!node_ptr)
    {
        log_error("[VFS] kopen_recur: Could not get mount point");
        return NULL;
    }

    do
    {
        if ((node_ptr->flags & FS_SYMLINK) &&
            !((flags & O_NOFOLLOW) && (flags & O_PATH) && depth == path_depth))
        {
            log_debug("Resolving symlink at: [%s]", node_ptr->name);

            if ((flags & O_NOFOLLOW) && depth == path_depth - 1)
            {
                free((void *)path);
                free(node_ptr);

                log_warn("[VFS] kopen_recur: NOFOLLOW symlink set");
                return NULL;
            }

            if (symlink_depth >= MAX_SYMLINK_DEPTH)
            {
                free((void *)path);
                free(node_ptr);

                log_error("[VFS] kopen_recur: Symlink recursion too deep");
                return NULL;
            }

            char *symlink_buf = malloc(MAX_SYMLINK_SIZE);
            int len = readlink_fs(node_ptr, symlink_buf, MAX_SYMLINK_SIZE);

            if (len < 0)
            {
                free((void *)path);
                free(node_ptr);
                free(symlink_buf);

                log_error("[VFS] kopen_recur: Could not read symlink");

                return NULL;
            }

            if (symlink_buf[len] != '\0')
            {
                free((void *)path);
                free(node_ptr);
                free(symlink_buf);

                log_error("[VFS] kopen_recur: Symlink buffer empty");

                return NULL;
            }

            fs_node_t *old_node_ptr = node_ptr;

            char *relpath = malloc(path_len + 1);

            if (!relpath)
            {
                free((void *)path);
                free(node_ptr);
                free(symlink_buf);

                log_error("[VFS] Could not allocate memory for relpath");

                return NULL;
            }

            char *ptr = relpath;

            memcpy(relpath, path, path_len + 1);

            for (uint32_t i = 0; depth && i < depth - 1; ++i)
            {
                while (*ptr != '\0')
                {
                    ptr++;
                }

                *ptr = PATH_SEPARATOR;
            }

            node_ptr = kopen_recur(symlink_buf, 0, symlink_depth + 1, relpath);

            free(relpath);

            free(old_node_ptr);

            free(symlink_buf);

            if (!node_ptr)
            {
                free((void *)path);
            }
        }

        if (path_offset >= path + path_len)
        {
            free(path);
            open_fs(node_ptr, flags);

            return node_ptr;
        }

        if (depth == path_depth)
        {
            open_fs(node_ptr, flags);
            free((void *)path);

            return node_ptr;
        }

        fs_node_t *node_next = finddir_fs(node_ptr, path_offset);

        free(node_ptr);

        node_ptr = node_next;

        if (!node_ptr)
        {
            free((void *)path);

            return NULL;
        }

        path_offset += strlen(path_offset) + 1;
        ++depth;
    } while (depth < path_depth + 1);

    free((void *)path);

    return NULL;
}

fs_node_t *kopen(char *filename, uint32_t flags)
{
    process_t *current_process = process_get_current();

    return kopen_recur(filename, flags, 0, current_process->wd_path);
}

static void print_vfs_tree_node(tree_node_t *node, size_t height)
{
    if (!node)
    {
        return;
    }

    for (uint32_t i = 0; i < height; ++i)
    {
        printf("  ");
    }

    vfs_entry_t *fnode = (vfs_entry_t *)node->value;

    if (fnode->file)
    {
        printf("[%s], [%s]\n", fnode->name, fnode->file->name);
    }
    else
    {
        printf("[%s], (empty)\n", fnode->name);
    }

    for (list_node_t *child = node->children->head; child != NULL;
         child = child->next)
    {
        print_vfs_tree_node((tree_node_t *)child->payload, height + 1);
    }
}

void print_vfs_tree(void)
{
    printf("Tree entries: %i\n", fs_tree->nodes);
    print_vfs_tree_node(fs_tree->root, 0);
}

//=============================================================================
// End of file
//=============================================================================