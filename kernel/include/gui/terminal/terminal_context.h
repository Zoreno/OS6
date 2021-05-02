/**
 * @file terminal_context.h
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

#ifndef _TERMINAL_CONTEXT_H
#define _TERMINAL_CONTEXT_H

#include <stdint.h>

typedef struct _terminal_context
{
    uint16_t width;
    uint16_t height;
    uint16_t bpp;
    uint32_t *framebuffer;

    int max_lines;
    int max_cols;
} terminal_context_t;

terminal_context_t *terminal_context_create(uint16_t width,
                                            uint16_t height,
                                            uint16_t bpp,
                                            uint32_t *framebuffer);

#endif

//=============================================================================
// End of file
//=============================================================================
