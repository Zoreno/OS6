#ifndef DEBUG_TERMINAL_H
#define DEBUG_TERMINAL_H

#define DEBUG_TERMINAL_WIDTH 80U
#define DEBUG_TERMINAL_HEIGHT 25U

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

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
