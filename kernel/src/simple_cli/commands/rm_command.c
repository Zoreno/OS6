#include <simple_cli/commands.h>

#include <syscall/syscall.h>

#include <stdio.h>

static int rm_command_remove_item(int recursive, const char* path);
static int rm_command_remove_directory(int recursive, const char* dir);

static int rm_command_remove_item(int recursive, const char* path)
{
    struct stat statbuf;
    syscall_lstat(path, (uintptr_t)&statbuf);

    if (S_ISDIR(statbuf.st_mode))
    {
        if (!recursive)
        {
            printf("[RM]: %s is a directory\n", path);
            return 1;
        }
        return rm_command_remove_directory(recursive, path);
    }

    syscall_unlink(path);

    return 0;
}

static int rm_command_remove_directory(int recursive, const char* dir)
{
    int fd = syscall_open(dir, O_DIRECTORY, O_RDWR);

    if (fd < 0)
    {
        printf("Could not open %s\n", dir);
        return fd;
    }

    struct dirent dirent;
    int i = 0;

    while (syscall_readdir(fd, i, &dirent) == 1)
    {
        if (!strcmp(dirent.name, ".") || !strcmp(dirent.name, ".."))
        {
            ++i;
            continue;
        }

        char tmp[128] = {0};

        sprintf(tmp, "%s/%s", dir, dirent.name);

        int status = rm_command_remove_item(recursive, tmp);

        if (status)
        {
            return status;
        }

        ++i;
    }

    syscall_close(fd);

    syscall_unlink(dir);

    return 0;
}

int rm_command(int argc, const char** argv)
{
    int recursive = 0;

    if (argc <= 1)
    {
        printf("Argument expected\n");
    }

    if (strcmp(argv[1], "-r") == 0)
    {
        recursive = 1;
    }

    for (int i = recursive ? 2 : 1; i < argc; ++i)
    {
        rm_command_remove_item(recursive, argv[i]);
    }

    return 0;
}

//=============================================================================
// End of file
//=============================================================================