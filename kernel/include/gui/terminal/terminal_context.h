#ifndef _TERMINAL_CONTEXT_H
#define _TERMINAL_CONTEXT_H

#include <stdint.h>

typedef struct _terminal_context
{
    uint16_t width;
    uint16_t height;
    uint16_t bpp;
    uint32_t *framebuffer;
} terminal_context_t;

terminal_context_t *terminal_context_create(uint16_t width, uint16_t height, uint16_t bpp, uint32_t *framebuffer);

#endif