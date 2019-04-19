#ifndef _DESKTOP_H_
#define _DESKTOP_H_

#include <gui/list.h>
#include <gui/context.h>
#include <gui/window.h>

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

void Desktop_process_mouse(
    Desktop *desktop,
    uint16_t x,
    uint16_t y,
    uint8_t mouse_button);

void Desktop_paint_handler(
    Window *desktop_window);

void Desktop_invalidate_start_bar(
    Window *desktop_window);

Window *Desktop_create_window(
    Window *desktop_window,
    int16_t x,
    int16_t y,
    uint16_t width,
    uint16_t height,
    uint16_t flags,
    char *title);

void Desktop_remove_window(
    Window *desktop_window,
    Window *window);

#endif // _DESKTOP_H_