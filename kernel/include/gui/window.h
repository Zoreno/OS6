#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <gui/context.h>
#include <gui/list.h>
#include <gui/rect.h>

#include <stdint.h>

#define COLOR_SET 0

#if COLOR_SET == 0

#define WIN_BGCOLOR 0xFFBBBBBB
#define WIN_TITLECOLOR 0xFFBE9270
#define WIN_TITLECOLOR_INACTIVE 0xFF908080
#define WIN_TEXTCOLOR 0xFFFFE0E0
#define WIN_TEXTCOLOR_INACTIVE 0xFFFFBBBB
#define WIN_BORDERCOLOR 0xFF000000

#elif COLOR_SET == 1

#define WIN_BGCOLOR 0xFFBC96E6
#define WIN_TITLECOLOR 0xFFC96480
#define WIN_TITLECOLOR_INACTIVE 0xFF55286F
#define WIN_TEXTCOLOR 0xFFFFE0E0
#define WIN_TEXTCOLOR_INACTIVE 0xFFFFBBBB
#define WIN_BORDERCOLOR 0xFF000000

#elif COLOR_SET == 2

#define WIN_BGCOLOR 0xFF6E9075
#define WIN_TITLECOLOR 0xFF81F0E5
#define WIN_TITLECOLOR_INACTIVE 0xFF78C091
#define WIN_TEXTCOLOR 0xFFFFE0E0
#define WIN_TEXTCOLOR_INACTIVE 0xFFFFBBBB
#define WIN_BORDERCOLOR 0xFF000000

#elif COLOR_SET == 3

#define WIN_BGCOLOR 0xFFFFFFFF
#define WIN_TITLECOLOR 0xFF2CC3FF
#define WIN_TITLECOLOR_INACTIVE 0xFF186B8C
#define WIN_TEXTCOLOR 0xFFFFE0E0
#define WIN_TEXTCOLOR_INACTIVE 0xFFFFBBBB
#define WIN_BORDERCOLOR 0xFF000000

#endif

#define WIN_TITLEHEIGHT 31
#define WIN_BORDERWIDTH 3

#define WIN_NODECORATION 0x1
#define WIN_MINIMIZED 0x2
#define WIN_SHOULD_CLOSE 0x4
#define WIN_NO_DRAG 0x8
#define WIN_NO_RESIZE 0x10
#define WIN_FLOATING 0x20
#define WIN_MAXIMIZED 0x40

#define WIN_DEFAULT_MIN_WIDTH 72
#define WIN_DEFAULT_MIN_HEIGHT 16
#define WIN_DEFAULT_MAX_WIDTH 4096
#define WIN_DEFAULT_MAX_HEIGHT 4096

struct Window_struct;

typedef void (*WindowPaintHandler)(struct Window_struct *);
typedef void (*WindowMousedownHandler)(struct Window_struct *, int, int);
typedef void (*WindowKeyHandler)(struct Window_struct *, int, int, int);
typedef void (*WindowTickHandler)(struct Window_struct *, int);

typedef struct Window_struct
{
    // This window's parent

    struct Window_struct *parent;

    // Window state

    int16_t x;
    int16_t y;
    uint16_t width;
    uint16_t height;
    uint16_t inner_width;  // The width of the inner renderable area.
    uint16_t inner_height; // The height of the inner renderable area.
    uint16_t flags;
    Context *context;
    gui_list_t *children;
    uint8_t last_button_state;
    int last_tick;

    char *title;

    uint32_t index;

    int16_t last_x;       // For returning from maximized state.
    int16_t last_y;       // For returning from maximized state.
    uint16_t last_width;  // For returning from maximized state.
    uint16_t last_height; // For returning from maximized state.

    // Settings

    uint16_t min_width;
    uint16_t min_height;

    uint16_t max_width;
    uint16_t max_height;

    // Subwindow handling

    struct Window_struct *drag_child;
    struct Window_struct *active_child;

    int dragging;
    int resizing;
    int16_t drag_off_x;
    int16_t drag_off_y;

    // Callbacks

    WindowPaintHandler paint_function;
    WindowMousedownHandler mousedown_function;
    WindowKeyHandler key_function;
    WindowTickHandler tick_function;

} Window;

Window *Window_new(
    unsigned int x,
    unsigned int y,
    unsigned int width,
    unsigned int height,
    uint16_t flags,
    Context *context,
    uint32_t index);

int Window_init(
    Window *window,
    int16_t x,
    int16_t y,
    uint16_t width,
    uint16_t height,
    uint16_t flags,
    Context *context,
    uint32_t index);

void Window_apply_bound_clipping(
    Window *window,
    int in_recursion,
    gui_list_t *dirty_regions);

void Window_draw_border(
    Window *window);

void Window_paint(
    Window *window,
    gui_list_t *dirty_regions,
    uint8_t paint_children);

void Window_process_mouse(
    Window *window,
    uint16_t mouse_x,
    uint16_t mouse_y,
    uint8_t mouse_buttons);

void Window_process_keyboard(
    Window *window,
    int key,
    int mods,
    int action);

void Window_process_tick(
    Window *window,
    int ticks);

gui_list_t *Window_get_windows_above(
    Window *parent,
    Window *child);

gui_list_t *Window_get_windows_below(
    Window *parent,
    Window *child);

Window *Window_create_window(
    Window *window,
    int16_t x,
    int16_t y,
    uint16_t width,
    uint16_t height,
    uint16_t flags);

void Window_insert_child(
    Window *window,
    Window *child);

void Window_raise(
    Window *window,
    uint8_t do_draw);

void Window_move(
    Window *window,
    int new_x,
    int new_y);

void Window_resize(
    Window *window,
    int new_width,
    int new_height);

void Window_invalidate(
    Window *window,
    int top,
    int left,
    int bottom,
    int right);

void Window_set_title(
    Window *window,
    char *new_title);

void Window_append_title(
    Window *window,
    char *add_c);

void Window_get_title(
    Window *window,
    char *buf);

void Window_minimize(
    Window *window);

void Window_restore(
    Window *window);

int Window_minimized(
    Window *window);

void Window_remove(
    Window *window);

void Window_close(
    Window *window);

int Window_should_close(
    Window *window);

void Window_request_close(
    Window *window);

int Window_is_focused(
    Window *window);

float Window_get_aspect_ratio(
    Window *window);

void Window_maximize(
    Window *window);

void Window_unmaximize(
    Window *window);

#endif // _WINDOW_H_