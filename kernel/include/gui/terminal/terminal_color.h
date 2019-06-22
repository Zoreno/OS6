/**
 * @file terminal_color.h
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-06-18
 * 
 * @brief Terminal color representation
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

#ifndef _TERMINAL_COLOR_H
#define _TERMINAL_COLOR_H

#include <stdint.h>

typedef struct _terminal_color
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} terminal_color_t;

terminal_color_t terminal_color_get_from_8bit(uint8_t val);
terminal_color_t terminal_color_get_from_4bit(uint8_t val);

#endif

//=============================================================================
// End of file
//=============================================================================