#include <simple_cli/simple_cli.h>

#include <syscall/syscall.h>

#include <stdio.h>

int mkdir_command(int argc, const char** argv)
{
    if (argc <= 1)
    {
        printf("Argument expected\n");
    }

    for (int i = 1; i < argc; ++i)
    {
        int ret = syscall_mkdir((char*)argv[i], 0777);

        if (ret < 0)
        {
            return ret;
        }
    }

    return 0;
}

//=============================================================================
// End of file
//=============================================================================