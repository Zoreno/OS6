/**
 * @file context.h
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-06-22
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

#ifndef _CONTEXT_H_
#define _CONTEXT_H_

#include <stdint.h>
#include <gui/rect.h>
#include <gui/gui_list.h>

#include <stdio.h>

typedef struct Context_struct
{
    uint32_t *buffer;
    uint16_t width;
    uint16_t height;

    int translate_x;
    int translate_y;

    gui_list_t *clip_rects;

    uint8_t clipping_on;

} Context;

Context *Context_new(
    uint16_t width,
    uint16_t height,
    uint32_t *buffer);

void Context_clipped_rect(
    Context *context,
    int x,
    int y,
    unsigned int width,
    unsigned int height,
    rect_t *clip_area,
    uint32_t color);

void Context_fill_rect(
    Context *context,
    int x,
    int y,
    unsigned int width,
    unsigned int height,
    uint32_t color);

void Context_clipped_bitmap(
    Context *context,
    int source_x,
    int source_y,
    int dest_x,
    int dest_y,
    unsigned int width,
    unsigned int height,
    rect_t *clip_area,
    unsigned int *source);

void Context_fill_bitmap(
    Context *context,
    int source_x,
    int source_y,
    int dest_x,
    int dest_y,
    unsigned int width,
    unsigned int height,
    unsigned int *source);

void Context_horizontal_line(
    Context *context,
    int x,
    int y,
    unsigned int length,
    uint32_t color);

void Context_vertical_line(
    Context *context,
    int x,
    int y,
    unsigned int length,
    uint32_t color);

void Context_draw_rect(
    Context *context,
    int x,
    int y,
    unsigned int width,
    unsigned int height,
    uint32_t color);

void Context_intersect_clip_rect(
    Context *context,
    rect_t *rect);

void Context_subtract_clip_rect(
    Context *context,
    rect_t *subtracted_rect);

void Context_add_clip_rect(
    Context *context,
    rect_t *added_rect);

void Context_clear_clip_rects(
    Context *context);

void Context_draw_text(
    Context *context,
    char *string,
    int x,
    int y,
    uint32_t color);

#endif // _CONTEXT_H_