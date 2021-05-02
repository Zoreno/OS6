/**
 * @file dsdt.h
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

#ifndef _ACPI_DSDT_H
#define _ACPI_DSDT_H

#include "acpi_header.h"

#include <stdint.h>

/**
 * Differentiated System Description Table.
 *
 * Signature must be "DSDT"
 */
typedef struct
{
    /**
     * ACPI standard header.
     */
    acpi_header_t header;

    /**
     * First byte of definition block.
     *
     * Size can be calculated from length in header.
     */
    uint8_t definition_block[1];
} __attribute__((packed)) dsdt_t;

#endif

//=============================================================================
// End of file
//=============================================================================
