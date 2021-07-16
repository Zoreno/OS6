/**
 * @file e1000.c
 * @author Joakim Bertils
 * @version 0.1
 * @date 2021-07-12
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

#include <arch/arch.h>
#include <drivers/e1000.h>
#include <logging/logging.h>
#include <mm/phys_mem.h>
#include <mm/virt_mem.h>
#include <util/mmio.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//=============================================================================
// Definitions
//=============================================================================

#define REG_EEPROM 0x0014
#define REG_MAC 0x5400

#define RX_DESC_COUNT 32
#define TX_DESC_COUNT 8

typedef struct rx_descriptor
{
    volatile uint64_t addr;
    volatile uint16_t length;
    volatile uint16_t checksum;
    volatile uint8_t status;
    volatile uint8_t errors;
    volatile uint16_t special;
} __attribute__((packed)) rx_descriptor_t;

typedef struct tx_descriptor
{
    volatile uint64_t addr;
    volatile uint16_t length;
    volatile uint8_t cso;
    volatile uint8_t cmd;
    volatile uint8_t status;
    volatile uint8_t css;
    volatile uint16_t special;
} __attribute__((packed)) tx_descriptor_t;

typedef struct e1000_device
{
    uint64_t mem_base;
    int eeprom;
    uint8_t mac[6];

    rx_descriptor_t *rx;
    tx_descriptor_t *tx;
} e1000_device_t;

//=============================================================================
// Local data
//=============================================================================

static e1000_device_t *device;

//=============================================================================
// Private functions
//=============================================================================

static void write_command(e1000_device_t *device,
                          uint16_t address,
                          uint32_t value)
{
    ASSERT(device->mem_base != 0);

    log_info(
        "[E1000] Writing %#08x to %#04x", value, device->mem_base + address);
    mmio_write32(device->mem_base + address, value);
}

static uint32_t read_command(e1000_device_t *device, uint16_t address)
{
    ASSERT(device->mem_base != 0);

    uint32_t val = mmio_read32(device->mem_base + address);

    log_info("[E1000] Read %#08x from %#04x", val, device->mem_base + address);

    return val;
}

static void detect_eeprom(e1000_device_t *device)
{
    write_command(device, REG_EEPROM, 0x1);

    int eepromExists = 0;

    for (int i = 0; (i < 1000) && !eepromExists; ++i)
    {
        uint32_t val = read_command(device, REG_EEPROM);

        if (val & 0x10)
        {
            eepromExists = 1;
        }
    }

    device->eeprom = eepromExists;
}

static uint16_t read_eeprom(e1000_device_t *device, uint8_t addr)
{
    uint32_t tmp = 0;
    write_command(device, REG_EEPROM, 1 | ((uint32_t)(addr) << 8));

    while (!((tmp = read_command(device, REG_EEPROM)) & (1 << 4)))
        ;

    return (uint16_t)((tmp >> 16) & 0xFFFF);
}

static int read_mac(e1000_device_t *device)
{
    if (device->eeprom)
    {
        uint32_t t;

        for (int i = 0; i < 3; ++i)
        {
            t = read_eeprom(device, i);
            device->mac[2 * i + 0] = t & 0xFF;
            device->mac[2 * i + 1] = t >> 8;
        }
        return 1;
    }

    for (int i = 0; i < 6; ++i)
    {
        device->mac[i] = mmio_read8(device->mem_base + REG_MAC + i);
    }

    return 1;
}

static void alloc_rx()
{
}

//=============================================================================
// Interface functions
//=============================================================================

void e1000_init(uint32_t id, PciDeviceInfo_t *deviceInfo)
{
    log_info("[E1000] Initializing E1000 driver");

    device = malloc(sizeof(e1000_device_t));

    if (!device)
    {
        log_error("[E1000] Failed to allocate memory");
        return;
    }

    uint64_t bar0 = deviceInfo->type0.BaseAddresses[0];
    uint64_t barType = bar0 & 1;
    device->mem_base = bar0 & ~3;

    log_info("[E1000] Bar type: %s", barType == 0 ? "Memory" : "IO");

    // This will map 16 pages (64 kb) of memory mapped IO registers.
    // Too much?
    virt_mem_map_pages((void *)device->mem_base,
                       (void *)device->mem_base,
                       16,
                       VIRT_MEM_WRITABLE);

    detect_eeprom(device);

    log_info("[E1000] EEPROM: %i", device->eeprom);

    memset(device->mac, 0, sizeof(device->mac));

    read_mac(device);

    char buf[32];

    sprintf(buf,
            "%02x:%02x:%02x:%02x:%02x:%02x",
            device->mac[0],
            device->mac[1],
            device->mac[2],
            device->mac[3],
            device->mac[4],
            device->mac[5]);

    log_info("[E1000] MAC address: %s", buf);
}

//=============================================================================
// End of file
//=============================================================================
