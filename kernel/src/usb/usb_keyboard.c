/**
 * @file usb_keyboard.c
 * @author Joakim Bertils
 * @version 0.1
 * @date 2021-01-03
 *
 * @brief
 *
 * @copyright Copyright (C) 2021,
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

#include <usb/usb_keyboard.h>

#include <stdint.h>
#include <string.h>

#include <logging/logging.h>
#include <mm/phys_mem.h>
#include <mm/virt_mem.h>
#include <usb/usb_device.h>
#include <usb/usb_registry.h>

//=============================================================================
// Definitions
//=============================================================================

typedef struct
{
    usb_transfer_t data_transfer;
    uint8_t data[8];
    uint8_t last_data[8];
} usb_keyboard_t;

//=============================================================================
// Local functions
//=============================================================================

void handle_key(uint16_t key, uint8_t value)
{
    if (value == 0)
    {
        log_debug("Released key: %i", key);
    }
    else
    {
        log_debug("Pressed key: %i", key);
    }
}

static void usb_process_keyboard(usb_keyboard_t *keyboard)
{
    uint8_t *data = keyboard->data;
    int error = 0;

    // Handle modifier keys
    uint8_t mod_delta = data[0] ^ keyboard->last_data[0];

    for (uint8_t i = 0; i < 8; ++i)
    {
        uint8_t mask = 1 << i;

        if (mod_delta & mask)
        {
            handle_key(mask << 8, data[0] & mask);
        }
    }

    // Handle release keys
    for (uint8_t i = 0; i < 8; ++i)
    {
        uint8_t usage = keyboard->last_data[i];
        if (usage)
        {
            if (!memchr(data + 2, usage, 6))
            {
                handle_key(usage, 0);
            }
        }
    }

    // Handle press keys
    for (uint8_t i = 2; i < 8; ++i)
    {
        uint8_t usage = data[i];

        if (usage >= 4)
        {
            if (!memchr(keyboard->last_data + 2, usage, 6))
            {
                handle_key(usage, 1);
            }
        }
        else if (usage > 0)
        {
            error = 1;
        }
    }

    // Handle error
    if (!error)
    {
        memcpy(keyboard->last_data, data, 8);
    }
}

static void usb_keyboard_poll(usb_device_t *dev)
{
    usb_keyboard_t *kbd = dev->drv;
    usb_transfer_t *t = &kbd->data_transfer;

    if (t->complete)
    {
        if (t->success)
        {
            usb_process_keyboard(kbd);
        }

        t->complete = 0;
        dev->hc_intr(dev, t);
    }
}

// TODO: This is wasting a lot of physical memory
// Rewrite to use a common pool for allocating
// identity mapped memory.
static void *alloc_physmem(size_t size)
{
    void *p = (void *)phys_mem_alloc_block();

    if (!p)
    {
        log_error("[USB_KEYBOARD] Error allocating physical memory");
        return p;
    }

    virt_mem_map_page(p, p, 0);

    return p;
}

//=============================================================================
// Interface functions
//=============================================================================

int usb_keyboard_init(usb_device_t *dev)
{
    if (dev->intfDesc.intf_class == USB_CLASS_HID &&
        dev->intfDesc.intf_sub_class == USB_SUBCLASS_BOOT &&
        dev->intfDesc.intf_protocol == USB_PROTOCOL_KBD)
    {
        log_info("[USB_KEYBOARD] Initializing");

        usb_keyboard_t *kbd = alloc_physmem(sizeof(usb_keyboard_t));
        memset(kbd->last_data, 0, sizeof(kbd->last_data));

        dev->drv = kbd;
        dev->drvPoll = usb_keyboard_poll;

        uint32_t intf_index = dev->intfDesc.intf_index;

        if (!usb_dev_request(dev,
                             RT_HOST_TO_DEV | RT_CLASS | RT_INTF,
                             REQ_SET_IDLE,
                             0,
                             intf_index,
                             0,
                             0))
        {
            log_error("[USB_KEYBOARD] Failed to set idle mode");
            return 0;
        }
        else
        {
            log_debug("[USB_KEYBOARD] Idle mode set!");
        }

        usb_transfer_t *t = &kbd->data_transfer;
        t->endp = &dev->endp;
        t->req = 0;
        t->data = kbd->data;
        t->len = 8;
        t->complete = 0;
        t->success = 0;

        dev->hc_intr(dev, t);

        return 1;
    }

    return 0;
}

//=============================================================================
// End of file
//=============================================================================
