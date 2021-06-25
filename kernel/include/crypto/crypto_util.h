/**
 * @file crypto_util.h
 * @author Joakim Bertils
 * @version 0.1
 * @date 2021-05-05
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

#ifndef _CRYPTO_UTIL_H
#define _CRYPTO_UTIL_H

#include <stdint.h>

uint8_t rotate_left8(uint8_t a, uint8_t n);
uint16_t rotate_left16(uint16_t a, uint8_t n);
uint32_t rotate_left32(uint32_t a, uint8_t n);
uint64_t rotate_left64(uint64_t a, uint8_t n);

uint8_t rotate_right8(uint8_t a, uint8_t n);
uint16_t rotate_right16(uint16_t a, uint8_t n);
uint32_t rotate_right32(uint32_t a, uint8_t n);
uint64_t rotate_right64(uint64_t a, uint8_t n);

uint8_t shift_left8(uint8_t a, uint8_t n);
uint16_t shift_left16(uint16_t a, uint8_t n);
uint32_t shift_left32(uint32_t a, uint8_t n);
uint64_t shift_left64(uint64_t a, uint8_t n);

uint8_t shift_right8(uint8_t a, uint8_t n);
uint16_t shift_right16(uint16_t a, uint8_t n);
uint32_t shift_right32(uint32_t a, uint8_t n);
uint64_t shift_right64(uint64_t a, uint8_t n);

int _abs(int val);
int _sign(int val);

#endif

//=============================================================================
// End of file
//=============================================================================
