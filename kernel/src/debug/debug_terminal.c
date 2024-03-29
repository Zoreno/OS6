/**
 * @file debug_terminal.c
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

#include <debug/debug_terminal.h>

#include <stdio.h>
#include <string.h>

#include <drivers/keyboard_ps2.h>

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

static FILE file;

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

static char debug_terminal_getch()
{
    KEYCODE code = keyboard_getch();

    return (char)code;
}

void debug_terminal_initialize()
{
    _text_color = _default_text_color;
    _back_color = _default_back_color;

    debug_terminal_clear();

    strcpy(file.name, "debug_terminal");

    file.read = debug_terminal_getch;
    file.write = debug_terminal_putch;
    file.eof = 0;

    set_stdout(&file);
}

void debug_terminal_restore_default()
{
    _text_color = _default_text_color;
    _back_color = _default_back_color;
}

void debug_terminal_clear()
{
    for (uint32_t i = 0; i < (DEBUG_TERMINAL_HEIGHT * DEBUG_TERMINAL_WIDTH);
         ++i)
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

//=============================================================================
// End of file
//=============================================================================
