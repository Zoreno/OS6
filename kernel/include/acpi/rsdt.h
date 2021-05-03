/**
 * @file rsdt.h
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

#ifndef _ACPI_RDST_H
#define _ACPI_RSDT_H

#include "acpi_header.h"

#include <stdint.h>

/**
 * Root System Description Table
 *
 * Contains pointers to other system description tables.
 *
 * Header signature must be "RSDT"
 */
typedef struct
{
    /**
     * Standard ACPI header.
     */
    acpi_header_t header;

    /**
     * First table entry.
     *
     * Number of entries is determined by size of table.
     */
    uint32_t tables[1];
} __attribute__((packed)) rsdt_t;

#endif

//=============================================================================
// End of file
//=============================================================================
