#include <vfs/vfs.h>
#include <util/list.h>

#include <string.h>
#include <stdlib.h>

fs_node_t *fs_root = 0;

uint32_t read_fs(fs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buffer)
{
    if (node && node->read)
    {
        uint32_t ret = node->read(node, offset, size, buffer);
        return ret;
    }

    return 0;
}

uint32_t write_fs(fs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buffer)
{
    if (node && node->write)
    {
        uint32_t ret = node->write(node, offset, size, buffer);
        return ret;
    }

    return 0;
}

void open_fs(fs_node_t *node, uint8_t read, uint8_t write)
{
    if (node && node->open)
    {
        node->open(node, read, write);
    }
}

void close_fs(fs_node_t *node)
{
    if (node && node != fs_root && node->close)
    {
        node->close(node);
    }
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
    if (node && (node->flags & FS_DIRECTORY) && node->finddir)
    {
        fs_node_t *ret = node->finddir(node, name);

        return ret;
    }

    return (fs_node_t *)NULL;
}

int mkdir_fs(char *name, uint16_t permission)
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

        return -1;
    }

    for (i = strlen(dir_name) - 1; i >= 0; --i)
    {
        if (dir_name[i] == '/')
        {
            dir_name[i] = '\0';
            break;
        }
    }

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
        return 1;
    }

    ++i;

    if ((node->flags & FS_DIRECTORY) && node->mkdir)
    {
        node->mkdir(node, dir_name + i, permission);
    }

    free(node);
    free(dir_name);

    return 0;
}

int create_file_fs(char *name, uint16_t permission)
{
    int32_t i = strlen(name);
    char *dir_name = malloc(i + 1);
    memcpy(dir_name, name, i);
    dir_name[i] = '\0';
    if (dir_name[i - 1] == '/')
        dir_name[i - 1] = '\0';
    if (strlen(dir_name) == 0)
    {
        free(dir_name);
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
        /* XXX This is wrong */
        node = kopen(".", 0);
    }

    if (node == NULL)
    {
        free(dir_name);
        return 2;
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

    fs_node_t *n = malloc(sizeof(fs_node_t));
    memcpy(n, source, sizeof(fs_node_t));

    return n;
}

char *canonicalize_path(char *cwd, char *input)
{
    list_t *out = list_create();

    if (strlen(input) && input[0] != PATH_SEPARATOR)
    {
        char *path = malloc((strlen(cwd) + 1) * sizeof(char));
        memcpy(path, cwd, strlen(cwd + 1));

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
    char *save;
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
            memcpy(s, pch, strlen(pch + 1));
            list_insert(out, s);
        }
        pch = strtok_r(NULL, PATH_SEPARATOR_STRING, &save);
    }

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
        output = malloc(sizeof(char) * (size - 1));
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

fs_node_t *get_mount_point(char *path, size_t path_depth)
{
    (void)path;
    (void)path_depth;

    return fs_root;
}

fs_node_t *kopen(char *filename, uint32_t flags)
{
    (void)flags;

    if (!fs_root || !filename)
    {
        return NULL;
    }

    // TODO: This should be read from the current process
    char *cwd = "/";

    char *path = canonicalize_path(cwd, filename);

    size_t path_len = strlen(path);

    if (path_len == 1)
    {
        fs_node_t *root_clone = malloc(sizeof(fs_node_t));
        memcpy(root_clone, fs_root, sizeof(fs_node_t));

        free(path);

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

    uint32_t depth;
    fs_node_t *node_ptr = malloc(sizeof(fs_node_t));
    fs_node_t *mount_point = get_mount_point(path, path_depth);
    memcpy(node_ptr, mount_point, sizeof(fs_node_t));
    fs_node_t *node_next = NULL;

    for (depth = 0; depth < path_depth; ++depth)
    {
        node_next = finddir_fs(node_ptr, path_offset);
        free(node_ptr);
        node_ptr = node_next;
        if (!node_ptr)
        {
            free((void *)path);
            return NULL;
        }
        else if (depth == path_depth - 1)
        {
            open_fs(node_ptr, 1, 0);
            free((void *)path);

            return node_ptr;
        }

        path_offset += strlen(path_offset) + 1;
    }

    free((void *)path);

    return NULL;
}
