#include <process/launch_program.h>
#include <process/process.h>
#include <syscall/syscall.h>
#include <exec/elf64.h>

#include <stdio.h>

int launch_program(char *path)
{
    pid_t pid;
    int status;
    int retval;

    // TODO: This needs to be fixed
    static char *programPath;
    programPath = path;

    pid = process_fork();

    if (pid < 0) // Error
    {
        printf("Error: Could not fork\n");
        return -1; // TODO: This should probably be something else
    } 
    else if (pid == 0) // Child
    {
        exec_elf(programPath, 0, NULL, NULL, 0);
        printf("[ERROR] Exec elf returned\n");

        return -1;
    }
    else
    {
        int status;

        retval = waitpid(pid, &status, 0);
    }

    return retval;
}