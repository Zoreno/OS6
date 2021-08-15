/**
 * @file pci.h
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-04-18
 *
 * @brief PCI device listing
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

#ifndef _PCI_H
#define _PCI_H

#include <pci/pci_device.h>

#include <stddef.h>
#include <stdint.h>

extern pci_device_list_t *device_list;

typedef struct _PciBAR
{
    union
    {
        void *address;
        uint16_t port;
    };

    uint64_t size;
    uint32_t flags;
} PciBAR_t;

#define PCI_DONT_CARE 0xFF

typedef struct _PciDriver_t
{
    uint16_t class;
    uint8_t progIntf;

    void (*init)(uint32_t id, PciDeviceInfo_t *deviceInfo);
} PciDriver_t;

void pciInit();

void pciWriteConfigField(uint32_t id, int field, size_t size, uint32_t value);
uint32_t pciReadConfigField(uint32_t id, int field, size_t size);

uint32_t pci_get_vga_lfb();

#endif

//=============================================================================
// End of file
//=============================================================================
