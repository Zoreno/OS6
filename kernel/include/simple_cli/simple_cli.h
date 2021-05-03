/**
 * @file simple_cli.h
 * @author Joakim Bertils
 * @version 0.1
 * @date 2020-08-09
 *
 * @brief Simple sh-like shell in the kernel
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

#ifndef _SIMPLE_CLI_H
#define _SIMPLE_CLI_H

#include <util/list.h>

typedef int (*simple_cli_command_func_t)(int, const char **);

typedef struct _simple_cli_command
{
    const char *name;
    simple_cli_command_func_t command;
} simple_cli_command_t;

typedef struct
{
    list_t *history;
    int history_index;
} simple_cli_t;

const char *simple_cli_get_user_name();
const char *simple_cli_get_user_group();
const char *simple_cli_get_working_directory();

void simple_cli_append_to_history(simple_cli_t *cli, const char *line);
int simple_cli_history_size(simple_cli_t *cli);
void simple_cli_next_history(simple_cli_t *cli, char *buffer);
void simple_cli_prev_history(simple_cli_t *cli, char *buffer);

void simple_cli_redraw_line(char *buffer, int buf_ptr);
char *simple_cli_read_line(simple_cli_t *cli);
const char **simple_cli_split_line(char *line);
int simple_cli_get_token_count(const char **tokens);
int simple_cli_run_command(const char *name, int argc, const char **argv);
int simple_cli_is_command(const char *name);
int simple_cli_launch(char **args);
void simple_cli_init();

#endif

//=============================================================================
// End of file
//=============================================================================
