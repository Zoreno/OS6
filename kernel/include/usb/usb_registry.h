/**
 * @file usb_registry.h
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-04-20
 *
 * @brief USB definitions for device registration
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

#ifndef _USB_REGISTRY_H
#define _USB_REGISTRY_H

#define USB_CLASS_INTF 0x00
#define USB_CLASS_AUDIO 0x01
#define USB_CLASS_COMM 0x02
#define USB_CLASS_HID 0x03
#define USB_CLASS_PHYSICAL 0x05
#define USB_CLASS_IMAGE 0x06
#define USB_CLASS_PRINTER 0x07
#define USB_CLASS_STORAGE 0x08
#define USB_CLASS_HUB 0x09
#define USB_CLASS_CDC_DATA 0x0a
#define USB_CLASS_SMART_CARD 0x0b
#define USB_CLASS_SECURITY 0x0d
#define USB_CLASS_VIDEO 0x0e
#define USB_CLASS_HEALTHCARE 0x0f
#define USB_CLASS_DIAGNOSTIC 0xdc
#define USB_CLASS_WIRELESS 0xe0
#define USB_CLASS_MISC 0xef
#define USB_CLASS_APP 0xfe
#define USB_CLASS_VENDOR 0xff

#define USB_SUBCLASS_BOOT 0x01

#define USB_PROTOCOL_KBD 0x01
#define USB_PROTOCOL_MOUSE 0x02

#endif

//=============================================================================
// End of file
//=============================================================================
