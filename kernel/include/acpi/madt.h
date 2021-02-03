/**
 * @file madt.h
 * @author Joakim Bertils
 * @version 0.1
 * @date 2021-02-01
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

#ifndef _ACPI_MADT_H
#define _ACPI_MADT_H

#include <stdint.h>

#include "acpi_header.h"

typedef struct 
{
    acpi_header_t header;
    uint32_t local_apic_addr;
    uint32_t flags;
} __attribute__((packed)) madt_t;

#define APIC_TYPE_LOCAL_APIC 0
#define APIC_TYPE_IO_APIC 1
#define APIC_TYPE_INTERRUPT_OVERRIDE 2
#define APIC_TYPE_NMI 4
#define APIC_TYPE_LAPIC_ADDR_OVERRIDE 5

typedef struct
{
    uint8_t entry_type;
    uint8_t record_length;
} __attribute__((packed)) madt_entry_header_t;

typedef struct
{
    madt_entry_header_t header;
    uint8_t acpi_processor_id;
    uint8_t apic_id;
    uint32_t flags;
} __attribute__((packed)) madt_entry_lapic_t;

typedef struct
{
    madt_entry_header_t header;
    uint8_t io_apic_id;
    uint8_t reserved;
    uint32_t io_apic_addr;
    uint32_t global_system_interrupt_base;
} __attribute__((packed)) madt_entry_io_apic_t;

typedef struct
{
    madt_entry_header_t header;
    uint8_t bus_source;
    uint8_t irq_source;
    uint32_t global_system_interrupt;
    uint16_t flags;
} __attribute__((packed)) madt_entry_interrupt_override_t;

typedef struct
{
    madt_entry_header_t header;
    uint8_t acpi_processor_id;
    uint16_t flags;
    uint8_t lint;
} __attribute__((packed)) madt_entry_nmi_t;

typedef struct
{
    madt_entry_header_t header;
    uint64_t addr;
} __attribute__((packed)) madt_entry_lapic_addr_override_t;

#endif

//=============================================================================
// End of file
//============================================================================= 
