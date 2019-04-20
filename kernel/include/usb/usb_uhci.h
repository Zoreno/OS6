/**
 * @file usb_uhci.h
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-04-20
 * 
 * @brief USB UHCI Host Controller driver
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

#ifndef _USB_UHCI_H
#define _USB_UHCI_H

#include <pci/pci_device.h>

void usb_uhci_init(uint32_t id, PciDeviceInfo_t *devInfo);

#endif

//=============================================================================
// End of file
//=============================================================================