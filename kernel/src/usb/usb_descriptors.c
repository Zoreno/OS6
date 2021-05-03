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
    printf(
        " USB: Version: %i.%i, VendorID: %#04x, Product: %#04x, Configs: %i\n",
        desc->usb_ver >> 8,
        (desc->usb_ver >> 4) & 0xF,
        desc->vendor_id,
        desc->product_id,
        desc->conf_count);
}

void usb_print_conf_desc(usb_conf_desc_t *desc)
{
    printf(
        "  Conf: total_len: %i, intf_count: %i, conf_value: %i, conf_str: %i\n",
        desc->total_len,
        desc->intf_count,
        desc->conf_value,
        desc->conf_str);
}

void usb_print_intf_desc(usb_intf_desc_t *desc)
{
    printf(
        "  Intf: alt_setting: %i, endp_count: %i, class: %i, subclass: %i, "
        "protocol: %i, str: %i\n",
        desc->alt_setting,
        desc->endp_count,
        desc->intf_class,
        desc->intf_sub_class,
        desc->intf_protocol,
        desc->intf_str);
}

void usb_print_endp_desc(usb_endp_desc_t *desc)
{
    printf("  Endp: addr: %#02x, attr: %i, max_packet_size: %i, interval: %i\n",
           desc->addr,
           desc->attributes,
           desc->max_packet_size,
           desc->interval);
}

void usb_print_hid_desc(usb_hid_desc_t *desc)
{
    printf(
        "  HID: ver: %i.%i, country: %i, desccount: %i, desc_type: %i, "
        "desc_len: %i\n",
        desc->hid_ver >> 8,
        (desc->hid_ver >> 8) & 0xFF,
        desc->country_code,
        desc->desc_count,
        desc->desc_type,
        desc->desc_len);
}

void usb_print_hub_desc(usb_hub_desc_t *desc)
{
    printf(
        "  Hub: port_count: %i, char: %#x, port_power_time: %i, current: %i\n",
        desc->port_count,
        desc->chars,
        desc->port_power_time,
        desc->current);
}

//=============================================================================
// End of file
//=============================================================================
