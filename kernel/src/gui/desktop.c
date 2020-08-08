/**
 * @file desktop.c
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

#include <gui/desktop.h>

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <gui/rect.h>
#include <gui/context.h>

#include <cmos/cmos_rtc.h>

#include <debug/backtrace.h>

#define cX 0xFF000000
#define cO 0xFFFFFFFF
#define c_ 0x00000000

unsigned int mouse_img[MOUSE_BUFSZ] = {
    cX, c_, c_, c_, c_, c_, c_, c_, c_, c_, c_,
    cX, cX, c_, c_, c_, c_, c_, c_, c_, c_, c_,
    cX, cO, cX, c_, c_, c_, c_, c_, c_, c_, c_,
    cX, cO, cO, cX, c_, c_, c_, c_, c_, c_, c_,
    cX, cO, cO, cO, cX, c_, c_, c_, c_, c_, c_,
    cX, cO, cO, cO, cO, cX, c_, c_, c_, c_, c_,
    cX, cO, cO, cO, cO, cO, cX, c_, c_, c_, c_,
    cX, cO, cO, cO, cO, cO, cO, cX, c_, c_, c_,
    cX, cO, cO, cO, cO, cO, cO, cO, cX, c_, c_,
    cX, cO, cO, cO, cO, cO, cO, cO, cO, cX, c_,
    cX, cO, cO, cO, cO, cO, cO, cO, cO, cO, cX,
    cX, cO, cO, cX, cO, cO, cO, cX, cX, cX, cX,
    cX, cO, cX, c_, cX, cO, cO, cX, c_, c_, c_,
    cX, cX, c_, c_, cX, cO, cO, cX, c_, c_, c_,
    cX, c_, c_, c_, c_, cX, cO, cO, cX, c_, c_,
    c_, c_, c_, c_, c_, cX, cO, cO, cX, c_, c_,
    c_, c_, c_, c_, c_, c_, cX, cO, cX, c_, c_,
    c_, c_, c_, c_, c_, c_, c_, cX, cX, c_, c_};

#define TASKBAR_HEIGHT 32
#define TASKBAR_ICON_WIDTH 128

void Desktop_paint_taskbar(Window *taskbar_window);
void Desktop_taskbar_process_mouse(
    Window *taskbar_window,
    int mouse_x,
    int mouse_y,
    uint8_t mouse_buttons);

void Desktop_taskbar_tick_handler(Window *taskbar_window, int ticks);

Desktop *Desktop_new(Context *context)
{
    Desktop *desktop;

    if (!(desktop = (Desktop *)malloc(sizeof(Desktop))))
    {
        return desktop;
    }

    if (!Window_init(
            (Window *)desktop,
            0,
            0,
            context->width,
            context->height,
            WIN_NODECORATION,
            context,
            0))
    {
        free(desktop);
        return (Desktop *)0;
    }

    desktop->window.parent = (Window *)0;

    if (!(desktop->taskbar = (Window *)malloc(sizeof(Window))))
    {
        free(desktop);
        return (Desktop *)0;
    }

    if (!Window_init(
            desktop->taskbar,
            0,
            desktop->window.height - TASKBAR_HEIGHT,
            desktop->window.width,
            TASKBAR_HEIGHT,
            WIN_NODECORATION | WIN_NO_RESIZE | WIN_NO_DRAG | WIN_FLOATING,
            context,
            0))
    {
        free(desktop);
        return (Desktop *)0;
    }
    desktop->window.inner_height = desktop->window.height - TASKBAR_HEIGHT;
    desktop->taskbar->paint_function = Desktop_paint_taskbar;
    desktop->taskbar->mousedown_function = Desktop_taskbar_process_mouse;
    desktop->taskbar->tick_function = Desktop_taskbar_tick_handler;

    Window_insert_child((Window *)desktop, desktop->taskbar);

    desktop->window.paint_function = Desktop_paint_handler;
    desktop->window.key_function = (WindowKeyHandler)0;

    desktop->window.last_button_state = 0;

    desktop->mouse_x = desktop->window.context->width / 2;
    desktop->mouse_y = desktop->window.context->height / 2;

    return desktop;
}

void Desktop_taskbar_process_mouse(
    Window *taskbar_window,
    int mouse_x,
    int mouse_y,
    uint8_t mouse_buttons)
{
    uint32_t i;
    Window *desktop_window = taskbar_window->parent;
    Window *child;
    int current_index = 0;

    if (taskbar_window->last_button_state != mouse_buttons)
    {
        for (i = 0; i < desktop_window->children->count; ++i)
        {
            child = (Window *)gui_list_get_at(desktop_window->children, i);

            if (child->flags & WIN_FLOATING)
            {
                continue;
            }

            if (mouse_x > (48 + (TASKBAR_ICON_WIDTH + 10) * current_index) &&
                mouse_x <= (48 + (TASKBAR_ICON_WIDTH + 10) * current_index + TASKBAR_ICON_WIDTH) &&
                mouse_y > 6 &&
                mouse_y <= 26)
            {

                if (child == desktop_window->active_child)
                {
                    Window_minimize(child);
                }
                else
                {
                    if (Window_minimized(child))
                    {
                        Window_restore(child);
                    }
                    else
                    {
                        Window_raise(child, 1);
                    }
                }
                break;
            }

            ++current_index;
        }
    }

    taskbar_window->last_button_state = mouse_buttons;
}

void Desktop_paint_taskbar(Window *taskbar_window)
{
    uint32_t i;
    int current_index = 0;
    Window *child;
    int active_child;
    Window *desktop_window = taskbar_window->parent;

    Context_fill_rect(
        desktop_window->context,
        0,
        0,
        taskbar_window->width,
        taskbar_window->height,
        0xFF101010);

    for (i = 0; i < desktop_window->children->count; ++i)
    {
        child = (Window *)gui_list_get_at(desktop_window->children, i);

        if (child->flags & WIN_FLOATING)
        {
            continue;
        }

        active_child = desktop_window->active_child && (desktop_window->active_child == child);

        Context_fill_rect(
            desktop_window->context,
            48 + (TASKBAR_ICON_WIDTH + 10) * current_index,
            6,
            TASKBAR_ICON_WIDTH,
            20,
            active_child ? 0xFFFFFFFF : 0xFF8F8F8F);

        Context_draw_text(
            desktop_window->context,
            child->title,
            52 + (TASKBAR_ICON_WIDTH + 10) * current_index,
            9,
            0xFF000000);

        ++current_index;
    }

    char full_time_buf[RTC_TIME_STR_MINLEN] = {0};

    RTC_time_to_string(full_time_buf, NULL);

    full_time_buf[8] = 0;

    Context_draw_text(
        desktop_window->context,
        full_time_buf,
        desktop_window->width - 80,
        4,
        0xFFFFFFFF);

    Context_draw_text(
        desktop_window->context,
        full_time_buf + 9,
        desktop_window->width - 90,
        20,
        0xFFFFFFFF);
}

void Desktop_taskbar_tick_handler(Window *taskbar_window, int ticks)
{
    //printf("Taskbar got tick %i\n", ticks);

    taskbar_window->last_tick += ticks;

    if (taskbar_window->last_tick < 500)
    {
        return;
    }

    taskbar_window->last_tick -= 500;

    Window *desktop_window = taskbar_window->parent;

    char full_time_buf[RTC_TIME_STR_MINLEN] = {0};

    RTC_time_to_string(full_time_buf, NULL);

    full_time_buf[9] = 0;

    Context_draw_text(
        desktop_window->context,
        full_time_buf,
        desktop_window->width - 80,
        4,
        0xFFFFFFFF);

    Context_draw_text(
        desktop_window->context,
        full_time_buf + 9,
        desktop_window->width - 90,
        20,
        0xFFFFFFFF);

    Window_invalidate(
        taskbar_window,
        0,
        taskbar_window->width - 90,
        taskbar_window->height - 1,
        taskbar_window->width - 1);
}

void Desktop_process_mouse(
    Desktop *desktop,
    uint16_t mouse_x,
    uint16_t mouse_y,
    uint8_t mouse_buttons)
{
    uint32_t i;
    int x;
    int y;

    Window *child;
    Window *desktop_window = (Window *)desktop;
    gui_list_t *dirty_list;
    rect_t *mouse_rect;

    gui_list_t *pending_remove;

    /*
     * Do the mouse handling/painting
     */
    Window_process_mouse((Window *)desktop, mouse_x, mouse_y, mouse_buttons);

    if ((pending_remove = gui_list_new()) != 0)
    {
        for (i = 0; i < desktop_window->children->count; ++i)
        {
            child = (Window *)gui_list_get_at(desktop_window->children, i);

            if (child->flags & WIN_SHOULD_CLOSE)
            {
                gui_list_add(pending_remove, child);
            }
        }

        while (pending_remove->count)
        {
            child = (Window *)gui_list_remove_at(pending_remove, 0);

            Desktop_remove_window(desktop_window, child);
        }

        free(pending_remove);
    }

    if (!(dirty_list = gui_list_new()))
    {
        return;
    }

    if (!(mouse_rect = rect_new(
              desktop->mouse_y,
              desktop->mouse_x,
              desktop->mouse_y + MOUSE_HEIGHT - 1,
              desktop->mouse_x + MOUSE_WIDTH - 1)))
    {
        free(dirty_list);
        return;
    }

    gui_list_add(dirty_list, mouse_rect);

    Window_paint(desktop_window, dirty_list, 1);

    gui_list_remove_at(dirty_list, 0);
    free(dirty_list);
    free(mouse_rect);

    desktop->mouse_x = mouse_x;
    desktop->mouse_y = mouse_y;

    for (y = 0; y < MOUSE_HEIGHT; ++y)
    {
        if ((y + mouse_y) >= desktop->window.context->height)
        {
            break;
        }

        for (x = 0; x < MOUSE_WIDTH; ++x)
        {
            if ((x + mouse_x) >= desktop->window.context->width)
            {
                break;
            }

            if (mouse_img[y * MOUSE_WIDTH + x] & 0xFF000000)
            {
                desktop->window.context->buffer[(y + mouse_y) * desktop->window.context->width + (x + mouse_x)] =
                    mouse_img[y * MOUSE_WIDTH + x];
            }
        }
    }
}

