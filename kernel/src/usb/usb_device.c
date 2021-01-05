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
#include <logging/logging.h>
#include <usb/usb_driver.h>

//=============================================================================
// Local variables
//=============================================================================

usb_device_t *_usb_device_list;
static uint32_t _next_usb_addr;

//=============================================================================
// Interface functions
//=============================================================================

usb_device_t *usb_get_device_list()
{
    return _usb_device_list;
}

usb_device_t *usb_dev_create()
{
    log_debug("[USB_DEVICE] Creating USB device");

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
        dev->max_packet_size = 0;
        dev->endp.toggle = 0;

        dev->hc_control = 0;
        dev->hc_intr = 0;
        dev->drvPoll = 0;

        _usb_device_list = dev;
    }

    return dev;
}

int usb_dev_init(usb_device_t *dev)
{
    log_debug("[USB_DEVICE] Initiating device...");

    usb_device_desc_t dev_desc;

    if (!usb_dev_request(dev,
                         RT_DEV_TO_HOST | RT_STANDARD | RT_DEV,
                         REQ_GET_DESC,
                         (USB_DESC_DEVICE << 8) | 0,
                         0,
                         8,
                         &dev_desc))
    {
        log_error("[USB_DEVICE] Failed to send dev request");
        return 0;
    }

    dev->max_packet_size = dev_desc.max_packet_size;

    uint32_t addr = ++_next_usb_addr;

    if (!usb_dev_request(dev,
                         RT_HOST_TO_DEV | RT_STANDARD | RT_DEV,
                         REQ_SET_ADDR,
                         addr,
                         0,
                         0,
                         0))
    {
        log_error("[USB_DEVICE] Failed to send dev request");
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
                         &dev_desc))
    {
        log_error("[USB_DEVICE] Failed to send dev request");
        return 0;
    }

    usb_print_device_desc(&dev_desc);

    uint16_t langs[USB_STRING_SIZE];
    if (!usb_dev_get_langs(dev, langs))
    {
        log_error("[USB_DEVICE] Failed to get lang ID");
        return 0;
    }

    uint32_t lang_id = langs[0];

    if (lang_id)
    {
        char product_string[USB_STRING_SIZE];
        char vendor_string[USB_STRING_SIZE];
        char serial_string[USB_STRING_SIZE];

        usb_dev_get_string(dev, product_string, lang_id, dev_desc.product_str);
        usb_dev_get_string(dev, vendor_string, lang_id, dev_desc.vendor_str);
        usb_dev_get_string(dev, serial_string, lang_id, dev_desc.serial_str);

        log_debug("[USB_DEVICE] Product: %s, vendor: %s, serial: %s",
                  product_string,
                  vendor_string,
                  serial_string);
    }

    uint8_t conf_buf[256];
    uint32_t picked_conf_value = 0;
    usb_intf_desc_t *picked_intf_desc = 0;
    usb_endp_desc_t *picked_endp_desc = 0;

    log_debug("[USB_DEVICE] conf_count: %i", dev_desc.conf_count);

    for (uint32_t conf_index = 0; conf_index < dev_desc.conf_count; ++conf_index)
    {
        if (!usb_dev_request(dev,
                             RT_DEV_TO_HOST | RT_STANDARD | RT_DEV,
                             REQ_GET_DESC,
                             (USB_DESC_CONF << 8) | conf_index,
                             0,
                             4,
                             conf_buf))
        {
            log_warn("[USB_DEVICE] Failed to send dev request");
            continue;
        }

        usb_conf_desc_t *conf_desc = (usb_conf_desc_t *)conf_buf;

        if (conf_desc->total_len > sizeof(conf_buf))
        {
            log_warn("[USB_DEVICE] Configuration buffer length %i greater than %d bytes",
                     conf_desc->total_len,
                     sizeof(conf_buf));

            continue;
        }

        if (!usb_dev_request(dev,
                             RT_DEV_TO_HOST | RT_STANDARD | RT_DEV,
                             REQ_GET_DESC,
                             (USB_DESC_CONF << 8) | conf_index,
                             0,
                             conf_desc->total_len,
                             conf_buf))
        {
            log_warn("[USB_DEVICE] Failed to send dev request");
            continue;
        }

        usb_print_conf_desc(conf_desc);

        if (!picked_conf_value)
        {
            picked_conf_value = conf_desc->conf_value;
        }

        uint8_t *data = conf_buf + conf_desc->len;
        uint8_t *end = conf_buf + conf_desc->total_len;

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

                if (!picked_intf_desc)
                {
                    picked_intf_desc = intf_desc;
                }
            }
            break;
            case USB_DESC_ENDP:
            {
                usb_endp_desc_t *endp_desc = (usb_endp_desc_t *)data;
                usb_print_endp_desc(endp_desc);

                if (!picked_endp_desc)
                {
                    picked_endp_desc = endp_desc;
                }
            }
            break;
            }

            data += len;
        }
    }

    if (picked_conf_value && picked_intf_desc && picked_endp_desc)
    {
        if (!usb_dev_request(dev,
                             RT_HOST_TO_DEV | RT_STANDARD | RT_DEV,
                             REQ_SET_CONF,
                             picked_conf_value,
                             0,
                             0,
                             0))
        {
            log_error("[USB_DEVICE] Failed to send dev request");
            return 0;
        }

        dev->intfDesc = *picked_intf_desc;
        dev->endp.desc = *picked_endp_desc;

        usb_driver_t *driver = usb_get_driver_table();

        while (driver->init)
        {
            if (driver->init(dev))
            {
                break;
            }

            ++driver;
        }

        if (!driver->init)
        {
            log_warn("[USB_DEVICE] Could not find driver for USB device");
        }
    }

    return 1;
}

