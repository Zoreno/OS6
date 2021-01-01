/**
 * @file usb_device.c
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

#include <usb/usb_device.h>

#include <stdio.h>
#include <stdlib.h>

#include <arch/arch.h>
#include <debug/backtrace.h>
#include <usb/usb_driver.h>

usb_device_t *_usb_device_list;

static uint32_t _next_usb_addr;

usb_device_t *usb_get_device_list()
{
    return _usb_device_list;
}

usb_device_t *usb_dev_create()
{
    usb_device_t *dev = malloc(sizeof(usb_device_t));

    if (dev)
    {
        dev->parent = 0;
        dev->next = _usb_device_list;
        dev->hc = 0;
        dev->drv = 0;

        dev->port = 0;
        dev->speed = 0;
        dev->addr = 0;
        dev->maxPacketSize = 0;
        dev->endp.toggle = 0;

        dev->hcControl = 0;
        dev->hcIntr = 0;
        dev->drvPoll = 0;

        _usb_device_list = dev;
    }

    return dev;
}

int usb_dev_init(usb_device_t *dev)
{
    usb_device_desc_t devDesc;

    if (!usb_dev_request(dev,
                         RT_DEV_TO_HOST | RT_STANDARD | RT_DEV,
                         REQ_GET_DESC,
                         (USB_DESC_DEVICE << 8) | 0,
                         0,
                         8,
                         &devDesc))
    {
        backtrace();
        return 0;
    }

    dev->maxPacketSize = devDesc.maxPacketSize;

    uint32_t addr = ++_next_usb_addr;

    if (!usb_dev_request(dev,
                         RT_HOST_TO_DEV | RT_STANDARD | RT_DEV,
                         REQ_SET_ADDR,
                         addr,
                         0,
                         0,
                         0))
    {
        backtrace();
        return 0;
    }

    dev->addr = addr;

    mdelay(2);

    if (!usb_dev_request(dev,
                         RT_DEV_TO_HOST | RT_STANDARD | RT_DEV,
                         REQ_GET_DESC,
                         (USB_DESC_DEVICE << 8) | 0,
                         0,
                         sizeof(usb_device_desc_t),
                         &devDesc))
    {
        backtrace();
        return 0;
    }

    usb_print_device_desc(&devDesc);

    uint16_t langs[USB_STRING_SIZE];
    usb_dev_get_langs(dev, langs);

    uint32_t langID = langs[0];

    if (langID)
    {
        char productString[USB_STRING_SIZE];
        char vendorString[USB_STRING_SIZE];
        char serialString[USB_STRING_SIZE];

        usb_dev_get_string(dev, productString, langID, devDesc.productStr);
        usb_dev_get_string(dev, vendorString, langID, devDesc.vendorStr);
        usb_dev_get_string(dev, serialString, langID, devDesc.serialStr);

        printf(" Product: %s, vendor: %s, serial: %s\n",
               productString,
               vendorString,
               serialString);
    }

    uint8_t confBuf[256];
    uint32_t pickedConfValue = 0;
    usb_intf_desc_t *pickedIntfDesc = 0;
    usb_endp_desc_t *pickedEndpDesc = 0;

    for (uint32_t confIndex = 0; confIndex < devDesc.confCount; ++confIndex)
    {
        if (!usb_dev_request(dev,
                             RT_DEV_TO_HOST | RT_STANDARD | RT_DEV,
                             REQ_GET_DESC,
                             (USB_DESC_CONF << 8) | confIndex,
                             0,
                             4, confBuf))
        {
            continue;
        }

        usb_conf_desc_t *confDesc = (usb_conf_desc_t *)confBuf;

        if (confDesc->totalLen > sizeof(confBuf))
        {
            printf("  Configuration buffer length %i greater than %d bytes\n",
                   confDesc->totalLen, sizeof(confBuf));

            continue;
        }

        if (!usb_dev_request(dev,
                             RT_DEV_TO_HOST | RT_STANDARD | RT_DEV,
                             REQ_GET_DESC,
                             (USB_DESC_CONF << 8) | confIndex,
                             0,
                             confDesc->totalLen,
                             confBuf))
        {
            continue;
        }

        usb_print_conf_desc(confDesc);

        if (!pickedConfValue)
        {
            pickedConfValue = confDesc->confValue;
        }

        uint8_t *data = confBuf + confDesc->len;
        uint8_t *end = confBuf + confDesc->totalLen;

        while (data < end)
        {
            uint8_t len = data[0];
            uint8_t type = data[1];

            switch (type)
            {
            case USB_DESC_INTF:
            {
                usb_intf_desc_t *intf_desc = (usb_intf_desc_t *)data;
                usb_print_intf_desc(intf_desc);

                if (!pickedIntfDesc)
                {
                    pickedIntfDesc = intf_desc;
                }
            }
            break;
            case USB_DESC_ENDP:
            {
                usb_endp_desc_t *endp_desc = (usb_endp_desc_t *)data;
                usb_print_endp_desc(endp_desc);

                if (!pickedEndpDesc)
                {
                    pickedEndpDesc = endp_desc;
                }
            }
            break;
            }

            data += len;
        }
    }

    if (pickedConfValue && pickedIntfDesc && pickedEndpDesc)
    {
        if (!usb_dev_request(dev,
                             RT_HOST_TO_DEV | RT_STANDARD | RT_DEV,
                             REQ_SET_CONF,
                             pickedConfValue,
                             0,
                             0,
                             0))
        {
            backtrace();
            return 0;
        }

        dev->intfDesc = *pickedIntfDesc;
        dev->endp.desc = *pickedEndpDesc;

        usb_driver_t *driver = usb_get_driver_table();

        while (driver->init)
        {
            if (driver->init(dev))
            {
                break;
            }

            ++driver;
        }
    }

    backtrace();
    return 1;
}

int usb_dev_request(usb_device_t *dev,
                    uint32_t type, uint32_t request,
                    uint32_t value, uint32_t index,
                    uint32_t len, void *data)
{
    usb_dev_req_t req;

    req.type = type;
    req.req = request;
    req.value = value;
    req.index = index;
    req.len = len;

    usb_transfer_t t;
    t.endp = 0;
    t.req = &req;
    t.data = data;
    t.len = len;
    t.complete = 0;
    t.success = 0;

    dev->hcControl(dev, &t);

    return t.success;
}

int usb_dev_get_langs(usb_device_t *dev, uint16_t *langs)
{
    langs[0] = 0;

    uint8_t buf[256];

    usb_string_desc_t *desc = (usb_string_desc_t *)buf;

    if (!usb_dev_request(dev,
                         RT_DEV_TO_HOST | RT_STANDARD | RT_DEV,
                         REQ_GET_DESC,
                         (USB_DESC_STRING << 8) | 0,
                         0,
                         desc->len,
                         desc))
    {
        return 0;
    }

    uint8_t langLen = (desc->len - 2) / 2;

    for (uint32_t i = 0; i < langLen; ++i)
    {
        langs[i] = desc->str[i];
    }

    langs[langLen] = 0;
    return 1;
}

int usb_dev_get_string(usb_device_t *dev, char *str, uint32_t langId, uint32_t strIndex)
{
    str[0] = '\0';
    if (!strIndex)
    {
        return 1;
    }

    uint8_t buf[256];
    usb_string_desc_t *desc = (usb_string_desc_t *)buf;

    // Get string length
    if (!usb_dev_request(dev,
                         RT_DEV_TO_HOST | RT_STANDARD | RT_DEV,
                         REQ_GET_DESC,
                         (USB_DESC_STRING << 8) | strIndex,
                         langId,
                         1,
                         desc))
    {
        return 0;
    }

    // Get string data
    if (!usb_dev_request(dev,
                         RT_DEV_TO_HOST | RT_STANDARD | RT_DEV,
                         REQ_GET_DESC,
                         (USB_DESC_STRING << 8) | strIndex,
                         langId,
                         desc->len,
                         desc))
    {
        return 0;
    }

    // Dumb Unicode to ASCII conversion
    uint32_t strLen = (desc->len - 2) / 2;

    for (uint32_t i = 0; i < strLen; ++i)
    {
        str[i] = desc->str[i];
    }

    str[strLen] = '\0';

    return 1;
}

int usb_dev_clear_halt(usb_device_t *dev)
{
    return usb_dev_request(dev,
                           RT_DEV_TO_HOST | RT_STANDARD | RT_ENDP,
                           REQ_CLEAR_FEATURE,
                           F_ENDPOINT_HALT,
                           dev->endp.desc.addr & 0xF,
                           0,
                           0);
}

//=============================================================================
// End of file
//=============================================================================