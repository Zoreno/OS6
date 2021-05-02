/**
 * @file terminal.c
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-06-18
 *
 * @brief
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

#include <gui/terminal/terminal.h>

#include <drivers/vbe.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <gui/terminal/terminal_color.h>

#include <cmos/cmos_rtc.h>

terminal_t *terminal_create()
{
    terminal_t *terminal = malloc(sizeof(terminal_t));

    if (!terminal)
    {
        return NULL;
    }

    uint16_t width = vbe_get_width();
    uint16_t height = vbe_get_height();
    uint16_t bpp = vbe_get_bpp();
    uint32_t *framebuffer = (uint32_t *)vbe_get_buffer();

    terminal->context =
        terminal_context_create(width, height, bpp, framebuffer);

    printf("Max Cols: %i, Max Lines: %i\n",
           terminal->context->max_cols,
           terminal->context->max_lines);

    terminal->buffers = malloc(sizeof(vector_t));

    vector_init(terminal->buffers);

    terminal_create_buffer(terminal);

    return terminal;
}

terminal_buffer_t *terminal_create_buffer(terminal_t *terminal)
{
    terminal_buffer_t *buffer = terminal_buffer_create();

    if (!buffer)
    {
        return NULL;
    }

    terminal->current_buffer = buffer;

    terminal_buffer_append_input(buffer, 'a');
    terminal_buffer_append_input(buffer, 'b');
    terminal_buffer_append_input(buffer, 'c');

    terminal_buffer_delete_input(buffer);

    vector_add(terminal->buffers, buffer);

    return buffer;
}

void terminal_destroy_buffer(terminal_t *terminal, terminal_buffer_t *buffer)
{
}

void terminal_redraw(terminal_t *terminal)
{
    static int i = 0;

    terminal_color_t color = terminal_color_get_from_8bit(2);

    vbe_pixel_t background;
    background.red = color.red;
    background.green = color.green;
    background.blue = color.blue;

    color = terminal_color_get_from_8bit(15);

    vbe_pixel_t foreground;
    foreground.red = color.red;
    foreground.green = color.green;
    foreground.blue = color.blue;

    char full_time_buf[RTC_TIME_STR_MINLEN] = {0};

    RTC_time_to_string(full_time_buf, NULL);

    vbe_fill_rect(background, 0, 0, terminal->context->width, 8);
    char str[100];
    sprintf(str, "The awesome terminal: %i, %s\n", i++, full_time_buf);
    vbe_print_string(foreground, 0, 8, str);

    vbe_fill_rect(background, 0, 8, terminal->context->width, 8);
    sprintf(str, "|");

    int terminal_count = vector_count(terminal->buffers);

    for (int j = 0; j < terminal_count; ++j)
    {
        sprintf(str,
                "%s %s |",
                str,
                ((terminal_buffer_t *)vector_get(terminal->buffers, j))->name);
    }

    vbe_print_string(foreground, 0, 16, str);

    if (terminal->current_buffer)
    {
        int lines_count = vector_count(terminal->current_buffer->lines);

        for (int lines = 0; lines < lines_count; ++lines)
        {
            vbe_fill_rect(
                background, 0, 8 * lines + 16, terminal->context->width, 8);
            vbe_print_string(
                foreground,
                0,
                8 * lines + 16 + 8,
                vector_get(terminal->current_buffer->lines, lines));
        }

        vbe_fill_rect(background,
                      0,
                      terminal->context->height - 8,
                      terminal->context->width,
                      8);
        char str[100];
        sprintf(str,
                "%s@%s:%s$",
                terminal->current_buffer->user,
                terminal->current_buffer->group,
                terminal->current_buffer->working_directory);

        size_t string_length = strlen(str);

        vbe_print_string(foreground, 0, terminal->context->height, str);
        vbe_print_string(foreground,
                         8 * string_length,
                         terminal->context->height,
                         terminal->current_buffer->input_line);
    }
}

//=============================================================================
// End of file
//=============================================================================
