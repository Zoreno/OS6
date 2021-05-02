/**
 * @file terminal_context.c
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-06-21
 *
 * @brief Terminal rendering context
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

#include <gui/terminal/terminal_context.h>

#include <stdlib.h>

terminal_context_t *terminal_context_create(uint16_t width,
                                            uint16_t height,
                                            uint16_t bpp,
                                            uint32_t *framebuffer)
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

    context->max_lines = height / 8;
    context->max_cols = width / 8;

    return context;
}

//=============================================================================
// End of file
//=============================================================================
