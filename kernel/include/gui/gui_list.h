/**
 * @file gui_list.h
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

#ifndef _LIST_H_
#define _LIST_H_

#include <gui/gui_listnode.h>

typedef struct _gui_list_t
{
    unsigned int count;
    gui_list_node_t *root_node;
} gui_list_t;

gui_list_t *gui_list_new();
int gui_list_add(gui_list_t *list, void *payload);
void *gui_list_get_at(gui_list_t *list, unsigned int index);
void *gui_list_remove_at(gui_list_t *list, unsigned int index);

#endif  // _LIST_H_

//=============================================================================
// End of file
//=============================================================================
