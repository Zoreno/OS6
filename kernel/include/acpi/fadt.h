/**
 * @file fadt.h
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

#ifndef _ACPI_FADT_H
#define _ACPI_FADT_H

#include <stdint.h>

#include "acpi_header.h"
#include "gas.h"

/**
 * Fixed System Description Table
 *
 * Defines various fixed hardware ACPI information vital to an ACPI-compatible
 * OS.
 *
 * Header signature must be "FACP"
 */
typedef struct
{
    /**
     * Standard ACPI header.
     */
    acpi_header_t header;

    /**
     * Physical memory address of the FACS
     */
    uint32_t facs;

    /**
     * Physical memory address of the DSDT
     */
    uint32_t dsdt;

    /**
     * Reserved
     */
    uint8_t reserved;

    /**
     * Preferred power management profile.
     *
     * Field Values:
     * 0 Unspecified
     * 1 Desktop
     * 2 Mobile
     * 3 Workstation
     * 4 Enterprise Server
     * 5 SOHO Server
     * 6 Appliance PC
     * 7 Performance Server
     * 8 Tablet
     *
     * >8 Reserved
     */
    uint8_t preferred_power;

    /**
     * System vector of the SCI interrupt.
     */
    uint16_t SCI_interrupt;

    /**
     * System port address of the SMI command port.
     */
    uint32_t SMI_command_port;

    /**
     * The value to write to SMI_CommandPort to disable SMI ownership of
     * ACPI hardware registers.
     */
    uint8_t acpi_enable;

    /**
     * The value to write to SMI_CommandPort to re-enable SMI ownership of
     * ACPI hardware registers.
     */
    uint8_t acpi_disable;

    /**
     * The value to write to SMI_CommandPort to enter S4BIOS state.
     */
    uint8_t S4BIOS_REQ;

    /**
     * If non-zero, this field contains the value OSPM writes to the
     * SMI_CommandPort register to assume processor performance state
     * control responibility.
     */
    uint8_t PSTATE_control;

    /**
     * System port address of the PM1a Event Register Block.
     */
    uint32_t PM1a_event_block;

    /**
     * System port address of the PM1b Event Register Block.
     */
    uint32_t PM1b_event_block;

    /**
     * System port address of the PM1a Control Register Block.
     */
    uint32_t PM1a_control_block;

    /**
     * System port address of the PM1b Control Register Block.
     */
    uint32_t PM1b_control_block;

    /**
     * System port address of the PM2 Control Register Block.
     */
    uint32_t PM2_control_block;

    /**
     * System port address of the Power Management Timer Control
     * Register Block.
     */
    uint32_t PM_timer_block;

    /**
     * System port address of the General Purpose Event 0 Register Block.
     */
    uint32_t GPE0_block;

    /**
     * System port address of the General Purpose Event 1 Register Block.
     */
    uint32_t GPE1_block;

    /**
     * Number of bytes decoded by PM1 Event Block.
     */
    uint8_t PM1_event_length;

    /**
     * Number of bytes decoded by PM1 Control Block.
     */
    uint8_t PM1_control_length;

    /**
     * Number of bytes decoded by PM2 Control Block.
     */
    uint8_t PM2_control_length;

    /**
     * Number of bytes decoded by PM Timer Block.
     */
    uint8_t PM_timer_length;

    /**
     * Number of bytes decoded by GPE0 Block.
     */
    uint8_t GPE0_length;

    /**
     * Number of bytes decoded by GPE1 Block.
     */
    uint8_t GPE1_length;

    /**
     * Offset within ACPI GPE where GPE1 based events begin.
     */
    uint8_t GPE1_base;

    /**
     * If non-zero, this field contains the value OSPM writes to the
     * SMI_CommandPort register to indicate OS support for the _CST object
     * and C states Changed notification.
     */
    uint8_t C_state_control;

    /**
     * Worst case latency to exit a C2 state.
     */
    uint16_t worst_C2_latency;

    /**
     * Worst case latency to exit a C3 state.
     */
    uint16_t worst_C3_latency;

    /**
     * Number of flush strides that need to be read to completely flush
     * dirty lines from any processor's memory caches.
     */
    uint16_t flush_size;

    /**
     * Cache line with in bytes of processor memory caches.
     */
    uint16_t flush_stride;

    /**
     * Offset of duty cycle setting in P_CNT register.
     */
    uint8_t duty_offset;

    /**
     * With of duty cycke setting in P_CNT register
     */
    uint8_t duty_width;

    /**
     * RTC CMOS RMA index to the day-of-month alarm value.
     */
    uint8_t day_alarm;

    /**
     * RTC CMOS RMA index to the month alarm value.
     */
    uint8_t month_alarm;

    /**
     * RTC CMOS RMA index to the century of data value.
     */
    uint8_t century;

    /**
     * Boot architecture flags.
     */
    uint16_t boot_architecture_flags;

    /**
     * Reserved
     */
    uint8_t reserved2;

    /**
     * Fixed feature flags.
     */
    uint32_t flags;

    /**
     * Address to reset register
     */
    generic_address_structure_t reset_reg;

    /**
     * Indicates the value to write to ResetReg port to reset system.
     */
    uint8_t reset_value;

    /**
     * Reserved.
     */
    uint8_t reserved3[3];

    /**
     * 64bit pointer to FACS
     */
    uint64_t X_facs;

    /**
     * 64bit pointer to DSDT
     */
    uint64_t X_dsdt;

    /**
     * System port address of the PM1a Event Register Block. 64 bit pointer.
     */
    generic_address_structure_t X_PM1a_event_block;

    /**
     * System port address of the PM1b Event Register Block. 64 bit pointer.
     */
    generic_address_structure_t X_PM1b_event_block;

    /**
     * System port address of the PM1a Control Register Block. 64 bit pointer.
     */
    generic_address_structure_t X_PM1a_control_block;

    /**
     * System port address of the PM1b Control Register Block. 64 bit pointer.
     */
    generic_address_structure_t X_PM1b_control_block;

    /**
     * System port address of the PM2 Control Register Block. 64 bit pointer.
     */
    generic_address_structure_t X_PM2_control_block;

    /**
     * System port address of the Power Management Timer Control
     * Register Block. 64 bit pointer.
     */
    generic_address_structure_t X_PM_timer_block;

    /**
     * System port address of the General Purpose Event 0 Register Block.
     * 64 bit pointer.
     */
    generic_address_structure_t X_GPE0_block;

    /**
     * System port address of the General Purpose Event 1 Register Block.
     * 64 bit pointer.
     */
    generic_address_structure_t X_GPE1_block;

    /**
     * Address of the Sleep register. 64 bit pointer.
     */
    generic_address_structure_t SLEEP_CONTROL_REG;

    /**
     * Address of the Sleep status register. 64 bit pointer.
     */
    generic_address_structure_t SLEEP_STATUS_REG;
} __attribute__((packed)) fadt_t;

#endif

//=============================================================================
// End of file
//=============================================================================
