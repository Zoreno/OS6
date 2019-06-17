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

} terminal_buffer_t;

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

    terminal_buffer_t *current_buffer;

    terminal_buffer_t *buffers;
    int num_buffers;

} terminal_t;

terminal_t *terminal_create();

terminal_buffer_t *terminal_create_buffer();
void terminal_destroy_buffer(terminal_buffer_t *buffer);

//=============================================================================
// External Interface
//=============================================================================

static terminal_t *_terminal;

void terminal_init()
{
    uint16_t width = vbe_get_width();
    uint16_t height = vbe_get_height();
    uint16_t bpp = vbe_get_bpp();
    uint32_t *framebuffer = (uint32_t *)vbe_get_buffer();

    vbe_pixel_t p;

    p.red = 255;
    p.green = 255;
    p.blue = 255;

    vbe_print_string(p, 0, 8, "joakim@os6:~/Documents/OS6$");

    while (1)
        ;
}

//=============================================================================
// End of file
//=============================================================================