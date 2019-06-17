/**
 * @file terminal_mode.c
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-06-17
 * 
 * @brief Pixel mode terminal for JOS
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

#include <gui/terminal_mode.h>

#include <drivers/vbe.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <arch/arch.h>

//=============================================================================
// Terminal color
//=============================================================================

typedef struct _terminal_color
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} terminal_color_t;

//=============================================================================
// Terminal buffer
//=============================================================================

typedef struct _terminal_buffer
{
    char *user;
    char *group;

    char *working_directory;

    char **lines;
    int num_lines;

    char *input_line;
    int input_line_size;

} terminal_buffer_t;

terminal_buffer_t *terminal_buffer_create();

terminal_buffer_t *terminal_buffer_create()
{
    terminal_buffer_t *buffer = malloc(sizeof(terminal_buffer_t));

    if (!buffer)
    {
        return NULL;
    }

    buffer->user = strdup("zoreno");
    buffer->group = strdup("ubuntu");
    buffer->working_directory = strdup("~/Docs/test");

    return buffer;
}

//=============================================================================
// Terminal context
//=============================================================================

typedef struct _terminal_context
{
    uint16_t width;
    uint16_t height;
    uint16_t bpp;
    uint32_t *framebuffer;
} terminal_context_t;

terminal_context_t *terminal_context_create(uint16_t width, uint16_t height, uint16_t bpp, uint32_t *framebuffer);

terminal_context_t *terminal_context_create(uint16_t width, uint16_t height, uint16_t bpp, uint32_t *framebuffer)
{
    terminal_context_t *context = malloc(sizeof(terminal_context_t));

    if (!context)
    {
        return context;
    }

    context->width = width;
    context->height = height;
    context->bpp = bpp;
    context->framebuffer = framebuffer;

    return context;
}

//=============================================================================
// Terminal
//=============================================================================

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

    terminal->context = terminal_context_create(width, height, bpp, framebuffer);

    terminal->max_lines = height / 8;
    terminal->max_cols = width / 8;

    printf("Max Cols: %i, Max Lines: %i\n", terminal->max_cols, terminal->max_lines);

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

    // TODO: Add to the list of buffers

    return buffer;
}

void terminal_destroy_buffer(terminal_t *terminal, terminal_buffer_t *buffer)
{
}

void terminal_redraw(terminal_t *terminal)
{
    static int i = 0;

    vbe_pixel_t black;
    black.red = 0;
    black.green = 0;
    black.blue = 0;

    vbe_pixel_t white;
    white.red = 255;
    white.green = 255;
    white.blue = 255;

    vbe_fill_rect(black, 0, 0, terminal->context->width, 8);
    char str[100];
    sprintf(str, "The awesome terminal: %i\n", i++);
    vbe_print_string(white, 0, 8, str);

    // TODO: Redirect to the current buffer
}

//=============================================================================
// External Interface
//=============================================================================

static terminal_t *_terminal;

void terminal_init()
{

    _terminal = terminal_create();

    while (1)
    {
        cli();
        terminal_redraw(_terminal);
        sti();

        mdelay(20);
    }
}

//=============================================================================
// End of file
//=============================================================================