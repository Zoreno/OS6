/**
 * @file gui_listnode.c
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

#include <gui/gui_listnode.h>

#include <stdint.h>
#include <stdlib.h>

gui_list_node_t *gui_list_node_new(void *payload)
{
    gui_list_node_t *list_node;

    if (!(list_node = (gui_list_node_t *)malloc(sizeof(gui_list_node_t))))
        return list_node;

    list_node->prev = (gui_list_node_t *)0;
    list_node->next = (gui_list_node_t *)0;
    list_node->payload = payload;

    return list_node;
}

//=============================================================================
// End of file
//=============================================================================
