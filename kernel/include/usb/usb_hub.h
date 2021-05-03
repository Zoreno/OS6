/**
 * @file usb_hub.h
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-04-20
 *
 * @brief USB hub initialization routine
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

#ifndef _USB_HUB_H
#define _USB_HUB_H

#include <usb/usb_device.h>

//=============================================================================
// Functions
//=============================================================================

/**
 * @brief Initializes an USB hub
 *
 * @param device Device pointer
 *
 * @return int 1 on success
 */
int usb_hub_init(usb_device_t *device);

#endif

//=============================================================================
// End of file
//=============================================================================
