/**
 * @file terminal_buffer.c
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

#include <gui/terminal/terminal_buffer.h>

#include <stdlib.h>
#include <string.h>

terminal_buffer_t *terminal_buffer_create()
{
    terminal_buffer_t *buffer = malloc(sizeof(terminal_buffer_t));

    if (!buffer)
    {
        return NULL;
    }

    buffer->user = strdup("zoreno");
    buffer->group = strdup("os6");

    // This should be a static buffer to be able to rewrite it
    buffer->working_directory = strdup("~/docs/test");

    buffer->input_line = malloc(128);
    memset(buffer->input_line, 0, 128);
    buffer->input_line_length = 0;
    buffer->input_line_max_size = 128;

    vector_init(buffer->lines);

    vector_add(buffer->lines, strdup("Hello world"));

    return buffer;
}

void terminal_buffer_append_input(terminal_buffer_t *buffer, char c)
{
    if (buffer->input_line_length + 1 >= buffer->input_line_max_size)
    {
        // TODO: This should be checked for NULL
        buffer->input_line = realloc(buffer->input_line, buffer->input_line_max_size * 2);
        buffer->input_line_max_size *= 2;
        // TODO: Clear the new part
    }

    buffer->input_line[buffer->input_line_length++] = c;
}

void terminal_buffer_append_string(terminal_buffer_t *buffer, const char *string)
{
    size_t length = strlen(string);

    while (buffer->input_line_length + length >= buffer->input_line_max_size)
    {
        // TODO: This should be checked for NULL
        buffer->input_line = realloc(buffer->input_line, buffer->input_line_max_size * 2);
        buffer->input_line_max_size *= 2;
        // TODO: Clear the new part
    }

    strcpy(buffer->input_line + buffer->input_line_length, string);
}

void terminal_buffer_delete_input(terminal_buffer_t *buffer)
{
    if (buffer->input_line_length > 0)
    {
        buffer->input_line[--buffer->input_line_length] = 0;
    }
}

//=============================================================================
// End of file
//=============================================================================