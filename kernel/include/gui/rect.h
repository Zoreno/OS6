#ifndef _RECT_H_
#define _RECT_H_

#include <gui/list.h>
#include <stdlib.h>

typedef struct _rect_t
{
    int top;
    int left;
    int bottom;
    int right;
} rect_t;

rect_t *rect_new(int top, int left, int bottom, int right);
gui_list_t *rect_split(rect_t *subject_rect, rect_t *cutting_rect);
rect_t *rect_intersect(rect_t *rect_a, rect_t *rect_b);

#endif // _RECT_H_