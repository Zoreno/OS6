/**
 * @file endian.c
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

#include <util/endian.h>

uint16_t swapInt16(uint16_t value)
{
    return SWAPINT16(value);
}

uint32_t swapInt32(uint32_t value)
{
    return SWAPINT32(value);
}

uint64_t swapInt64(uint64_t value)
{
    return SWAPINT64(value);
}

uint8_t reverseInt4(uint8_t value)
{
    value = ((value & 0x0C) >> 2) | ((value & 0x03) << 2);
    value = ((value & 0x0A) >> 1) | ((value & 0x05) << 1);

    return value;
}

uint8_t reverseInt8(uint8_t value)
{
    value = ((value & 0xF0) >> 4) | ((value & 0x0F) << 4);
    value = ((value & 0xCC) >> 2) | ((value & 0x33) << 2);
    value = ((value & 0xAA) >> 1) | ((value & 0x55) << 1);

    return value;
}

uint16_t reverseInt16(uint16_t value)
{
    value = ((value & 0xFF00) >> 8) | ((value & 0x00FF) << 8);
    value = ((value & 0xF0F0) >> 4) | ((value & 0x0F0F) << 4);
    value = ((value & 0xCCCC) >> 2) | ((value & 0x3333) << 2);
    value = ((value & 0xAAAA) >> 1) | ((value & 0x5555) << 1);

    return value;
}

uint32_t reverseInt32(uint32_t value)
{
    value = ((value & 0xFFFF0000UL) >> 16) | ((value & 0x0000FFFFUL) << 16);
    value = ((value & 0xFF00FF00UL) >> 8) | ((value & 0x00FF00FFUL) << 8);
    value = ((value & 0xF0F0F0F0UL) >> 4) | ((value & 0x0F0F0F0FUL) << 4);
    value = ((value & 0xCCCCCCCCUL) >> 2) | ((value & 0x33333333UL) << 2);
    value = ((value & 0xAAAAAAAAUL) >> 1) | ((value & 0x55555555UL) << 1);

    return value;
}

uint64_t reverseInt64(uint64_t value)
{
    value = ((value & 0xFFFFFFFF00000000ULL) >> 32) |
            ((value & 0x00000000FFFFFFFFULL) << 32);
    value = ((value & 0xFFFF0000FFFF0000ULL) >> 16) |
            ((value & 0x0000FFFF0000FFFFULL) << 16);
    value = ((value & 0xFF00FF00FF00FF00ULL) >> 8) |
            ((value & 0x00FF00FF00FF00FFULL) << 8);
    value = ((value & 0xF0F0F0F0F0F0F0F0ULL) >> 4) |
            ((value & 0x0F0F0F0F0F0F0F0FULL) << 4);
    value = ((value & 0xCCCCCCCCCCCCCCCCULL) >> 2) |
            ((value & 0x3333333333333333ULL) << 2);
    value = ((value & 0xAAAAAAAAAAAAAAAAULL) >> 1) |
            ((value & 0x5555555555555555ULL) << 1);

    return value;
}

//=============================================================================
// End of file
//=============================================================================
