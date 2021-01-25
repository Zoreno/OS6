/**
 * @file simple_cli.c
 * @author Joakim Bertils
 * @version 0.1
 * @date 2020-12-31
 *
 * @brief
 *
 * @copyright Copyright (C) 2020,
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

#include <simple_cli/simple_cli.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <process/process.h>
#include <simple_cli/commands.h>
#include <simple_cli/vt_100.h>

#define BUFFER_SIZE 1024

char *last_command = NULL;

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

void simple_cli_redraw_line(char *buffer)
{
    char line_start[BUFFER_SIZE] = {0};
    sprintf(line_start,
            "%s@%s:%s$ ",
            simple_cli_get_user_name(),
            simple_cli_get_user_group(),
            simple_cli_get_working_directory());

    vt_100_erase_line();

    int line;
    vt_100_get_cursor_location(&line, NULL);
    vt_100_set_cursor_location(line, 0);

    printf("%s", line_start);
    printf("%s", buffer);
}

#define KEY_BACKSPACE 127
#define KEY_ENTER 13
#define KEY_ESCAPE VT_100_ESCAPE

char *simple_cli_read_line()
{
    char c;

    char *buffer = malloc(BUFFER_SIZE);
    int buf_ptr = 0;

    if (!buffer)
    {
        printf("[ERROR] Allocating buffer\n");
        for (;;)
            ;
    }

    memset(buffer, 0, BUFFER_SIZE);

    simple_cli_redraw_line(buffer);

    while (1)
    {
        c = getc();

        switch (c)
        {
        case KEY_ENTER:
        {
            putchar('\n');
            buffer[buf_ptr] = 0;
            return buffer;
        }
        case KEY_BACKSPACE:
        {
            if (buf_ptr > 0)
            {
                buf_ptr--;
                buffer[buf_ptr] = 0;
            }
        }
        break;
        case KEY_ESCAPE:
        {
            int command = vt_100_handle_command();

            if (command == VT_100_UP)
            {
                memcpy(buffer, last_command, BUFFER_SIZE);
                buf_ptr = strlen(last_command);
            }
        }
        break;
        default:

            buffer[buf_ptr++] = c;
            break;
        }

        simple_cli_redraw_line(buffer);
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

simple_cli_command_t _commands[] = {
    {.name = "test", .command = test_command},
    {.name = "cd", .command = cd_command},
    {.name = "ls", .command = ls_command},
    {.name = "mkdir", .command = mkdir_command},
    {.name = "pwd", .command = pwd_command},
    {.name = "rm", .command = rm_command},
    {.name = "exit", .command = exit_command},
    {.name = "time", .command = time_command},
    {.name = "launch", .command = launch_command},
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

    // debug_print_process_tree();

    if (pid == 0)
    {
        printf("Args after fork(child): %#016x\n", &args);
        printf("Child Process: %i\n", process_get_pid());
        // exec_elf(args[0], simple_cli_get_token_count(args), args, NULL,
        // 0);

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
    last_command = malloc(BUFFER_SIZE);

    while (1)
    {
        char *line = simple_cli_read_line();

        if (line)
        {
            memcpy(last_command, line, BUFFER_SIZE);
        }

        const char **args = simple_cli_split_line(line);

        if (simple_cli_is_command(args[0]))
        {
            long long int return_code = simple_cli_run_command(
                args[0], simple_cli_get_token_count(args), args);

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

//=============================================================================
// End of file
//=============================================================================