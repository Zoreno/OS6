/**
 * @file pci_io.c
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-06-22
 *
 * @brief
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

#include <pci/pci_io.h>

#include <arch/arch.h>

#define MAKE_ADDR(id, reg) (0x80000000 | id | (reg & 0xFC))

#define PCI_CONFIG_ADDR 0xCF8
#define PCI_CONFIG_DATA 0xCFC

uint8_t pci_read_b(uint32_t id, uint32_t reg)
{
    uint32_t addr = MAKE_ADDR(id, reg);

    outportl(PCI_CONFIG_ADDR, addr);

    return inportb(PCI_CONFIG_DATA + (reg & 0x03));
}

uint16_t pci_read_w(uint32_t id, uint32_t reg)
{
    uint32_t addr = MAKE_ADDR(id, reg);

    outportl(PCI_CONFIG_ADDR, addr);

    return inportw(PCI_CONFIG_DATA + (reg & 0x02));
}

uint32_t pci_read_l(uint32_t id, uint32_t reg)
{
    uint32_t addr = MAKE_ADDR(id, reg);

    outportl(PCI_CONFIG_ADDR, addr);

    return inportl(PCI_CONFIG_DATA);
}

void pci_write_b(uint32_t id, uint32_t reg, uint8_t data)
{
    uint32_t addr = MAKE_ADDR(id, reg);

    outportl(PCI_CONFIG_ADDR, addr);

    outportb(PCI_CONFIG_DATA + (reg & 0x03), data);
}

void pci_write_w(uint32_t id, uint32_t reg, uint16_t data)
{
    uint32_t addr = MAKE_ADDR(id, reg);

    outportl(PCI_CONFIG_ADDR, addr);

    outportw(PCI_CONFIG_DATA + (reg & 0x02), data);
}

void pci_write_l(uint32_t id, uint32_t reg, uint32_t data)
{
    uint32_t addr = MAKE_ADDR(id, reg);

    outportl(PCI_CONFIG_ADDR, addr);

    outportl(PCI_CONFIG_DATA, data);
}

//=============================================================================
// End of file
//=============================================================================