void Desktop_invalidate_start_bar(Window *desktop_window)
{
    Desktop *desktop = (Desktop *)desktop_window;

    Window_invalidate(
        desktop_window,
        desktop->taskbar->y,
        desktop->taskbar->x,
        desktop->taskbar->y + desktop->taskbar->height,
        desktop->taskbar->x + desktop->taskbar->width);
}

void Desktop_paint_handler(Window *desktop_window)
{

    Desktop *desktop = (Desktop *)desktop_window;
    // TODO: Draw bitmap when possible
    /*
    Context_fill_rect(
        desktop_window->context, 
        0, 
        0, 
        desktop_window->context->width,
        desktop_window->context->height, 
        0xFFFF9933); 
*/

    Context_fill_rect(
        desktop_window->context,
        0,
        0,
        desktop_window->context->width / 2,
        desktop_window->context->height / 2,
        0xFF69BEEF);

    Context_fill_rect(
        desktop_window->context,
        desktop_window->context->width / 2,
        0,
        desktop_window->context->width / 2,
        desktop_window->context->height / 2,
        0xFFBEEF69);

    Context_fill_rect(
        desktop_window->context,
        0,
        desktop_window->context->height / 2,
        desktop_window->context->width / 2,
        desktop_window->context->height / 2,
        0xFFBEEF69);

    Context_fill_rect(
        desktop_window->context,
        desktop_window->context->width / 2,
        desktop_window->context->height / 2,
        desktop_window->context->width / 2,
        desktop_window->context->height / 2,
        0xFF69BEEF);
}

Window *Desktop_create_window(
    Window *desktop_window,
    int16_t x,
    int16_t y,
    uint16_t width,
    uint16_t height,
    uint16_t flags,
    char *title)
{
    Window *new_window = Window_create_window(desktop_window, x, y, width, height, flags);

    if (!new_window)
    {
        return new_window;
    }

    Window_set_title(new_window, title);

    Desktop_invalidate_start_bar(desktop_window);

    Window_paint(desktop_window, (gui_list_t *)0, 1);

    return new_window;
}

void Desktop_remove_window(
    Window *desktop_window,
    Window *window)
{

    uint32_t window_index = window->index;
    uint32_t i;

    Window *child;

    for (i = 0; i < desktop_window->children->count; ++i)
    {
        child = (Window *)gui_list_get_at(desktop_window->children, i);

        if (child->index > window_index)
        {
            --child->index;
        }
    }

    Window_remove(window);

    desktop_window->active_child = (Window *)0;

    Desktop_invalidate_start_bar(desktop_window);

    Window_paint(desktop_window, (gui_list_t *)0, 1);
}

/* "'(file-name-nondirectory (buffer-file-name))'" ends here */