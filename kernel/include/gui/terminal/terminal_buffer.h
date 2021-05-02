/**
 * @file terminal_buffer.h
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-06-18
 * 
 * @brief Terminal buffer
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

#ifndef _TERMINAL_BUFFER_H
#define _TERMINAL_BUFFER_H

#include <util/vector.h>

typedef struct _terminal_buffer
{
    char *user;
    char *group;

    char *working_directory;

    char *name;

    vector_t *lines;

    char *input_line;
    int input_line_length;
    int input_line_max_size;

} terminal_buffer_t;

terminal_buffer_t *terminal_buffer_create();

// TODO: Maybe unicode
void terminal_buffer_append_input(terminal_buffer_t *buffer, char c);
void terminal_buffer_append_string(terminal_buffer_t *buffer, const char *string);

void terminal_buffer_delete_input(terminal_buffer_t *buffer);

#endif

//=============================================================================
// End of file
//=============================================================================
