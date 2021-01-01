/**
 * @file usb_descriptors.c
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

#include <usb/usb_descriptors.h>

#include <stdio.h>

void usb_print_device_desc(usb_device_desc_t *desc)
{
    printf(" USB: Version: %i.%i, VendorID: %#04x, Product: %#04x, Configs: %i\n",
           desc->usbVer >> 8,
           (desc->usbVer >> 4) & 0xF,
           desc->vendorId,
           desc->productId,
           desc->confCount);
}

void usb_print_conf_desc(usb_conf_desc_t *desc)
{
    printf("  Conf: totalLen: %i, intfCount: %i, confValue: %i, confStr: %i\n",
           desc->totalLen,
           desc->intfCount,
           desc->confValue,
           desc->confStr);
}

void usb_print_intf_desc(usb_intf_desc_t *desc)
{
    printf("  Intf: altSetting: %i, endpCount: %i, class: %i, subclass: %i, protocol: %i, str: %i\n",
           desc->altSetting,
           desc->endpCount,
           desc->intfClass,
           desc->intfSubClass,
           desc->intfProtocol,
           desc->intfStr);
}

void usb_print_endp_desc(usb_endp_desc_t *desc)
{
    printf("  Endp: addr: %#02x, attr: %i, maxPacketSize: %i, interval: %i\n",
           desc->addr,
           desc->attributes,
           desc->maxPacketSize,
           desc->interval);
}

void usb_print_hid_desc(usb_hid_desc_t *desc)
{
    printf("  HID: ver: %i.%i, country: %i, desccount: %i, descType: %i, descLen: %i\n",
           desc->hidVer >> 8,
           (desc->hidVer >> 8) & 0xFF,
           desc->countryCode,
           desc->descCount,
           desc->descType,
           desc->descLen);
}

void usb_print_hub_desc(usb_hub_desc_t *desc)
{
    printf("  Hub: portCount: %i, char: %#x, powerTime: %i, current: %i\n",
           desc->portCount,
           desc->chars,
           desc->portPowerTime,
           desc->current);
}

//=============================================================================
// End of file
//=============================================================================