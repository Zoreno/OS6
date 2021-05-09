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

uint8_t rol8(uint8_t a, uint8_t n);
uint16_t rol16(uint16_t a, uint8_t n);
uint32_t rol32(uint32_t a, uint8_t n);
uint64_t rol64(uint64_t a, uint8_t n);

uint8_t ror8(uint8_t a, uint8_t n);
uint16_t ror16(uint16_t a, uint8_t n);
uint32_t ror32(uint32_t a, uint8_t n);
uint64_t ror64(uint64_t a, uint8_t n);

uint8_t shl8(uint8_t a, uint8_t n);
uint16_t shl16(uint16_t a, uint8_t n);
uint32_t shl32(uint32_t a, uint8_t n);
uint64_t shl64(uint64_t a, uint8_t n);

uint8_t shr8(uint8_t a, uint8_t n);
uint16_t shr16(uint16_t a, uint8_t n);
uint32_t shr32(uint32_t a, uint8_t n);
uint64_t shr64(uint64_t a, uint8_t n);

int _abs(int val);
int _sign(int val);

#endif

//=============================================================================
// End of file
//=============================================================================
