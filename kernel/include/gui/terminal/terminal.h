/**
 * @file terminal.h
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-06-18
 * 
 * @brief Terminal for the OS6 operating system
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

#ifndef _TERMINAL_H
#define _TERMINAL_H

#include <gui/terminal/terminal_context.h>
#include <gui/terminal/terminal_buffer.h>

typedef struct _terminal
{
    terminal_context_t *context;

    int max_lines;
    int max_cols;

    terminal_buffer_t *current_buffer;

    terminal_buffer_t *buffers;
    int num_buffers;

} terminal_t;

terminal_t *terminal_create();

terminal_buffer_t *terminal_create_buffer(terminal_t *terminal);
void terminal_destroy_buffer(terminal_t *terminal, terminal_buffer_t *buffer);
void terminal_redraw(terminal_t *terminal);

#endif