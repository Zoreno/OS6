/**
 * @file rect.h
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

#ifndef _RECT_H_
#define _RECT_H_

#include <gui/gui_list.h>
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