/**
 * @file desktop.h
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

#ifndef _DESKTOP_H_
#define _DESKTOP_H_

#include <gui/context.h>
#include <gui/gui_list.h>
#include <gui/window.h>

#include <stdint.h>

#define MOUSE_WIDTH 11
#define MOUSE_HEIGHT 18
#define MOUSE_BUFSZ (MOUSE_WIDTH * MOUSE_HEIGHT)

#define DESKTOP_BACKGROUND

typedef struct Desktop_struct
{
    Window window;
    uint16_t mouse_x;
    uint16_t mouse_y;

    Window *taskbar;

} Desktop;

Desktop *Desktop_new(Context *context);

void Desktop_process_mouse(Desktop *desktop,
                           uint16_t x,
                           uint16_t y,
                           uint8_t mouse_button);

void Desktop_paint_handler(Window *desktop_window);

void Desktop_invalidate_start_bar(Window *desktop_window);

Window *Desktop_create_window(Window *desktop_window,
                              int16_t x,
                              int16_t y,
                              uint16_t width,
                              uint16_t height,
                              uint16_t flags,
                              char *title);

void Desktop_remove_window(Window *desktop_window, Window *window);

#endif  // _DESKTOP_H_

//=============================================================================
// End of file
//=============================================================================
