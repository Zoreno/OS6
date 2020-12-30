/**
 * @file gas.h
 * @author Joakim Bertils
 * @version 0.1
 * @date 2020-12-29
 * 
 * @brief ACPI Generic address structure.
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

#ifndef _ACPI_GAS_H
#define _ACPI_GAS_H

#include <stdint.h>

/**
* Generic Address Structure.
*
* Uniform way of describing register locations. Describes registers within
* tables in ACPI.
*/
typedef struct
{
    /**
	* Address Space ID.
	*
	* Defined values are:
	*
	* 0				System Memory
	* 1				System I/O
	* 2				PCI Configuration Space
	* 3				Embedded Controller
	* 4				SMBus
	* 5 to 0x09		Reserved
	* 0x0A			Platform Communications Channel (PCC)
	* 0x0B to 0x7E	Reserved
	* 0x7F			Functional Fixed Hardware
	* 0x80 to 0xBF	Reserved
	* 0xC0 to 0xFF	OEM Defined
	*/
    uint8_t address_space;

    /**
	* Size in bits of given register.
	*/
    uint8_t bit_width;

    /**
	* Bit offset of given register.
	*/
    uint8_t bit_offset;

    /**
	* Specifies access size.
	*
	* Defined values are:
	*
	* 0 Undefined (legacy reasons)
	* 1 Byte access
	* 2 Word access
	* 3 Dword access
	* 4 QWord access
	*/
    uint8_t access_size;

    /**
	* 64 bit address of register in given address space.
	*/
    uint64_t address;
} __attribute__((packed)) generic_address_structure_t;

#endif

//=============================================================================
// End of file
//=============================================================================