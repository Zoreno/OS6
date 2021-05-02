/**
 * @file usb_keyboard.h
 * @author Joakim Bertils
 * @version 0.1
 * @date 2021-01-03
 *
 * @brief
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

#ifndef _USB_KEYBOARD_H
#define _USB_KEYBOARD_H

#include <usb/usb_device.h>

int usb_keyboard_init(usb_device_t *dev);

#endif

//=============================================================================
// End of file
//=============================================================================
