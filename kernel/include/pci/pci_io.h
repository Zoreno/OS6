/**
 * @file pci_io.h
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-04-18
 * 
 * @brief PCI IO functions
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

#ifndef _PCI_IO_H
#define _PCI_IO_H

#include <stdint.h>

/**
* @brief Reads a 8 bit PCI register.
*
* @param id		Device ID.
* @param reg	Register to read.
*
* @return		Data in register.
*/
uint8_t pci_read_b(uint32_t id, uint32_t reg);

/**
* @brief Reads a 16 bit PCI register.
*
* @param id		Device ID.
* @param reg	Register to read.
*
* @return		Data in register.
*/
uint16_t pci_read_w(uint32_t id, uint32_t reg);

/**
* @brief Reads a 32 bit PCI register.
*
* @param id		Device ID.
* @param reg	Register to read.
*
* @return		Data in register.
*/
uint32_t pci_read_l(uint32_t id, uint32_t reg);

/**
* @brief Writes a 8 bit value to a PCI register.
*
* @param id		Device ID.
* @param reg	Register to read.
* @param data	Data to write
*/
void pci_write_b(uint32_t id, uint32_t reg, uint8_t data);

/**
* @brief Writes a 16 bit value to a PCI register.
*
* @param id		Device ID.
* @param reg	Register to read.
* @param data	Data to write
*/
void pci_write_w(uint32_t id, uint32_t reg, uint16_t data);

/**
* @brief Writes a 32 bit value to a PCI register.
*
* @param id		Device ID.
* @param reg	Register to read.
* @param data	Data to write
*/
void pci_write_l(uint32_t id, uint32_t reg, uint32_t data);

#endif