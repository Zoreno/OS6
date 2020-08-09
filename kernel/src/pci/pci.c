/**
 * @file pci.c
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-04-20
 * 
 * @brief PCI bus enumeration and device driver initialization
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

#include <pci/pci.h>

#include <arch/arch.h>
#include <pci/pci_io.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <usb/usb_uhci.h>
#include <usb/usb_ehci.h>

pci_device_list_t *device_list = 0;

#define SERIAL_PCI_OUTPUT 1

#define PCI_MAKE_ID(bus, dev, func) (((bus) << 16) | ((dev) << 11) | ((func) << 8))

// PCI Configuration Registers
#define PCI_CONFIG_VENDOR_ID 0x00
#define PCI_CONFIG_HEADER_TYPE 0x0e

void pciCheckDevice(uint32_t bus, uint32_t dev, uint32_t func);

void pciCheckDevice(uint32_t bus, uint32_t dev, uint32_t func)
{
    uint32_t id = PCI_MAKE_ID(bus, dev, func);

    // No existing device
    if (pci_read_w(id, PCI_CONFIG_VENDOR_ID) == 0xFFFF)
    {
        return;
    }

    PciDeviceInfo_t *devInfo = NULL;

    // Make sure there is a list
    if (device_list == 0)
    {
        // Allocate head
        device_list = malloc(sizeof(pci_device_list_t));
        device_list->next = 0; // Since we are not guaranteed empty pages.

        // read info into the first node.
        pci_read_device_info(id, &device_list->dev_info);

        devInfo = &device_list->dev_info;
    }
    else
    {
        // get next node
        pci_device_list_t *cur_node = device_list;

        //  Loop to the end
        while (cur_node->next != 0)
        {
            cur_node = cur_node->next;
        }

        // Allocate a new node
        cur_node->next = malloc(sizeof(pci_device_list_t));

        cur_node = cur_node->next;

        cur_node->next = 0;

        // read info into the current node node.
        pci_read_device_info(id, &cur_node->dev_info);

        devInfo = &cur_node->dev_info;
    }

    if (!devInfo)
    {
        return;
    }

    const PciDriver_t _pci_driver_table[] =
        {
            {usb_uhci_init},
            {usb_ehci_init},
            {0},
        };

    const PciDriver_t *driver = _pci_driver_table;

    while (*driver->init)
    {
        driver->init(id, devInfo);
        ++driver;
    }
}

uint32_t pci_get_vga_lfb()
{
    pci_device_list_t *cur_node = device_list;

    while (cur_node != NULL)
    {
        PciDeviceInfo_t *dev = &cur_node->dev_info;

        if (dev->vendorID == 0x1234 && dev->deviceID == 0x1111)
        {
            // BAR0 contains the physical address of the video memory
            return dev->type0.BaseAddresses[0];
        }

        cur_node = cur_node->next;
    }

    return 0;
}

void pciInit()
{
    for (uint32_t bus = 0; bus < 256; ++bus)
    {
        for (uint32_t dev = 0; dev < 32; ++dev)
        {
            uint32_t baseID = PCI_MAKE_ID(bus, dev, 0);

            uint8_t headerType = pci_read_b(baseID, PCI_CONFIG_HEADER_TYPE);

            uint32_t funcCount = headerType & 0x80 ? 8 : 1;

            for (uint8_t func = 0; func < funcCount; ++func)
            {
                pciCheckDevice(bus, dev, func);
            }
        }
    }

    // Send out all read info to serial output for reference

#if SERIAL_PCI_OUTPUT
    printf("\n==========================================\n");
    printf("============ PCI Device List =============\n");
    printf("==========================================\n\n");

    pci_device_list_t *cur_node = device_list;

    while (cur_node != 0)
    {

        PciDeviceInfo_t *dev = &cur_node->dev_info;

        printf("Device Name: %s\n", pci_device_name(dev->vendorID, dev->deviceID));
        printf("Class Name: %s\n", pci_class_name(dev->classCode, dev->subClass, dev->progIntf));

        printf("Vendor ID: %#x\n", dev->vendorID);
        printf("Device ID: %#x\n", dev->deviceID);
        printf("Command: %#x\n", dev->command_w); // TODO Print bit for bit
        printf("Status: %#x\n", dev->status_w);   // TODO Print bit for bit
        printf("Revision ID: %#x\n", dev->revisionID);
        printf("Prog IF: %#x\n", dev->progIntf);
        printf("Subclass: %#x\n", dev->subClass);
        printf("Class Code: %#x\n", dev->classCode);
        printf("Cache line size: %#x\n", dev->cacheLineSize);
        printf("Latency Timer: %#x\n", dev->latencyTimer);
        printf("Header Type: %#x\n", dev->headerType);
        printf("BIST: %#x\n", dev->BIST);
        printf("\n");

        switch (dev->headerType & 0x7F)
        {
        case 0:
            for (int i = 0; i < 6; ++i)
            {
                printf("BAR[%i]: %#x\n", i, dev->type0.BaseAddresses[i]);
            }

            printf("CIS: %#x\n", dev->type0.CIS);
            printf("SubVendorID: %#x\n", dev->type0.SubVendorID);
            printf("SubSystemID: %#x\n", dev->type0.SubSystemID);
            printf("ROMBaseAddress: %#x\n", dev->type0.ROMBaseAddress);
            printf("CapabilitiesPtr: %#x\n", dev->type0.CapabilitiesPtr);
            printf("InterruptLine: %#x\n", dev->type0.InterruptLine);
            printf("InterruptPin: %#x\n", dev->type0.InterruptPin);
            printf("MinimumGrant: %#x\n", dev->type0.MinimumGrant);
            printf("MaximumLatency: %#x\n", dev->type0.MaximumLatency);
            break;
        case 1:
            break;
        case 2:
            break;
        default:
            printf("Invalid header type\n");
            break;
        }

        printf("\n==========================================\n\n");

        cur_node = cur_node->next; // Advance
    }

#endif
}

//=============================================================================
// End of file
//=============================================================================