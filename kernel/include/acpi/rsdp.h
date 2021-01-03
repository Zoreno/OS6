/**
 * @file rsdp.h
 * @author Joakim Bertils
 * @version 0.1
 * @date 2020-12-29
 * 
 * @brief ACPI Root system description pointer.
 * 
 * @copyright Copyright (C) 2020,
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

#ifndef _ACPI_RSDP_H
#define _ACPI_RSDP_H

#include <stdint.h>

/**
* Root System Description Pointer (RSDP)
*
* Structure providing the location of the RSDT or XSDT
*/
typedef struct
{
    /**
	* Signature.
	*
	* Should be "RSD PTR "
	*/
    char signature[8];

    /**
	* Checksum of the fields defined in ACPI 1.0 spec. Sum of all bytes must be
	* 0.
	*/
    uint8_t checksum;

    /**
	* An OEM-supplied string that identifies the OEM.
	*/
    char oemid[6];

    /**
	* Revision of the structure. ACPI 1.0 value is 0. Current value
	* should be 2.
	*/
    uint8_t revision;

    /**
	* 32 bit physical address of the RSDT.
	*/
    uint32_t rsdt_address;

    /**
	* Length of the table including the header.
	*/
    uint32_t length;

    /**
	* 64 bit physical address of the XSDT.
	*/
    uint64_t xsdt_address;

    /**
	* Checksum of the entire extended(ACPI 2.0+) table. Sum of all bytes should
	* be 0.
	*/
    uint8_t extended_checksum;

    /**
	* Reserved.
	*/
    char reserved[3];
} __attribute__((packed)) rsdp_t;

#endif

//=============================================================================
// End of file
//=============================================================================