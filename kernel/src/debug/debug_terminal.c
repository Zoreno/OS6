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

void debug_terminal_initialize()
{
    _text_color = _default_text_color;
    _back_color = _default_back_color;

    debug_terminal_clear();
}

void debug_terminal_restore_default()
{
    _text_color = _default_text_color;
    _back_color = _default_back_color;
}

void debug_terminal_clear()
{
    for (uint32_t i = 0; i < (DEBUG_TERMINAL_HEIGHT * DEBUG_TERMINAL_WIDTH); ++i)
    {
        _VIDMEM[i] = (char_entry_t){' ', _text_color, _back_color};
    }

    _cursor_x = 0;
    _cursor_y = 0;
}

void debug_terminal_print(const char *str)
{
    while (*str)
    {
        debug_terminal_putch(*(str++));
    }
}

void debug_terminal_putch(char c)
{
    if (c == 0)
    {
        return;
    }

    if (c == '\n')
    {
        _cursor_x = 0;
        ++_cursor_y;
        return;
    }

    if (_cursor_x >= DEBUG_TERMINAL_WIDTH)
    {
        _cursor_x = 0;
        ++_cursor_y;
        return;
    }

    scroll();

    _VIDMEM[offset(_cursor_x++, _cursor_y)] =
        (char_entry_t){c, _text_color, _back_color};
}

vga_color_t debug_terminal_set_text_color(vga_color_t new_color)
{
    vga_color_t old_color = _text_color;
    _text_color = new_color;
    return old_color;
}

vga_color_t debug_terminal_set_back_color(vga_color_t new_color)
{
    vga_color_t old_color = _back_color;
    _back_color = new_color;
    return old_color;
}

void debug_terminal_set_cursor(uint32_t x, uint32_t y)
{
    _cursor_x = x;
    _cursor_y = y;
}

void debug_terminal_get_cursor(uint32_t *x, uint32_t *y)
{
    *x = _cursor_x;
    *y = _cursor_y;
}