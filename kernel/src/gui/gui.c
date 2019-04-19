#include <gui/gui.h>

#include <drivers/vbe.h>
#include <drivers/keyboard.h>
#include <drivers/mouse.h>

#include <gui/desktop.h>
#include <gui/window.h>
#include <gui/context.h>

void gui_init()
{
    uint16_t width = vbe_get_width();
    uint16_t height = vbe_get_height();
    uint32_t *framebuffer = vbe_get_buffer();

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

    Desktop *desktop = Desktop_new(context);

    Desktop_create_window(
        (Window *)desktop,
        10,
        10,
        300,
        200,
        0,
        "Window 1");

    Window_paint((Window *)desktop, NULL, 1);

    for (;;)
        ;
}