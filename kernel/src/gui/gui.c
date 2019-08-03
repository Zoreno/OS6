/**
 * @file gui.c
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

#include <gui/gui.h>

#include <drivers/vbe.h>
#include <drivers/keyboard_ps2.h>
#include <drivers/mouse_ps2.h>

#include <gui/desktop.h>
#include <gui/window.h>
#include <gui/context.h>

#include <arch/arch.h>

Desktop *desktop;

void gui_mouse_callback(uint16_t mouse_x, uint16_t mouse_y, uint8_t buttons);

static uint8_t buttons;
static uint16_t mouse_x;
static uint16_t mouse_y;

void gui_mouse_button_handler(mouse_button_event_t *ev)
{
    if (ev->action == MOUSE_ACTION_PRESS)
    {
        buttons |= ev->button & 0xF;
    }
    else if (ev->action == MOUSE_ACTION_RELEASE)
    {
        buttons &= ~(ev->button & 0xF);
    }

    gui_mouse_callback(mouse_x, mouse_y, buttons);
}

void gui_mouse_moved_handler(mouse_moved_event_t *ev)
{
    printf("Mouse pos: %i,%i\n", (int64_t)ev->x, (int64_t)-ev->y);
    printf("Mouse mov: %i,%i\n", (int64_t)ev->dx, (int64_t)-ev->dy);

    mouse_x = ev->x;
    mouse_y = -ev->y;

    //gui_mouse_callback(ev->x, -ev->y, buttons);
}

void gui_mouse_callback(uint16_t mouse_x, uint16_t mouse_y, uint8_t buttons)
{
    Desktop_process_mouse(desktop, mouse_x, mouse_y, buttons);
}

extern int timer_updated;

void gui_init()
{
    uint16_t width = vbe_get_width();
    uint16_t height = vbe_get_height();
    uint32_t *framebuffer = (uint32_t *)vbe_get_buffer();

#if 0

    vbe_pixel_t p;

    p.red = 0x16;
    p.green = 0x62;
    p.blue = 0xFF;

    vbe_fill_rect(p, 0, 0, width, height);

    for (;;)
        ;

#endif

    Context *context = Context_new(width, height, framebuffer);

    desktop = Desktop_new(context);

    cli();
    Desktop_create_window(
        (Window *)desktop,
        10,
        10,
        300,
        200,
        0,
        "Window 1");

    Desktop_create_window(
        (Window *)desktop,
        400,
        10,
        300,
        200,
        0,
        "Window 2");

    sti();

    register_mouse_moved_handler(gui_mouse_moved_handler);
    register_mouse_button_handler(gui_mouse_button_handler);

    while (1)
    {
        if (timer_updated)
        {
            timer_updated = 0;
            cli();
            //Window_paint((Window *)desktop, NULL, 1);
            Desktop_invalidate_start_bar((Window *)desktop);
            sti();
        }
        else
        {
            __asm__ volatile("hlt");
        }
    }
}