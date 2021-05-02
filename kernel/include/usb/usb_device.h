/**
 * @file usb_device.h
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

#ifndef _USB_DEVICE_H
#define _USB_DEVICE_H

#include <usb/usb_descriptors.h>
#include <usb/usb_device_request.h>

#include <stdint.h>

//=============================================================================
// USB Limits
//=============================================================================

#define USB_STRING_SIZE 127

//=============================================================================
// USB Speeds
//=============================================================================

#define USB_FULL_SPEED 0x00
#define USB_LOW_SPEED 0x01
#define USB_HIGH_SPEED 0x02

//=============================================================================
// USB Endpoint
//=============================================================================

typedef struct _usb_endpoint_t
{
    usb_endp_desc_t desc;
    uint32_t toggle;

} usb_endpoint_t;

//=============================================================================
// USB Transfer
//=============================================================================

typedef struct _usb_transfer_t
{
    usb_endpoint_t *endp;
    usb_dev_req_t *req;
    void *data;
    uint32_t len;

    // TODO: These should be in a bitfield to save some space
    int complete;
    int success;

} usb_transfer_t;

//=============================================================================
// USB Device
//=============================================================================

typedef struct _usb_device_t
{
    struct _usb_device_t *parent;
    struct _usb_device_t *next;
    void *hc;
    void *drv;

    uint32_t port;
    uint32_t speed;
    uint32_t addr;
    uint32_t max_packet_size;

    usb_endpoint_t endp;

    usb_intf_desc_t intfDesc;

    void (*hc_control)(struct _usb_device_t *dev, usb_transfer_t *t);
    void (*hc_intr)(struct _usb_device_t *dev, usb_transfer_t *t);

    void (*drvPoll)(struct _usb_device_t *dev);

} usb_device_t;

//=============================================================================
// USB Functions
//=============================================================================

usb_device_t *usb_get_device_list();

usb_device_t *usb_dev_create();

int usb_dev_init(usb_device_t *dev);

int usb_dev_request(usb_device_t *dev,
                    uint32_t type,
                    uint32_t request,
                    uint32_t value,
                    uint32_t index,
                    uint32_t len,
                    void *data);

int usb_dev_get_langs(usb_device_t *dev, uint16_t *langs);

int usb_dev_get_string(usb_device_t *dev,
                       char *str,
                       uint32_t langId,
                       uint32_t strIndex);

int usb_dev_clear_halt(usb_device_t *dev);

#endif

//=============================================================================
// End of file
//=============================================================================
