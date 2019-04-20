/**
 * @file usb_device_request.h
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-04-20
 * 
 * @brief Usb device request struct
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

#ifndef _USB_DEVICE_REQUEST_H
#define _USB_DEVICE_REQUEST_H

#include <stdint.h>

//=============================================================================
// USB request type
//=============================================================================

#define RT_TRANSFER_MASK 0x80
#define RT_DEV_TO_HOST 0x80
#define RT_HOST_TO_DEV 0x00

#define RT_TYPE_MASK 0x60
#define RT_STANDARD 0x00
#define RT_CLASS 0x20
#define RT_VENDOR 0x40

#define RT_RECIPIENT_MASK 0x1f
#define RT_DEV 0x00
#define RT_INTF 0x01
#define RT_ENDP 0x02
#define RT_OTHER 0x03

//=============================================================================
// USB Device requests
//=============================================================================

#define REQ_GET_STATUS 0x00
#define REQ_CLEAR_FEATURE 0x01
#define REQ_SET_FEATURE 0x03
#define REQ_SET_ADDR 0x05
#define REQ_GET_DESC 0x06
#define REQ_SET_DESC 0x07
#define REQ_GET_CONF 0x08
#define REQ_SET_CONF 0x09
#define REQ_GET_INTF 0x0a
#define REQ_SET_INTF 0x0b
#define REQ_SYNC_FRAME 0x0c

//=============================================================================
// USB HUB Class requests
//=============================================================================

#define REQ_CLEAR_TT_BUFFER 0x08
#define REQ_RESET_TT 0x09
#define REQ_GET_TT_STATE 0x0a
#define REQ_STOP_TT 0x0b

//=============================================================================
// USB HID Interface requests
//=============================================================================

#define REQ_GET_REPORT 0x01
#define REQ_GET_IDLE 0x02
#define REQ_GET_PROTOCOL 0x03
#define REQ_SET_REPORT 0x09
#define REQ_SET_IDLE 0x0a
#define REQ_SET_PROTOCOL 0x0b

//=============================================================================
// USB Standard feature selectors
//=============================================================================

#define F_DEVICE_REMOTE_WAKEUP 1 // Device
#define F_ENDPOINT_HALT 2        // Endpoint
#define F_TEST_MODE 3            // Device

//=============================================================================
// USB HUB Feature Selectors
//=============================================================================

#define F_C_HUB_LOCAL_POWER 0    // Hub
#define F_C_HUB_OVER_CURRENT 1   // Hub
#define F_PORT_CONNECTION 0      // Port
#define F_PORT_ENABLE 1          // Port
#define F_PORT_SUSPEND 2         // Port
#define F_PORT_OVER_CURRENT 3    // Port
#define F_PORT_RESET 4           // Port
#define F_PORT_POWER 8           // Port
#define F_PORT_LOW_SPEED 9       // Port
#define F_C_PORT_CONNECTION 16   // Port
#define F_C_PORT_ENABLE 17       // Port
#define F_C_PORT_SUSPEND 18      // Port
#define F_C_PORT_OVER_CURRENT 19 // Port
#define F_C_PORT_RESET 20        // Port
#define F_PORT_TEST 21           // Port
#define F_PORT_INDICATOR 22      // Port

//=============================================================================
// USB Device request
//=============================================================================

typedef struct _usb_dev_req_t
{
    uint8_t type;
    uint8_t req;
    uint16_t value;
    uint16_t index;
    uint16_t len;

} __attribute__((packed)) usb_dev_req_t;

#endif

//=============================================================================
// End of fileUSB HUB Descriptor types
//=============================================================================