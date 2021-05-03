/**
 * @file terminal_color.c
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-06-18
 *
 * @brief Terminal color
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

#include <gui/terminal/terminal_color.h>

terminal_color_t terminal_color_get_from_8bit(uint8_t val)
{
    terminal_color_t color;

    // Val = 0 - 15
    if (val < 16)
    {
        switch (val)
        {
            // Black
        case 0:
            color.red = 0;
            color.green = 0;
            color.blue = 0;
            break;
            // Red
        case 1:
            color.red = 222;
            color.green = 56;
            color.blue = 43;
            break;
            // Green
        case 2:
            color.red = 57;
            color.green = 181;
            color.blue = 74;
            break;
            // Yellow
        case 3:
            color.red = 255;
            color.green = 199;
            color.blue = 6;
            break;
            // Blue
        case 4:
            color.red = 0;
            color.green = 111;
            color.blue = 184;
            break;
            // Magenta
        case 5:
            color.red = 118;
            color.green = 38;
            color.blue = 113;
            break;
            // Cyan
        case 6:
            color.red = 222;
            color.green = 56;
            color.blue = 43;
            break;
            // White
        case 7:
            color.red = 204;
            color.green = 204;
            color.blue = 204;
            break;
            // Bright Black
        case 8:
            color.red = 128;
            color.green = 128;
            color.blue = 128;
            break;
            // Bright Red
        case 9:
            color.red = 255;
            color.green = 0;
            color.blue = 0;
            break;
            // Bright Green
        case 10:
            color.red = 0;
            color.green = 255;
            color.blue = 0;
            break;
            // Bright Yellow
        case 11:
            color.red = 255;
            color.green = 255;
            color.blue = 0;
            break;
            // Bright Blue
        case 12:
            color.red = 0;
            color.green = 0;
            color.blue = 255;
            break;
            // Bright Magenta
        case 13:
            color.red = 255;
            color.green = 0;
            color.blue = 255;
            break;
            // Bright Cyan
        case 14:
            color.red = 0;
            color.green = 255;
            color.blue = 255;
            break;
            // Bright White
        case 15:
            color.red = 255;
            color.green = 255;
            color.blue = 255;
            break;
        default:
            // Should not happen
            break;
        }
    }
    // Val = 16 - 231
    else if (val < 232)
    {
        val -= 16;

        uint8_t r = val / 36;
        val -= 36 * r;
        uint8_t g = val / 6;
        val -= 6 * g;
        uint8_t b = val;

        color.red = 255 * r / 5;
        color.green = 255 * g / 5;
        color.blue = 255 * b / 5;
    }
    // Val = 232 - 255
    else
    {
        val -= 232;

        uint8_t grayscale = 255 * val / 24;

        color.red = grayscale;
        color.green = grayscale;
        color.blue = grayscale;
    }

    return color;
}

terminal_color_t terminal_color_get_from_4bit(uint8_t val)
{
    return terminal_color_get_from_8bit(val % 16);
}

//=============================================================================
// End of file
//=============================================================================
