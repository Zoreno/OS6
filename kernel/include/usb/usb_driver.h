/**
 * @file usb_driver.h
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-04-20
 *
 * @brief USB driver interface
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

#ifndef _USB_DRIVER_H
#define _USB_DRIVER_H

#include <usb/usb_device.h>

typedef struct _usb_driver_t
{
    int (*init)(usb_device_t *device);
} usb_driver_t;

usb_driver_t *usb_get_driver_table();

#endif

//=============================================================================
// End of file
//=============================================================================
