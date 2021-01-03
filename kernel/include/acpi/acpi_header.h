/**
 * @file acpi_header.h
 * @author Joakim Bertils
 * @version 0.1
 * @date 2020-12-29
 * 
 * @brief 
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

#ifndef _ACPI_HEADER_H
#define _ACPI_HEADER_H

#include <stdint.h>

/**
* Standard ACPI table header. Makes up the first 36 bytes of most ACPI
* tables.
*/
typedef struct
{
    /**
	* 4 character ascii string of table identifier.
	*/
    char signature[4];

    /**
	* Length of the entire table in bytes, including header.
	*/
    uint32_t length;

    /**
	* Revision of the structure.
	*/
    uint8_t revision;

    /**
	* Checksum for the entire table. All bytes must add to 0 for the table
	* to be considered valid.
	*/
    uint8_t checksum;

    /**
	* An OEM-supplied string that identifies the OEM.
	*/
    char oem_id[6];

    /**
	* An OEM-supplied string that the OEM uses to identify the particular data
	* table.
	*/
    uint64_t oem_table_id;

    /**
	* An OEM-supplied revision number.
	*/
    uint32_t oem_revision;

    /**
	* Vendor ID of ultility that created the table.
	*/
    uint32_t creator_id;

    /**
	* Revision of the utility that created the table.
	*/
    uint32_t creator_revision;
} __attribute__((packed)) acpi_header_t;

#endif

//=============================================================================
// End of file
//=============================================================================