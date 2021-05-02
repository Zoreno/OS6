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

//=============================================================================
// Definitions
//=============================================================================

#define BUFFER_SIZE 1024

#define KEY_BACKSPACE 127
#define KEY_ENTER 13
#define KEY_ESCAPE VT_100_ESCAPE

//=============================================================================
// Local variables
//=============================================================================

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

//=============================================================================
// Private forward declarations
//=============================================================================

static int compare_string(void *a, void *b);
static int clamp(int val, int min, int max);
static void get_history_at(simple_cli_t *cli, int index, char *buffer);
static void insert_char(char *buffer, int buf_ptr, int buffer_size, char c);
static void erase_char(char *buffer, int buf_ptr, int buffer_size);

//=============================================================================
// Private functions
//=============================================================================

static int compare_string(void *a, void *b)
{
    const char *str1 = (const char *)a;
    const char *str2 = (const char *)b;

    return strcmp(str1, str2) == 0;
}

static int clamp(int val, int min, int max)
{
    if (val > max)
    {
        return max;
    }

    if (val < min)
    {
        return min;
    }

    return val;
}

static void get_history_at(simple_cli_t *cli, int index, char *buffer)
{
    if (!cli || !cli->history)
    {
        return;
    }

    list_node_t *node = cli->history->tail;

    if (!node)
    {
        return;
    }

    for (int i = 0; i < cli->history_index; ++i)
    {
        node = node->prev;
    }

    memcpy(buffer, node->payload, BUFFER_SIZE);
}

static void insert_char(char *buffer, int buf_ptr, int buffer_size, char c)
{
    // Move everything after the pointer one step
    memmove(&buffer[buf_ptr + 1], &buffer[buf_ptr], buffer_size - buf_ptr - 1);

    // Insert char at pointer
    buffer[buf_ptr] = c;
}

static void erase_char(char *buffer, int buf_ptr, int buffer_size)
{
    // Move everything after the pointer one step
    memmove(&buffer[buf_ptr], &buffer[buf_ptr + 1], buffer_size - buf_ptr - 1);

    // Clear the last element
    buffer[buffer_size] = 0;
}

//=============================================================================
// Interface functions
//=============================================================================

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

void simple_cli_append_to_history(simple_cli_t *cli, const char *line)
{
    list_node_t *prev_node =
        list_find_comp(cli->history, (char *)line, compare_string);

    if (prev_node != NULL)
    {
        list_delete(cli->history, prev_node);
        free(prev_node->payload);
        free(prev_node);
    }

    size_t length = strlen(line) + 1;

    char *new_line = malloc(length);

    memcpy(new_line, line, length);

    list_insert(cli->history, new_line);

    cli->history_index = -1;
}

int simple_cli_history_size(simple_cli_t *cli)
{
    return cli->history->length;
}

void simple_cli_next_history(simple_cli_t *cli, char *buffer)
{
    cli->history_index =
        clamp(cli->history_index + 1, 0, simple_cli_history_size(cli) - 1);

    get_history_at(cli, cli->history_index, buffer);
}

void simple_cli_prev_history(simple_cli_t *cli, char *buffer)
{
    cli->history_index =
        clamp(cli->history_index - 1, 0, simple_cli_history_size(cli) - 1);

    get_history_at(cli, cli->history_index, buffer);
}

void simple_cli_redraw_line(char *buffer, int buf_ptr)
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

    vt_100_set_cursor_location(line, strlen(line_start) + buf_ptr + 1);
}

char *simple_cli_read_line(simple_cli_t *cli)
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

    simple_cli_redraw_line(buffer, buf_ptr);

    while (1)
    {
        c = getc();

        switch (c)
        {
        case KEY_ENTER:
        {
            putchar('\n');
            return buffer;
        }
        case 8:
        case KEY_BACKSPACE:
        {
            if (buf_ptr > 0)
            {
                buf_ptr--;
                erase_char(buffer, buf_ptr, BUFFER_SIZE);
            }
        }
        break;
        case KEY_ESCAPE:
        {
            int command = vt_100_handle_command();

            switch (command)
            {
            case VT_100_UP:
                simple_cli_next_history(cli, buffer);
                buf_ptr = strlen(buffer);
                break;
            case VT_100_DOWN:
                simple_cli_prev_history(cli, buffer);
                buf_ptr = strlen(buffer);
                break;
            case VT_100_LEFT:
                if (buf_ptr > 0)
                {
                    buf_ptr--;
                }
                break;
            case VT_100_RIGHT:
                if (buf_ptr < strlen(buffer) && buf_ptr < BUFFER_SIZE - 1)
                {
                    buf_ptr++;
                }
                break;
            default:
                break;
            }
        }
        break;
        default:
            if (buf_ptr < BUFFER_SIZE - 1)
            {
                insert_char(buffer, buf_ptr, BUFFER_SIZE, c);
                buf_ptr++;
            }
            break;
        }

        simple_cli_redraw_line(buffer, buf_ptr);
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
    simple_cli_t cli;

    cli.history = list_create();
    cli.history_index = -1;

    while (1)
    {
        char *line = simple_cli_read_line(&cli);

        if (line)
        {
            simple_cli_append_to_history(&cli, line);
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
