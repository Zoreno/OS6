/**
 * @file usb_hub.c
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-04-20
 *
 * @brief USB hub initialization and probing
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

#include <usb/usb_hub.h>

#include <stdio.h>
#include <stdlib.h>

#include <arch/arch.h>
#include <logging/logging.h>
#include <usb/usb_registry.h>

#define PORT_CONNECTION (1 << 0)    // Current Connect Status
#define PORT_ENABLE (1 << 1)        // Port Enabled
#define PORT_SUSPEND (1 << 2)       // Suspend
#define PORT_OVER_CURRENT (1 << 3)  // Over-current
#define PORT_RESET (1 << 4)         // Port Reset
#define PORT_POWER (1 << 8)         // Port Power
#define PORT_SPEED_MASK (3 << 9)    // Port Speed
#define PORT_SPEED_SHIFT 9
#define PORT_TEST (1 << 11)                 // Port Test Control
#define PORT_INDICATOR (1 << 12)            // Port Indicator Control
#define PORT_CONNECTION_CHANGE (1 << 16)    // Connect Status Change
#define PORT_ENABLE_CHANGE (1 << 17)        // Port Enable Change
#define PORT_OVER_CURRENT_CHANGE (1 << 19)  // Over-current Change

typedef struct _usb_hub_t
{
    usb_device_t *dev;
    usb_hub_desc_t desc;
} usb_hub_t;

static uint32_t usb_hub_reset_port(usb_hub_t *hub, uint32_t port)
{
    usb_device_t *dev = hub->dev;

    if (!usb_dev_request(dev,
                         RT_HOST_TO_DEV | RT_CLASS | RT_OTHER,
                         REQ_SET_FEATURE,
                         F_PORT_RESET,
                         port + 1,
                         0,
                         0))
    {
        log_error("[USB_HUB] Failed to send dev request");
        return 0;
    }

    uint32_t status = 0;

    for (uint32_t i = 0; i < 10; ++i)
    {
        mdelay(10);

        if (!usb_dev_request(dev,
                             RT_DEV_TO_HOST | RT_CLASS | RT_OTHER,
                             REQ_GET_STATUS,
                             0,
                             port + 1,
                             sizeof(status),
                             &status))
        {
            log_error("[USB_HUB] Failed to send dev request");
            return 0;
        }

        if (~status & PORT_CONNECTION)
        {
            break;
        }

        if (status & PORT_ENABLE)
        {
            break;
        }
    }

    return status;
}

static void usb_hub_probe(usb_hub_t *hub)
{
    usb_device_t *dev = hub->dev;

    uint32_t port_count = hub->desc.port_count;

    if ((hub->desc.chars & HUB_POWER_MASK) == HUB_POWER_INDIVIDUAL)
    {
        for (uint32_t port = 0; port < port_count; ++port)
        {
            if (!usb_dev_request(dev,
                                 RT_HOST_TO_DEV | RT_CLASS | RT_OTHER,
                                 REQ_SET_FEATURE,
                                 F_PORT_POWER,
                                 port + 1,
                                 0,
                                 0))
            {
                log_error("[USB_HUB] Failed to send dev request");
                return;
            }
        }

        mdelay(hub->desc.port_power_time * 2);
    }

    for (uint32_t port = 0; port < port_count; ++port)
    {
        uint32_t status = usb_hub_reset_port(hub, port);

        if (status & PORT_ENABLE)
        {
            uint32_t speed = (status & PORT_SPEED_MASK) >> PORT_SPEED_SHIFT;

            usb_device_t *dev = usb_dev_create();

            if (dev)
            {
                dev->parent = hub->dev;
                dev->hc = hub->dev->hc;
                dev->port = port;
                dev->speed = speed;
                dev->max_packet_size = 8;

                dev->hc_control = hub->dev->hc_control;
                dev->hc_intr = hub->dev->hc_intr;

                if (!usb_dev_init(dev))
                {
                    log_error("[USB_HUB] Failed to init usb device");
                }
            }
        }
    }
}

static void usb_hub_poll(usb_device_t *dev)
{
}

int usb_hub_init(usb_device_t *dev)
{
    if (dev->intfDesc.intf_class == USB_CLASS_HUB)
    {
        printf("Initializing Hub\n");

        usb_hub_desc_t desc;

        if (!usb_dev_request(dev,
                             RT_DEV_TO_HOST | RT_CLASS | RT_DEV,
                             REQ_GET_DESC,
                             (USB_DESC_HUB << 8) | 0,
                             0,
                             sizeof(usb_hub_desc_t),
                             &desc))
        {
            log_error("[USB_HUB] Failed to send dev request");
            return 0;
        }

        usb_print_hub_desc(&desc);

        usb_hub_t *hub = malloc(sizeof(usb_hub_t));

        hub->dev = dev;
        hub->desc = desc;

        dev->drv = hub;
        dev->drvPoll = usb_hub_poll;

        usb_hub_probe(hub);

        return 1;
    }

    return 0;
}

//=============================================================================
// End of file
//=============================================================================
