/**
 * @file mmio.c
 * @author Joakim Bertils
 * @version 0.1
 * @date 2021-06-27
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

#include <util/mmio.h>

void mmio_write8(uintptr_t address, uint8_t value)
{
    *((volatile uint8_t *)(address)) = value;
}

void mmio_write16(uintptr_t address, uint16_t value)
{
    *((volatile uint16_t *)(address)) = value;
}

void mmio_write32(uintptr_t address, uint32_t value)
{
    *((volatile uint32_t *)(address)) = value;
}

void mmio_write64(uintptr_t address, uint64_t value)
{
    *((volatile uint64_t *)(address)) = value;
}

uint8_t mmio_read8(uintptr_t address)
{
    return *((volatile uint8_t *)(address));
}

uint16_t mmio_read16(uintptr_t address)
{
    return *((volatile uint16_t *)(address));
}

uint32_t mmio_read32(uintptr_t address)
{
    return *((volatile uint32_t *)(address));
}

uint64_t mmio_read64(uintptr_t address)
{
    return *((volatile uint64_t *)(address));
}

//=============================================================================
// End of file
//=============================================================================
