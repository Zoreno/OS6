/**
 * @file debug_terminal.h
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

#ifndef DEBUG_TERMINAL_H
#define DEBUG_TERMINAL_H

#include <stdint.h>

#define DEBUG_TERMINAL_WIDTH 80U
#define DEBUG_TERMINAL_HEIGHT 25U

typedef enum VGA_COLOR
{
    VGA_COLOR_BLACK = 0,
    VGA_COLOR_BLUE = 1,
    VGA_COLOR_GREEN = 2,
    VGA_COLOR_CYAN = 3,
    VGA_COLOR_RED = 4,
    VGA_COLOR_MAGENTA = 5,
    VGA_COLOR_BROWN = 6,
    VGA_COLOR_LIGHT_GREY = 7,
    VGA_COLOR_DARK_GREY = 8,
    VGA_COLOR_LIGHT_BLUE = 9,
    VGA_COLOR_LIGHT_GREEN = 0xA,
    VGA_COLOR_LIGHT_CYAN = 0xB,
    VGA_COLOR_LIGHT_RED = 0xC,
    VGA_COLOR_LIGHT_MAGENTA = 0xD,
    VGA_COLOR_LIGHT_BROWN = 0xE,
    VGA_COLOR_WHITE = 0xF,
} vga_color_t;

void debug_terminal_initialize();
void debug_terminal_restore_default();
void debug_terminal_clear();
void debug_terminal_print(const char *str);
void debug_terminal_putch(char c);
vga_color_t debug_terminal_set_text_color(vga_color_t new_color);
vga_color_t debug_terminal_set_back_color(vga_color_t new_color);
void debug_terminal_set_cursor(uint32_t x, uint32_t y);
void debug_terminal_get_cursor(uint32_t *x, uint32_t *y);

#endif

//=============================================================================
// End of file
//=============================================================================
