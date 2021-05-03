/**
 * @file usb.c
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-04-20
 *
 * @brief Top level USB routines
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

#include <usb/usb.h>
#include <usb/usb_controller.h>
#include <usb/usb_device.h>

void usb_poll()
{
    for (usb_controller_t *c = usb_get_controller_list(); c; c = c->next)
    {
        if (c->poll)
        {
            c->poll(c);
        }
    }

    for (usb_device_t *dev = usb_get_device_list(); dev; dev = dev->next)
    {
        if (dev->drvPoll)
        {
            dev->drvPoll(dev);
        }
    }
}

//=============================================================================
// End of file
//=============================================================================
