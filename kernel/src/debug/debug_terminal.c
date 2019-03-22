#include <debug/debug_terminal.h>

typedef struct
{
    uint8_t character;
    uint8_t text_color : 4;
    uint8_t back_color : 4;
} __attribute__((packed)) char_entry_t;

static uint32_t offset(uint32_t x, uint32_t y);
static void scroll();

static char_entry_t *_VIDMEM = (char_entry_t *)0xB8000;
static uint32_t _cursor_x = 0;
static uint32_t _cursor_y = 0;

static vga_color_t _default_text_color = VGA_COLOR_BLACK;
static vga_color_t _default_back_color = VGA_COLOR_WHITE;

static vga_color_t _text_color;
static vga_color_t _back_color;

uint32_t offset(uint32_t x, uint32_t y)
{
    return x + y * DEBUG_TERMINAL_WIDTH;
}

void scroll()
{
    if (_cursor_y >= DEBUG_TERMINAL_HEIGHT)
    {
        unsigned int i;
        for (i = 0; i < (DEBUG_TERMINAL_HEIGHT - 1) * DEBUG_TERMINAL_WIDTH; ++i)
        {
            _VIDMEM[i] = _VIDMEM[i + DEBUG_TERMINAL_WIDTH];
        }

        for (; i < DEBUG_TERMINAL_HEIGHT * DEBUG_TERMINAL_WIDTH; ++i)
        {
            _VIDMEM[i] = (char_entry_t){' ', _text_color, _back_color};
        }

        _cursor_y = DEBUG_TERMINAL_HEIGHT - 1;
    }
}