/**
 * @file vt_100.h
 * @author Joakim Bertils
 * @version 0.1
 * @date 2021-01-25
 *
 * @brief Functions for handling VT-100 capable terminals
 *
 * @copyright Copyright (C) 2021,
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

#ifndef _VT_100_H
#define _VT_100_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VT_100_ESCAPE 27

#define VT_100_UP 1
#define VT_100_DOWN 2
#define VT_100_LEFT 3
#define VT_100_RIGHT 4

int vt_100_handle_bracket();
int vt_100_handle_command();
void vt_100_erase_line();

void vt_100_set_foreground_color(int color);
void vt_100_set_background_color(int color);
void vt_100_restore_style();
void vt_100_set_bold();
void vt_100_set_dim();
void vt_100_set_cursor_location(int line, int column);
void vt_100_get_cursor_location(int *line, int *column);

#endif

//=============================================================================
// End of file
//=============================================================================
