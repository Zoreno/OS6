/**
 * @file vbe.h
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-04-18
 *
 * @brief VGA Bochs Extension driver
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

#ifndef _VBE_H
#define _VBE_H

#include <stdint.h>

/**
 * @brief Struct representing a pixel on the screen
 *
 *
 */
typedef struct _vbe_pixel_t
{
    /**
     * Blue color channel
     *
     *
     */
    uint8_t blue;

    /**
     * Green color channel
     *
     *
     */
    uint8_t green;

    /**
     * Red color channel
     *
     *
     */
    uint8_t red;

    /**
     * Reserved padding byte for now.
     *
     *
     */
    uint8_t reserved;
} __attribute__((packed)) vbe_pixel_t;

void vbe_bochs_set_gfx(uint16_t width, uint16_t height, uint16_t bpp);
void vbe_clear_screen(vbe_pixel_t pixel);
void vbe_fill_rect(
    vbe_pixel_t pixel, int32_t x, int32_t y, uint32_t width, uint32_t height);
void vbe_put_pixel(vbe_pixel_t pixel, int32_t x, int32_t y);
void vbe_put_char(vbe_pixel_t pixel, int32_t x, int32_t y, char c);
void vbe_print_string(vbe_pixel_t pixel, int32_t x, int32_t y, char *str);
uint16_t vbe_get_width();
uint16_t vbe_get_height();
uint16_t vbe_get_bpp();
vbe_pixel_t *vbe_get_buffer();

#endif

//=============================================================================
// End of file
//=============================================================================
