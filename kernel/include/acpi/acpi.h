/**
 * @file acpi.h
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

#ifndef _ACPI_H
#define _ACPI_H

#include <acpi/madt.h>

#include <stdint.h>

/**
 * @brief Initiate the ACPI subsystem
 *
 *
 * @return 0 if successful
 */
int acpi_init();

/**
 * @brief Check if ACPI is currently enabled.
 *
 *
 * @return Non-zero if enabled.
 */
int acpi_is_enabled();

/**
 * @brief Enables ACPI power management.
 *
 *
 */
int acpi_enable();

/**
 * @brief Shuts down the computer
 *
 *
 */
void acpi_power_off();

/**
 * @brief Get the CMOS Century register
 *
 *
 * @return CMOS address of the century register.
 */
uint8_t acpi_get_century_register();

/**
 * @brief Returns a pointer to the MADT table, if present.
 *
 *
 * @return Pointer to MADT table, or NULL.
 */
madt_t *acpi_get_madt();

#endif

//=============================================================================
// End of file
//=============================================================================
