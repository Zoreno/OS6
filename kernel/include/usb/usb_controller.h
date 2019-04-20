/**
 * @file usb_controller.h
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-04-20
 * 
 * @brief USB controller interface
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

#ifndef _USB_CONTROLLER_H
#define _USB_CONTROLLER_H

//=============================================================================
// Types
//=============================================================================

typedef struct _usb_controller_t
{
    struct _usb_controller_t *next;
    void *hc;

    void (*poll)(struct _usb_controller_t *controller);
} usb_controller_t;

//=============================================================================
// Functions
//=============================================================================

usb_controller_t *usb_get_controller_list();

void usb_set_controller_list(usb_controller_t *new_controller_list);

#endif

//=============================================================================
// End of file
//=============================================================================