/**
 * @file crypto_util.c
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

#include <crypto/crypto_util.h>

uint8_t rotate_left8(uint8_t a, uint8_t n)
{
    return (a << n) | (a >> (8 - n));
}

uint16_t rotate_left16(uint16_t a, uint8_t n)
{
    return (a << n) | (a >> (16 - n));
}

uint32_t rotate_left32(uint32_t a, uint8_t n)
{
    return (a << n) | (a >> (32 - n));
}

uint64_t rotate_left64(uint64_t a, uint8_t n)
{
    return (a << n) | (a >> (64 - n));
}

uint8_t rotate_right8(uint8_t a, uint8_t n)
{
    return (a >> n) | (a << (8 - n));
}

uint16_t rotate_right16(uint16_t a, uint8_t n)
{
    return (a >> n) | (a << (16 - n));
}

uint32_t rotate_right32(uint32_t a, uint8_t n)
{
    return (a >> n) | (a << (32 - n));
}

uint64_t rotate_right64(uint64_t a, uint8_t n)
{
    return (a >> n) | (a << (64 - n));
}

uint8_t shift_left8(uint8_t a, uint8_t n)
{
    return a << n;
}

uint16_t shift_left16(uint16_t a, uint8_t n)
{
    return a << n;
}

uint32_t shift_left32(uint32_t a, uint8_t n)
{
    return a << n;
}

uint64_t shift_left64(uint64_t a, uint8_t n)
{
    return a << n;
}

uint8_t shift_right8(uint8_t a, uint8_t n)
{
    return a >> n;
}

uint16_t shift_right16(uint16_t a, uint8_t n)
{
    return a >> n;
}

uint32_t shift_right32(uint32_t a, uint8_t n)
{
    return a >> n;
}

uint64_t shift_right64(uint64_t a, uint8_t n)
{
    return a >> n;
}

int _abs(int val)
{
    return val >= 0 ? val : -val;
}

int _sign(int val)
{
    return val >= 0 ? 1 : -1;
}

//=============================================================================
// End of file
//=============================================================================
