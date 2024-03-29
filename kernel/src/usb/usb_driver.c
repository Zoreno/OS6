/**
 * @file usb_driver.c
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-04-20
 *
 * @brief USB Driver interface
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

#include <usb/usb_driver.h>
#include <usb/usb_hub.h>
#include <usb/usb_keyboard.h>

//=============================================================================
// Local variables
//=============================================================================

static usb_driver_t usb_driver_table[] = {
    {usb_hub_init}, {usb_keyboard_init}, {0}};

//=============================================================================
// Interface functions
//=============================================================================

usb_driver_t *usb_get_driver_table()
{
    return usb_driver_table;
}

//=============================================================================
// End of file
//=============================================================================
