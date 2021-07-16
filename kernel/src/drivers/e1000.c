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
#include <mm/virt_mem.h>
#include <util/mmio.h>

#include <assert.h>
#include <stdio.h>
#include <string.h>

//=============================================================================
// Definitions
//=============================================================================

#define REG_EEPROM 0x0014
#define REG_MAC 0x5400

//=============================================================================
// Local data
//=============================================================================

static uint64_t memBase = 0;

//=============================================================================
// Private functions
//=============================================================================

static void write_command(uint16_t address, uint32_t value)
{
    ASSERT(memBase != 0);

    log_info("[E1000] Writing %#08x to %#04x", value, memBase + address);
    mmio_write32(memBase + address, value);
}

static uint32_t read_command(uint16_t address)
{
    ASSERT(memBase != 0);

    uint32_t val = mmio_read32(memBase + address);

    log_info("[E1000] Read %#08x from %#04x", val, memBase + address);

    return val;
}

static int detect_eeprom()
{
    write_command(REG_EEPROM, 0x1);

    int eepromExists = 0;

    for (int i = 0; (i < 1000) && !eepromExists; ++i)
    {
        uint32_t val = read_command(REG_EEPROM);

        if (val & 0x10)
        {
            eepromExists = 1;
        }
    }

    return eepromExists;
}

static uint16_t read_eeprom(uint8_t addr)
{
    uint32_t tmp = 0;
    write_command(REG_EEPROM, 1 | ((uint32_t)(addr) << 8));

    while (!((tmp = read_command(REG_EEPROM)) & (1 << 4)))
        ;

    return (uint16_t)((tmp >> 16) & 0xFFFF);
}

static int read_mac(uint8_t *mac, int eeprom)
{
    if (eeprom)
    {
        uint32_t t;

        for (int i = 0; i < 3; ++i)
        {
            t = read_eeprom(i);
            mac[2 * i + 0] = t & 0xFF;
            mac[2 * i + 1] = t >> 8;
        }
        return 1;
    }

    for (int i = 0; i < 6; ++i)
    {
        mac[i] = mmio_read8(memBase + REG_MAC + i);
    }

    return 1;
}

//=============================================================================
// Interface functions
//=============================================================================

void e1000_init(uint32_t id, PciDeviceInfo_t *deviceInfo)
{
    log_info("[E1000] Initializing E1000 driver");

    uint64_t bar0 = deviceInfo->type0.BaseAddresses[0];
    uint64_t barType = bar0 & 1;
    memBase = bar0 & ~3;

    log_info("[E1000] Bar type: %s", barType == 0 ? "Memory" : "IO");

    virt_mem_map_pages((void *)bar0, (void *)bar0, 16, VIRT_MEM_WRITABLE);

    int eeprom = detect_eeprom();

    log_info("[E1000] EEPROM: %i", eeprom);

    uint8_t mac[6];
    memset(mac, 0, sizeof(mac));

    read_mac(mac, eeprom);

    char buf[32];

    sprintf(buf,
            "%02x:%02x:%02x:%02x:%02x:%02x",
            mac[0],
            mac[1],
            mac[2],
            mac[3],
            mac[4],
            mac[5]);

    log_info("[E1000] MAC address: %s", buf);
}

//=============================================================================
// End of file
//=============================================================================
