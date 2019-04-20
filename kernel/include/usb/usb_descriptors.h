/**
 * @file usb_descriptors.h
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-04-20
 * 
 * @brief USB device descriptor types
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

#ifndef _USB_DESCRIPTORS_H
#define _USB_DESCRIPTORS_H

#include <stdint.h>

//=============================================================================
// USB Base Descriptor types
//=============================================================================

#define USB_DESC_DEVICE 0x01
#define USB_DESC_CONF 0x02
#define USB_DESC_STRING 0x03
#define USB_DESC_INTF 0x04
#define USB_DESC_ENDP 0x05

//=============================================================================
// USB HID Descriptor types
//=============================================================================

#define USB_DESC_HID 0x21
#define USB_DESC_REPORT 0x22
#define USB_DESC_PHYSICAL 0x23

//=============================================================================
// USB HUB Descriptor types
//=============================================================================

#define USB_DESC_HUB 0x29

//=============================================================================
// USB Device Descriptor
//=============================================================================

typedef struct _usb_device_desc_t
{
    uint8_t len;
    uint8_t type;
    uint16_t usbVer;
    uint8_t devClass;
    uint8_t devSubClass;
    uint8_t devProtocol;
    uint8_t maxPacketSize;
    uint16_t vendorId;
    uint16_t productId;
    uint16_t deviceVer;
    uint8_t vendorStr;
    uint8_t productStr;
    uint8_t serialStr;
    uint8_t confCount;

} __attribute__((packed)) usb_device_desc_t;

//=============================================================================
// USB Configuration Descriptor
//=============================================================================

typedef struct _usb_conf_desc_t
{
    uint8_t len;
    uint8_t type;
    uint16_t totalLen;
    uint8_t intfCount;
    uint8_t confValue;
    uint8_t confStr;
    uint8_t attributes;
    uint8_t maxPower;

} __attribute__((packed)) usb_conf_desc_t;

//=============================================================================
// USB String Descriptor
//=============================================================================

typedef struct _usb_string_desc_t
{
    uint8_t len;
    uint8_t type;
    uint16_t str[];

} __attribute__((packed)) usb_string_desc_t;

//=============================================================================
// USB Interface Descriptor
//=============================================================================

typedef struct _usb_intf_desc_t
{
    uint8_t len;
    uint8_t type;
    uint8_t intfIndex;
    uint8_t altSetting;
    uint8_t endpCount;
    uint8_t intfClass;
    uint8_t intfSubClass;
    uint8_t intfProtocol;
    uint8_t intfStr;

} __attribute__((packed)) usb_intf_desc_t;

//=============================================================================
// USB Endpoint Descriptor
//=============================================================================

typedef struct _usb_endp_desc_t
{
    uint8_t len;
    uint8_t type;
    uint8_t addr;
    uint8_t attributes;
    uint16_t maxPacketSize;
    uint8_t interval;

} __attribute__((packed)) usb_endp_desc_t;

//=============================================================================
// USB HID Descriptor
//=============================================================================

typedef struct _usb_hid_desc_t
{
    uint8_t len;
    uint8_t type;
    uint16_t hidVer;
    uint8_t countryCode;
    uint8_t descCount;
    uint8_t descType;
    uint16_t descLen;

} __attribute__((packed)) usb_hid_desc_t;

//=============================================================================
// USB Hub Descriptor
//=============================================================================

typedef struct usb_hub_desc_t
{
    uint8_t len;
    uint8_t type;
    uint8_t portCount;
    uint16_t chars;
    uint8_t portPowerTime;
    uint8_t current;

} __attribute__((packed)) usb_hub_desc_t;

//=============================================================================
// Hub characteristics
//=============================================================================

#define HUB_POWER_MASK 0x03 // Logical Power Switching Mode
#define HUB_POWER_GLOBAL 0x00
#define HUB_POWER_INDIVIDUAL 0x01
#define HUB_COMPOUND 0x04        // Part of a Compound Device
#define HUB_CURRENT_MASK 0x18    // Over-current Protection Mode
#define HUB_TT_TTI_MASK 0x60     // TT Think Time
#define HUB_PORT_INDICATORS 0x80 // Port Indicators

//=============================================================================
// Functions
//=============================================================================

void usb_print_device_desc(usb_device_desc_t *desc);
void usb_print_conf_desc(usb_conf_desc_t *desc);
void usb_print_intf_desc(usb_intf_desc_t *desc);
void usb_print_endp_desc(usb_endp_desc_t *desc);

void usb_print_hid_desc(usb_hid_desc_t *desc);
void usb_print_hub_desc(usb_hub_desc_t *desc);

#endif

//=============================================================================
// End of file
//=============================================================================