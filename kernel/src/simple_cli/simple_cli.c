#include <simple_cli/simple_cli.h>
#include <simple_cli/commands.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <process/process.h>

const char *simple_cli_get_user_name()
{
    return "root";
}

const char *simple_cli_get_user_group()
{
    return "root";
}

const char *simple_cli_get_working_directory()
{
    return process_get_current()->wd_path;
}

char *simple_cli_read_line()
{
    char c;

    const int buffer_size = 1024;

    char *buffer = malloc(buffer_size);
    int buf_ptr = 0;

    if (!buffer)
    {
        printf("[ERROR] Allocating buffer\n");
        for (;;)
            ;
    }

    memset(buffer, 0, buffer_size);

    char line_start[256] = {0};
    sprintf(line_start, "%s@%s:%s$ ", simple_cli_get_user_name(),
            simple_cli_get_user_group(),
            simple_cli_get_working_directory());

    printf("%s", line_start);

    while (1)
    {
        c = getc();

        if (c == 13)
        {
            putchar('\n');
            buffer[buf_ptr] = 0;
            return buffer;
        }
        else if (c == 127)
        {
            if (buf_ptr > 0)
            {
                buf_ptr--;
                putchar(13);
                printf("%s", line_start);
                buffer[buf_ptr] = 0;
                printf("%s ", buffer);
                putchar(13);
                printf("%s", line_start);
                printf("%s", buffer);
            }
        }
        else
        {
            putchar(c);
            buffer[buf_ptr++] = c;
        }
    }
}

const char **simple_cli_split_line(char *line)
{
    const char *strtok_delim = " \t\r\n\a";
    const int max_tokens_increment = 64;
    int max_tokens = max_tokens_increment;
    int position = 0;

    char **tokens = malloc(max_tokens * sizeof(char *));
    char *token;

    if (!tokens)
    {
        printf("[ERROR] Allocating buffer\n");
        for (;;)
            ;
    }

    token = strtok(line, strtok_delim);

    while (token != NULL)
    {
        tokens[position++] = token;

        if (position >= max_tokens)
        {
            max_tokens += max_tokens_increment;
            tokens = realloc(tokens, max_tokens * sizeof(char *));
            if (!tokens)
            {
                printf("[ERROR] Allocating buffer\n");
                for (;;)
                    ;
            }
        }

        token = strtok(NULL, strtok_delim);
    }

    tokens[position] = NULL;
    return (const char **)tokens;
}

int simple_cli_get_token_count(const char **tokens)
{
    int i = 0;
    while (tokens[i] != NULL)
    {
        ++i;
    }

    return i;
}

simple_cli_command_t _commands[] = {{.name = "test", .command = test_command},
                                    {.name = "cd", .command = cd_command},
                                    {.name = "ls", .command = ls_command},
                                    {.name = "mkdir", .command = mkdir_command},
                                    {.name = "pwd", .command = pwd_command},
                                    {.name = "rm", .command = rm_command},
                                    {.name = NULL, .command = NULL}};

int simple_cli_run_command(const char *name, int argc, const char **argv)
{
    if (!simple_cli_is_command(name))
    {
        return -1;
    }

    int i = 0;

    while (_commands[i].name != NULL)
    {
        if (strcmp(_commands[i].name, name) == 0)
        {
            return _commands[i].command(argc, argv);
        }

        ++i;
    }

    return -1;
}

int simple_cli_is_command(const char *name)
{
    if (!name)
    {
        return 0;
    }

    if (strcmp(name, "") == 0)
    {
        return 0;
    }

    int i = 0;

    while (_commands[i].name != NULL)
    {
        if (strcmp(_commands[i].name, name) == 0)
        {
            return 1;
        }

        ++i;
    }

    return 0;
}

int simple_cli_launch(char **args)
{
    pid_t pid;
    pid_t wpid;

    int status = 0;

    printf("Args before fork: %#016x\n", &args);

    phys_mem_dump_statistics();
    virt_mem_print_dir(process_get_current()->page_directory);

    pid = process_fork();

    //debug_print_process_tree();

    if (pid == 0)
    {
        printf("Args after fork(child): %#016x\n", &args);
        printf("Child Process: %i\n", process_get_pid());
        //exec_elf(args[0], simple_cli_get_token_count(args), args, NULL, 0);

        // exec_elf should not return

        process_exit(1337);
    }
    else if (pid < 0)
    {
        printf("[FORK] Error\n");
    }
    else
    {
        printf("Args after fork(parent): %#016x\n", &args);
        printf("Parent Process: %i\n", process_get_pid());
        do
        {
            wpid = waitpid(pid, &status, 0);
        } while (0);
    }

    printf("Child process exited with status code %i\n", (int64_t)status);

    return 1;
}

void simple_cli_init()
{
    while (1)
    {
        char *line = simple_cli_read_line();
        const char **args = simple_cli_split_line(line);
        //simple_cli_launch(args);

        if (simple_cli_is_command(args[0]))
        {
            long long int return_code = simple_cli_run_command(args[0],
                                                               simple_cli_get_token_count(args),
                                                               args);

            if (return_code)
            {
                printf("Command returned with code: %i\n", return_code);
            }
        }
        else
        {
            printf("Invalid command\n");
        }
        free(line);
        free(args);
    }
}