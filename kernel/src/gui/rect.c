/**
 * @file rect.c
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

#include <gui/rect.h>

#include <stdlib.h>

rect_t *rect_new(int top, int left, int bottom, int right)
{
    rect_t *rect;
    if (!(rect = (rect_t *)malloc(sizeof(rect_t))))
    {
        return rect;
    }

    rect->top = top;
    rect->left = left;
    rect->bottom = bottom;
    rect->right = right;

    return rect;
}

gui_list_t *rect_split(rect_t *subject_rect, rect_t *cutting_rect)
{
    gui_list_t *output_rects;

    if (!(output_rects = gui_list_new()))
        return output_rects;

    rect_t subject_copy;

    subject_copy.top = subject_rect->top;
    subject_copy.left = subject_rect->left;
    subject_copy.bottom = subject_rect->bottom;
    subject_copy.right = subject_rect->right;

    rect_t *temp_rect;

    if (cutting_rect->left > subject_copy.left &&
        cutting_rect->left <= subject_copy.right)
    {
        if (!(temp_rect = rect_new(
                  subject_copy.top,
                  subject_copy.left,
                  subject_copy.bottom,
                  cutting_rect->left - 1)))
        {
            free(output_rects);

            return (gui_list_t *)0;
        }

        gui_list_add(output_rects, temp_rect);

        subject_copy.left = cutting_rect->left;
    }

    if (cutting_rect->top > subject_copy.top &&
        cutting_rect->top <= subject_copy.bottom)
    {
        if (!(temp_rect = rect_new(
                  subject_copy.top,
                  subject_copy.left,
                  cutting_rect->top - 1,
                  subject_copy.right)))
        {
            while (output_rects->count)
            {
                temp_rect = gui_list_remove_at(output_rects, 0);

                free(temp_rect);
            }

            free(output_rects);

            return (gui_list_t *)0;
        }

        gui_list_add(output_rects, temp_rect);

        subject_copy.top = cutting_rect->top;
    }

    if (cutting_rect->right >= subject_copy.left &&
        cutting_rect->right < subject_copy.right)
    {
        if (!(temp_rect = rect_new(
                  subject_copy.top,
                  cutting_rect->right + 1,
                  subject_copy.bottom,
                  subject_copy.right)))
        {
            while (output_rects->count)
            {
                temp_rect = gui_list_remove_at(output_rects, 0);
                free(temp_rect);
            }

            free(output_rects);

            return (gui_list_t *)0;
        }

        gui_list_add(output_rects, temp_rect);

        subject_copy.right = cutting_rect->right;
    }

    if (cutting_rect->bottom >= subject_copy.top &&
        cutting_rect->bottom < subject_copy.bottom)
    {
        if (!(temp_rect = rect_new(
                  cutting_rect->bottom + 1,
                  subject_copy.left,
                  subject_copy.bottom,
                  subject_copy.right)))
        {
            while (output_rects->count)
            {
                temp_rect = gui_list_remove_at(output_rects, 0);
                free(temp_rect);
            }

            free(output_rects);

            return (gui_list_t *)0;
        }

        gui_list_add(output_rects, temp_rect);

        subject_copy.bottom = cutting_rect->bottom;
    }

    return output_rects;
}

rect_t *rect_intersect(rect_t *rect_a, rect_t *rect_b)
{
    rect_t *result_rect;

    if (!(rect_a->left <= rect_b->right &&
          rect_a->right >= rect_b->left &&
          rect_a->top <= rect_b->bottom &&
          rect_a->bottom >= rect_b->top))
    {
        return (rect_t *)0;
    }

    if (!(result_rect = rect_new(
              rect_a->top,
              rect_a->left,
              rect_a->bottom,
              rect_a->right)))
    {
        return (rect_t *)0;
    }

    if (rect_b->left >= result_rect->left &&
        rect_b->left <= result_rect->right)
    {
        result_rect->left = rect_b->left;
    }

    if (rect_b->top >= result_rect->top &&
        rect_b->top <= result_rect->bottom)
    {
        result_rect->top = rect_b->top;
    }

    if (rect_b->right >= result_rect->left &&
        rect_b->right <= result_rect->right)
    {
        result_rect->right = rect_b->right;
    }

    if (rect_b->bottom >= result_rect->top &&
        rect_b->bottom <= result_rect->bottom)
    {
        result_rect->bottom = rect_b->bottom;
    }

    return result_rect;
}