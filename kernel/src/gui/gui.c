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

void gui_mouse_moved_handler(mouse_moved_event_t *ev)
{
    //printf("Mouse pos: %i,%i\n", (int64_t)ev->x, (int64_t)-ev->y);
    //printf("Mouse mov: %i,%i\n", (int64_t)ev->dx, (int64_t)-ev->dy);
    gui_mouse_callback(ev->x, -ev->y, 0);
}

void gui_mouse_callback(uint16_t mouse_x, uint16_t mouse_y, uint8_t buttons)
{
    Desktop_process_mouse(desktop, mouse_x, mouse_y, buttons);
}

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

    Desktop_create_window(
        (Window *)desktop,
        10,
        10,
        300,
        200,
        0,
        "Window 1");

    //register_mouse_moved_handler(gui_mouse_moved_handler);

    while (1)
    {
        Window_paint((Window *)desktop, NULL, 1);
        mdelay(1000);
    }
}