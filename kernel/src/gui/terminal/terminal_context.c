#include <gui/terminal/terminal_context.h>

#include <stdlib.h>

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
