/**
 * @file terminal_mode.c
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-06-17
 *
 * @brief Pixel mode terminal for JOS
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

#include <arch/arch.h>
#include <drivers/vbe.h>
#include <gui/terminal/terminal.h>
#include <gui/terminal_mode.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//=============================================================================
// Local variables
//=============================================================================

//=============================================================================
// External Interface
//=============================================================================

static terminal_t *_terminal;

void terminal_init()
{
    _terminal = terminal_create();

    while (1)
    {
        cli();
        terminal_redraw(_terminal);
        sti();

        mdelay(50);
    }
}

//=============================================================================
// End of file
//=============================================================================