int usb_dev_request(usb_device_t *dev,
                    uint32_t type,
                    uint32_t request,
                    uint32_t value,
                    uint32_t index,
                    uint32_t len,
                    void *data)
{
    log_debug("[USB_DEVICE] Creating device request");

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

    dev->hc_control(dev, &t);

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
                         1,
                         desc))
    {
        log_error("[USB_DEVICE] Failed to send dev request");
        return 0;
    }

    if (!usb_dev_request(dev,
                         RT_DEV_TO_HOST | RT_STANDARD | RT_DEV,
                         REQ_GET_DESC,
                         (USB_DESC_STRING << 8) | 0,
                         0,
                         desc->len,
                         desc))
    {
        log_error("[USB_DEVICE] Failed to send dev request");
        return 0;
    }

    uint8_t lang_len = (desc->len - 2) / 2;

    for (uint32_t i = 0; i < lang_len; ++i)
    {
        langs[i] = desc->str[i];
    }

    langs[lang_len] = 0;
    return 1;
}

int usb_dev_get_string(usb_device_t *dev, char *str, uint32_t langId, uint32_t str_index)
{
    str[0] = '\0';
    if (!str_index)
    {
        return 1;
    }

    uint8_t buf[256];
    usb_string_desc_t *desc = (usb_string_desc_t *)buf;

    // Get string length
    if (!usb_dev_request(dev,
                         RT_DEV_TO_HOST | RT_STANDARD | RT_DEV,
                         REQ_GET_DESC,
                         (USB_DESC_STRING << 8) | str_index,
                         langId,
                         1,
                         desc))
    {
        log_error("[USB_DEVICE] Failed to send dev request");
        return 0;
    }

    // Get string data
    if (!usb_dev_request(dev,
                         RT_DEV_TO_HOST | RT_STANDARD | RT_DEV,
                         REQ_GET_DESC,
                         (USB_DESC_STRING << 8) | str_index,
                         langId,
                         desc->len,
                         desc))
    {
        return 0;
    }

    // Dumb Unicode to ASCII conversion
    uint32_t str_len = (desc->len - 2) / 2;

    for (uint32_t i = 0; i < str_len; ++i)
    {
        str[i] = desc->str[i];
    }

    str[str_len] = '\0';

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