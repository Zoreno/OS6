/**
 * @file ide.c
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-06-22
 *
 * @brief This module implements a subset of the ATA command set for handling
 * HDDs through the IDE controller.
 *
 * Currently, two controllers with two devices each are supported. The module
 * will check the controllers for any conntected devices and setup a device for
 * each found deivce.
 *
 * @note ATAPI devices are not yet fully supported.
 *
 * Roadmap for this module:
 *
 *  - Implement support for ATAPI.
 *  - Support for DMA transfers.
 *  - Support LBA48 PIO mode.
 *  - IDENTIFY_DEVICE block as struct.
 *  - Read IOBASE from PCI instead of hard-coded values.
 *  - Parse more data from PCI.
 *  - Parse the feature sets present in the IDE controller.
 *  - Power management
 *  - Spawn a thread that handles all disk I/O. Other threads may send IPC
 *    to this thread to read from disk.
 *  - Keep track of statistics for blocks read and written, errors, etc.
 *  - All registers should be read and written with functions.
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

#include <arch/arch.h>
#include <debug/backtrace.h>
#include <drivers/blockdev.h>
#include <drivers/ide.h>
#include <logging/logging.h>
#include <pci/pci.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//=============================================================================
// Defintitions
//=============================================================================

#define NUM_IDE_CONTROLLERS 2
#define NUM_DEVICES_PER_CONTROLLER 2

#define PRIMARY_IDE_CONTROLLER 0
#define SECONDARY_IDE_CONTROLLER 1

// TODO: These should be read from the PCI, however the specification says
// that the controller always boots in legacy mode, which have these IOBASE
// registers as default.
#define PRIMARY_IDE_CONTROLLER_IOBASE 0x1F0
#define SECONDARY_IDE_CONTROLLER_IOBASE 0x170

/**
 * @brief Offset of each IDE controller register. Given relative to the
 * controller IOBASE.
 *
 *
 */
enum ata_register_t
{
    ATA_REGISTER_DATA = 0,
    ATA_REGISTER_ERROR = 1,
    ATA_REGISTER_NSECTOR = 2,
    ATA_REGISTER_SECTOR = 3,
    ATA_REGISTER_LCYL = 4,
    ATA_REGISTER_HCYL = 5,
    ATA_REGISTER_DRV_HEAD = 6,
    ATA_REGISTER_STATUS = 7,
    ATA_REGISTER_COMMAND = 7,
    ATA_REGISTER_DEV_CTL = 0x206,
};

/**
 * @brief Ata commands from the ATA command specification.
 *
 *
 */
typedef enum
{
    ATA_COMMAND_READ_SECTOR = 0x20,
    ATA_COMMAND_READ_SECTOR_NORETRY = 0x21,
    ATA_COMMAND_READ_LONG = 0x22,
    ATA_COMMAND_READ_LONG_NORETRY = 0x23,
    ATA_COMMAND_WRITE_SECTOR = 0x30,
    ATA_COMMAND_WRITE_SECTOR_NORETRY = 0x31,
    ATA_COMMAND_WRITE_LONG = 0x32,
    ATA_COMMAND_WRITE_LONG_NORETRY = 0x33,
    ATA_COMMAND_ATAPI_IDENTIFY = 0xA1,
    ATA_COMMAND_READ_MULTIPLE = 0xC4,
    ATA_COMMAND_WRITE_MULTIPLE = 0xC5,
    ATA_COMMAND_CHECK_POWER_MODE = 0xE5,
    ATA_COMMAND_FLUSH_CACHE = 0xE7,
    ATA_COMMAND_IDENTIFY = 0xEC,
    ATA_COMMAND_MEDIA_EJECT = 0xED,
} ata_command_t;

/**
 * @brief Bitfield for the ATA status register
 *
 *
 */
enum ata_status_register_t
{
    ATA_STATUS_BUSY = 0x80,
    ATA_STATUS_DEVICE_READY = 0x40,
    ATA_STATUS_DEVICE_FAULT = 0x20,
    ATA_STATUS_DEFERRED_WRITE_ERROR = 0x10,
    ATA_STATUS_DATA_REQUEST = 0x08,
    ATA_STATUS_ALIGNMENT_ERROR = 0x04,
    ATA_STATUS_SENSE_DATA = 0x02,
    ATA_STATUS_ERROR = 0x01,
};

/**
 * @brief Bitfield for the ATA error register
 *
 *
 */
enum ata_error_register_t
{
    ATA_ERROR_INTERFACE_CRC = 0x80,
    ATA_ERROR_UNCORRECTABLE_ERROR = 0x40,
    ATA_ERROR_ID_NOT_FOUND = 0x10,
    ATA_ERROR_ABORT = 0x04,
    ATA_ERROR_END_OF_MEDIA = 0x02,
};

/**
 * @brief Internal error codes for the IDE operations.
 *
 *
 */
typedef enum
{
    /**
     * @brief The operation finished successfully.
     *
     *
     */
    IDE_SUCCESS = 0,

    /**
     * @brief The operation finished with a general error.
     *
     *
     */
    IDE_ERROR = 1,

    /**
     * @brief The operation did not finish within the given time.
     *
     *
     */
    IDE_TIMEOUT = 2,

    /**
     * @brief Device was already selected during selection and force was not
     * specified.
     *
     *
     */
    IDE_ALREADY_SELECTED = 3,
} ide_error_t;

#define ATA_CTL_SRST 0x04
#define ATA_CTL_nIEN 0x02

/**
 * @brief Default timeout in microseconds for wait operations
 *
 *
 */
#define ATA_TIMEOUT 30000000

#define IO_READ 0
#define IO_WRITE 1

#define MASTER 0
#define SLAVE 1

/**
 * @brief The size of a block
 *
 *
 */
#define BLOCK_SIZE 512

#define MAX_NBLOCKS 256

struct _ide_controller;

typedef struct _ide_device
{
    struct _ide_controller *controller;

    uint8_t position : 1;
    uint8_t present : 1;
    uint8_t atapi : 1;
    uint8_t lba : 1;
    uint8_t dma : 1;

    char model[40];
    char serial[20];
    char firmware[8];

    uint32_t cylinders;
    uint32_t heads;
    uint32_t sectors;
    uint32_t capacity;

} ide_device_t;

typedef struct _ide_controller
{
    uint32_t iobase;

    ide_device_t devices[NUM_DEVICES_PER_CONTROLLER];

    volatile int irq;

} ide_controller_t;

//=============================================================================
// Local variables
//=============================================================================

static ide_controller_t controllers[NUM_IDE_CONTROLLERS] = {0};

///=============================================================================
// Private function forward declarations
//=============================================================================

static uint32_t get_iobase(ide_controller_t *controller);
static uint8_t read_status_register(ide_controller_t *controller);
static uint8_t read_error_register(ide_controller_t *controller);
static void clear_error_register(ide_controller_t *controller);
static void write_control_register(ide_controller_t *controller, uint8_t value);
static void write_command(ide_controller_t *controller, ata_command_t cmd);
static void write_data(ide_controller_t *controller, uint16_t data);
static uint16_t read_data(ide_controller_t *controller);
static void wait_for_irq(ide_controller_t *controller);
static void write_block(ide_controller_t *controller, const void *buffer);
static void read_block(ide_controller_t *controller, void *buffer);
static void read_block_no_irq(ide_controller_t *controller, void *buffer);
static ide_error_t flush_cache(ide_controller_t *controller);
static size_t format_size_shift_and_print(size_t size,
                                          char *buffer,
                                          size_t bits,
                                          const char *unit);
static size_t format_size_to_human_readable(size_t size, char *buffer);
static void format_device_info(ide_device_t *device,
                               char *buf,
                               size_t controller_index);
static void fix_ide_string(char *s, int len);
static ide_error_t wait_for_controller(ide_controller_t *controller,
                                       uint8_t mask,
                                       uint8_t value,
                                       uint32_t timeout);
static ide_error_t reset_controller(ide_controller_t *controller);
static void delay400ns(ide_device_t *device);
static uint8_t read_drive_head_register(ide_controller_t *controller);
static void write_drive_head_register(ide_controller_t *controller,
                                      uint8_t value);
static int is_device_selected(ide_device_t *device);
static ide_error_t wait_until_ready(ide_controller_t *controller,
                                    size_t num_retries);
static ide_error_t select_device(ide_device_t *device, int force);
static void identify_ide_device(ide_device_t *device);
static ide_device_t *get_ide_device(unsigned int minor);
static uint32_t ide_read_write_blocks(uint32_t minor,
                                      uint32_t block,
                                      uint32_t nblocks,
                                      void *buffer,
                                      int direction);
static uint32_t ide_write_blocks(uint32_t minor,
                                 uint32_t block,
                                 uint32_t nblocks,
                                 void *buffer);
static uint32_t ide_read_blocks(uint32_t minor,
                                uint32_t block,
                                uint32_t nblocks,
                                void *buffer);
static void ide_primary_irq(system_stack_t *regs);
static void ide_secondary_irq(system_stack_t *regs);
static uint8_t read_count_register(ide_controller_t *controller);
static void write_count_register(ide_controller_t *controller, uint8_t value);
static ide_error_t check_power_mode(ide_controller_t *controller);

//=============================================================================
// Private functions
//=============================================================================

/**
 * @brief Get the base of the register map for the controller.
 *
 * @param controller Controller to get the iobase register for.
 *
 * @return Base register for IO.
 */
static uint32_t get_iobase(ide_controller_t *controller)
{
    return controller->iobase;
}

/**
 * @brief Read from the status register, i.e. IOBASE + 7
 *
 * @param controller The controller to read from.
 *
 * @return Value in the status register.
 */
static uint8_t read_status_register(ide_controller_t *controller)
{
    uint32_t iobase = get_iobase(controller);

    return inportb(iobase + ATA_REGISTER_STATUS);
}

/**
 * @brief Read the error register.
 *
 * @param controller The controller to read from.
 *
 * @return Value in the error register.
 */
static uint8_t read_error_register(ide_controller_t *controller)
{
    uint32_t iobase = get_iobase(controller);

    return inportb(iobase + ATA_REGISTER_ERROR);
}

/**
 * @brief Clear the error register by writing a zero.
 *
 * @param controller The controller to write to.
 *
 */
static void clear_error_register(ide_controller_t *controller)
{
    uint32_t iobase = get_iobase(controller);

    outportb(iobase + ATA_REGISTER_ERROR, 0);
}

/**
 * @brief Write to the DEV_CTL register
 *
 * @param controller The controller to write to.
 * @param value Value to write.
 *
 */
static void write_control_register(ide_controller_t *controller, uint8_t value)
{
    uint32_t iobase = get_iobase(controller);

    outportb(iobase + ATA_REGISTER_DEV_CTL, value);
}

/**
 * @brief Write a command to the controller.
 *
 * @param controller Controller to write the command to.
 * @param cmd Command to write.
 *
 */
static void write_command(ide_controller_t *controller, ata_command_t cmd)
{
    uint32_t iobase = get_iobase(controller);

    outportb(iobase + ATA_REGISTER_COMMAND, cmd);
}

/**
 * @brief Write a word to the data register.
 *
 * @param controller Controller to write to.
 * @param data 2 byte data to write.
 *
 */
static void write_data(ide_controller_t *controller, uint16_t data)
{
    uint32_t iobase = get_iobase(controller);

    outportw(iobase + ATA_REGISTER_DATA, data);
}

/**
 * @brief Read a word from the data register.
 *
 * @param controller Controller to read from.
 *
 * @return Word read from the data register.
 */
static uint16_t read_data(ide_controller_t *controller)
{
    uint32_t iobase = get_iobase(controller);

    return inportw(iobase + ATA_REGISTER_DATA);
}

/**
 * @brief Waits until the IRQ flag is set and then clears the flag.
 *
 * @param controller Controller to wait for.
 *
 */
static void wait_for_irq(ide_controller_t *controller)
{
    while (!controller->irq)
        ;

    controller->irq = 0;
}

/**
 * @brief Writes a block of data (@ref BLOCK_SIZE bytes).
 *
 * Waits for IRQ to fire after write.
 *
 * @param controller Controller to write to.
 * @param buffer Buffer to write.
 *
 */
static void write_block(ide_controller_t *controller, const void *buffer)
{
    const size_t words_per_block = BLOCK_SIZE / sizeof(uint16_t);

    const uint16_t *buf = (const uint16_t *)buffer;

    for (size_t i = 0; i < words_per_block; ++i)
    {
        write_data(controller, buf[i]);
    }

    wait_for_irq(controller);
}

/**
 * @brief Reads a block of data (@ref BLOCK_SIZE bytes).
 *
 * Waits for IRQ to fire before read.
 *
 * @param controller Controller to read from.
 * @param buffer Buffer to write to.
 *
 */
static void read_block(ide_controller_t *controller, void *buffer)
{
    const size_t words_per_block = BLOCK_SIZE / sizeof(uint16_t);

    uint16_t *buf = (uint16_t *)buffer;

    wait_for_irq(controller);

    for (size_t i = 0; i < words_per_block; ++i)
    {
        buf[i] = read_data(controller);
    }
}

/**
 * @brief Reads a block of data (@ref BLOCK_SIZE bytes).
 *
 * Does not wait for IRQ to fire before reading.
 *
 * @param controller Controller to read from.
 * @param buffer Buffer to write to.
 *
 */
static void read_block_no_irq(ide_controller_t *controller, void *buffer)
{
    const size_t words_per_block = BLOCK_SIZE / sizeof(uint16_t);

    uint16_t *buf = (uint16_t *)buffer;

    for (size_t i = 0; i < words_per_block; ++i)
    {
        buf[i] = read_data(controller);
    }
}

/**
 * @brief Sends the FLUSH_CACHE command to the controller.
 *
 * @param controller Controller to send command to.
 *
 * @return Error code representing the result of the operation.
 */
static ide_error_t flush_cache(ide_controller_t *controller)
{
    write_command(controller, ATA_COMMAND_FLUSH_CACHE);

    if (!wait_for_controller(controller, ATA_STATUS_BUSY, 0, ATA_TIMEOUT))
    {
        log_error(
            "[IDE] Error waiting for controller after sending flush "
            "command");
        return IDE_TIMEOUT;
    }

    return IDE_SUCCESS;
}

/**
 * @brief Prints the number @a size with one decimal after shifting it @a bits
 * bits to the right.
 *
 * @param size Number ot shift.
 * @param buffer Buffer to print the number to.
 * @param bits Number of bits to shift.
 * @param unit String containing the unit to append to the buffer.
 *
 * @return Number of bytes emitted to @a buffer.
 */
static size_t format_size_shift_and_print(size_t size,
                                          char *buffer,
                                          size_t bits,
                                          const char *unit)
{
    size_t t;
    size_t td;

    if (bits > 0)
    {
        // Get whole units larger than the size of the unit.
        t = size / (1ULL << bits);

        // Get first decimal (base 10)
        td = (size - t * (1ULL << bits)) / ((1ULL << bits) / 10ULL);
    }
    else
    {
        t = size;
        td = 0;
    }

    // TODO: Unit should be optional
    return sprintf(buffer, "%d.%1d%c%s", (int)t, (int)td, ' ', unit);
}

/**
 * @brief Prints the size with unit and one decimal, i.e. 100.0 KB.
 *
 * @param size Size to format
 * @param buffer Buffer to print the result to.
 *
 * @return Number of characters emitted to @a buffer.
 */
static size_t format_size_to_human_readable(size_t size, char *buffer)
{
    struct
    {
        size_t bits;
        const char *unit;
    } formats[] = {
        {.bits = 40, .unit = "TB"},
        {.bits = 30, .unit = "GB"},
        {.bits = 20, .unit = "MB"},
        {.bits = 10, .unit = "KB"},
        {.bits = 0, .unit = "B"},
    };

    for (size_t i = 0; i < sizeof(formats) / sizeof(*formats); ++i)
    {
        if (size >= (1ULL << formats[i].bits))
        {
            return format_size_shift_and_print(
                size, buffer, formats[i].bits, formats[i].unit);
        }
    }

    // We did not find a matching format
    // This should not happen though
    return 0;
}

/**
 * @brief Formats the device info into a human-readable string.
 *
 * @param device Device to print the info for.
 * @param buffer Buffer to write the index to.
 * @param controller_index Index of the controller the device is connected to.
 *
 */
static void format_device_info(ide_device_t *device,
                               char *buffer,
                               size_t controller_index)
{
    size_t size = device->capacity * BLOCK_SIZE;

    char size_buffer[32];
    memset(size_buffer, 0, sizeof(size_buffer));
    format_size_to_human_readable(size, size_buffer);

    sprintf(buffer,
            "%s [%i-%i]: %s (%i/%i/%i - %i sectors (%s)) LBA:%s "
            "- DMA:%s",
            device->atapi ? "CD-ROM" : "Hard Disk",
            controller_index,
            device->position,
            device->model,
            device->cylinders,
            device->heads,
            device->sectors,
            device->capacity,
            size_buffer,
            device->lba ? "YES" : "NO",
            device->dma ? "YES" : "NO");
}

/**
 * @brief Converts an ATA string to an ASCII string.
 *
 * @param s String to convert.
 * @param len Size of the buffer containing the string.
 *
 */
static void fix_ide_string(char *s, int len)
{
    char c;
    char *p = s;
    char *end = s + (len & ~1);

    while (p != end)
    {
        c = *p;

        *p = *(p + 1);

        *(p + 1) = c;

        p += 2;
    }

    p = end - 1;

    *p-- = '\0';

    while (p-- != s)
    {
        c = *p;

        if (c > 32 && c < 127)
        {
            break;
        }

        *p = '\0';
    }
}

/**
 * @brief Waits for the controller to set the given bits.
 *
 * The condition is fulfilled if status & @a mask == @a value.
 *
 * @param controller Controller to wait for.
 * @param mask Mask with the interesting bits.
 * @param value Value to wait for.
 * @param timeout Timeout until the operation is considered failed.
 *
 * @return Error code representing the result of the operation.
 */
static ide_error_t wait_for_controller(ide_controller_t *controller,
                                       uint8_t mask,
                                       uint8_t value,
                                       uint32_t timeout)
{
    uint8_t status;

    do
    {
        udelay(1);

        status = read_status_register(controller);

    } while ((status & mask) != value && --timeout);

    if (timeout == 0)
    {
        return IDE_TIMEOUT;
    }

    return IDE_SUCCESS;
}

/**
 * @brief Try to reset the controller by writing to the control register.
 *
 * @param controller Controller to reset.
 *
 * @return Error code representing the result of the operation.
 */
static ide_error_t reset_controller(ide_controller_t *controller)
{
    uint32_t iobase = controller->iobase;

    write_control_register(controller, ATA_CTL_SRST);

    mdelay(2);

    write_control_register(controller, 0);

    if (wait_for_controller(controller, ATA_STATUS_BUSY, 0, ATA_TIMEOUT) !=
        IDE_SUCCESS)
    {
        return IDE_TIMEOUT;
    }

    return IDE_SUCCESS;
}

/**
 * @brief Delay 400ns by reading from the status register.
 *
 * Each read takes ~100ns, so 4 reads is needed.
 *
 * @param device Device to read from.
 *
 */
static void delay400ns(ide_device_t *device)
{
    for (int i = 0; i < 4; ++i)
    {
        read_status_register(device->controller);
    }
}

/**
 * @brief Read from the drive head register.
 *
 * @param controller Controller to read from.
 *
 * @return Value of the register.
 */
static uint8_t read_drive_head_register(ide_controller_t *controller)
{
    uint32_t iobase = get_iobase(controller);

    return inportb(iobase + ATA_REGISTER_DRV_HEAD);
}

/**
 * @brief Write to the drive head register.
 *
 * @param controller Controller to write to.
 * @param value Value to write to the register.
 *
 */
static void write_drive_head_register(ide_controller_t *controller,
                                      uint8_t value)
{
    uint32_t iobase = get_iobase(controller);

    outportb(iobase + ATA_REGISTER_DRV_HEAD, value);
}

/**
 * @brief Check whether the device is the currently selected device.
 *
 * @param device Device to check.
 *
 * @return Non-zero if selected.
 */
static int is_device_selected(ide_device_t *device)
{
    uint8_t tmp = read_drive_head_register(device->controller);

    return (tmp ^ (device->position << 4)) == 0x10;
}

/**
 * @brief Waits until both @ref ATA_STATUS_BUSY and @ref ATA_STATUS_DATA_REQUEST
 * are cleared in the status register.
 *
 * @param controller Controller to wait for.
 * @param num_retries Number of microseconds to wait (roughly).
 *
 * @return Error code representing the result of the operation.
 */
static ide_error_t wait_until_ready(ide_controller_t *controller,
                                    size_t num_retries)
{
    size_t i = 0;

    while ((read_status_register(controller) &
            (ATA_STATUS_BUSY | ATA_STATUS_DATA_REQUEST)))
    {
        udelay(1);
        ++i;

        if (i >= num_retries)
        {
            return IDE_TIMEOUT;
        }
    }

    return IDE_SUCCESS;
}

/**
 * @brief Select a device as the currently active device.
 *
 * All commands to the contoller will affect the currently selected drive.
 *
 * @param device Device to select.
 * @param force Wehter to change device even if the device is currently
 * selected.
 *
 * @return Non-zero on success.
 */
static ide_error_t select_device(ide_device_t *device, int force)
{
    ide_controller_t *controller = device->controller;

    const size_t num_retries = 100;

    if (!force && is_device_selected(device))
    {
        log_warn("[IDE] Device already selected");

        return IDE_ALREADY_SELECTED;
    }

    if (wait_until_ready(controller, num_retries) != IDE_SUCCESS)
    {
        log_warn("[IDE] Resetting controller");

        if (reset_controller(controller) != IDE_SUCCESS)
        {
            log_error("[IDE] Failed to reset controller");
        }

        return IDE_ERROR;
    }

    write_drive_head_register(
        controller,
        (uint8_t)0xA0 | (device->lba << 6) | (device->position << 4));

    delay400ns(device);

    uint8_t status;

    if (wait_until_ready(controller, num_retries) != IDE_SUCCESS)
    {
        log_warn("[IDE] Devce not ready after selecting");

        if (reset_controller(controller) != IDE_SUCCESS)
        {
            log_error("[IDE] Failed to reset controller");
        }

        return IDE_ERROR;
    }

    return IDE_SUCCESS;
}

/**
 * @brief Try to identify the device.
 *
 * Will populate the @a device struct with information found.
 *
 * @param device Pointer to device struct.
 *
 */
static void identify_ide_device(ide_device_t *device)
{
    int i;
    ide_controller_t *controller = device->controller;
    uint32_t iobase = controller->iobase;

    uint8_t status;
    uint8_t cl;
    uint8_t ch;
    ata_command_t cmd;
    uint16_t info[256];

    device->present = 0;

    write_count_register(controller, 0xAB);

    if (read_count_register(controller) != 0xAB)
    {
        return;
    }

    ide_error_t reset = reset_controller(controller);

    if (reset != IDE_SUCCESS)
    {
        log_error("[IDE] Failed to reset controller");
        return;
    }

    if (select_device(device, 1) != IDE_SUCCESS)
    {
        log_error("[IDE] Failed to select device");

        return;
    }

    uint8_t count = read_count_register(controller);
    uint8_t lba = inportb(iobase + ATA_REGISTER_SECTOR);

    if (count == 0x01 && lba == 0x01)
    {
        cl = inportb(iobase + ATA_REGISTER_LCYL);
        ch = inportb(iobase + ATA_REGISTER_HCYL);

        status = read_status_register(controller);

        if (cl == 0x14 && ch == 0xeb)
        {
            device->present = 1;
            device->atapi = 1;
        }
        else if (cl == 0 && ch == 0 && status != 0)
        {
            device->present = 1;
        }
    }

    if (!device->present)
    {
        return;
    }

    cmd = device->atapi ? ATA_COMMAND_ATAPI_IDENTIFY : ATA_COMMAND_IDENTIFY;

    write_command(controller, cmd);

    if (wait_for_controller(
            controller,
            ATA_STATUS_BUSY | ATA_STATUS_DATA_REQUEST | ATA_STATUS_ERROR,
            ATA_STATUS_DATA_REQUEST,
            ATA_TIMEOUT) != IDE_SUCCESS)
    {
        device->present = 0;
        return;
    }

    read_block_no_irq(controller, info);

    device->lba = (info[49] >> 9) & 1;
    device->dma = (info[49] >> 8) & 1;

    device->cylinders = (uint32_t)info[1];
    device->heads = (uint32_t)info[3];
    device->sectors = (uint32_t)info[6];

    if (device->lba)
    {
        device->capacity = (uint32_t)info[60] + (uint32_t)(info[61] << 16);
    }
    else
    {
        device->capacity = device->heads * device->sectors * device->cylinders;
    }

    memcpy(device->model, &info[27], 40);
    memcpy(device->serial, &info[10], 20);
    memcpy(device->firmware, &info[23], 8);

    fix_ide_string(device->model, 40);
    fix_ide_string(device->serial, 20);
    fix_ide_string(device->firmware, 8);
}

/**
 * @brief Get the ide device from the minor number.
 *
 * @param minor Minor number describing the device. Valid values are [0-3].
 *
 * @return Pointer to IDE device object.
 */
static ide_device_t *get_ide_device(unsigned int minor)
{
    ide_controller_t *controller;
    ide_device_t *device;

    controller = &controllers[minor / NUM_DEVICES_PER_CONTROLLER];
    device = &controller->devices[minor % NUM_DEVICES_PER_CONTROLLER];

    return device;
}

static uint32_t ide_read_write_blocks(uint32_t minor,
                                      uint32_t block,
                                      uint32_t nblocks,
                                      void *buffer,
                                      int direction)
{
    ide_device_t *device;
    ide_controller_t *controller;

    uint8_t *buf = (uint8_t *)buffer;
    uint8_t sc;
    uint8_t cl;
    uint8_t ch;
    uint8_t hd;
    ata_command_t cmd;

    uint32_t iobase;
    uint32_t i;

    device = get_ide_device(minor);

    if (!device->present)
    {
        log_error("[IDE] Device not present");
        return 0;
    }

    if (!nblocks)
    {
        log_warn("[IDE] Zero blocks requested");
        return 0;
    }

    if (nblocks >= MAX_NBLOCKS)
    {
        nblocks = MAX_NBLOCKS;
    }

    if (block + nblocks > device->capacity)
    {
        log_error("[IDE] Request outside capacity");
        log_debug("[IDE] block+nblocks: %i, device->capacity: %i",
                  block + nblocks,
                  device->capacity);
        return 0;
    }

    controller = device->controller;
    iobase = controller->iobase;

    if (select_device(device, 1) != IDE_SUCCESS)
    {
        log_error("[IDE] Could not select device");
        return 0;
    }

    if (device->lba)
    {
        // 28 bit PIO mode
        sc = block & 0xFF;
        cl = (block >> 8) & 0xFF;
        ch = (block >> 16) & 0xFF;
        hd = (block >> 24) & 0xF;
    }
    else
    {
        // CHS mode
        int cyl = block / (device->heads * device->sectors);
        int tmp = block % (device->heads * device->sectors);

        sc = tmp % device->sectors + 1;
        cl = cyl & 0xFF;
        ch = (cyl >> 8) & 0xFF;
        hd = tmp / device->sectors;
    }

    cmd = (direction == IO_READ) ? ATA_COMMAND_READ_SECTOR
                                 : ATA_COMMAND_WRITE_SECTOR;

    controller->irq = 0;

    write_drive_head_register(
        controller, 0xE0 | (device->lba << 6) | (device->position << 4) | hd);
    clear_error_register(controller);

    outportb(iobase + ATA_REGISTER_NSECTOR, nblocks);
    outportb(iobase + ATA_REGISTER_SECTOR, sc);
    outportb(iobase + ATA_REGISTER_LCYL, cl);
    outportb(iobase + ATA_REGISTER_HCYL, ch);

    write_command(controller, cmd);

    delay400ns(device);

    if (read_status_register(controller) & ATA_STATUS_ERROR)
    {
        log_error("[IDE] Device reported error");
        return 0;
    }

    if (direction == IO_WRITE)
    {
        for (int block = 0; block < nblocks; ++block)
        {
            write_block(controller, buf);

            buf += BLOCK_SIZE;
        }
    }

    if (direction == IO_READ)
    {
        for (int block = 0; block < nblocks; ++block)
        {
            read_block(controller, buf);

            buf += BLOCK_SIZE;
        }
    }

    if (read_status_register(controller) & ATA_STATUS_ERROR)
    {
        log_error("[IDE] Device reported error");
        return 0;
    }

    if (direction == IO_WRITE)
    {
        if (flush_cache(controller) != IDE_SUCCESS)
        {
            // We failed to flush the cache, so the the write did not go as
            // planned.
            return 0;
        }
    }

    return nblocks;
}

/**
 * @brief Writes a number of blocks to the IDE device.
 *
 * @param minor Minor number for the device.
 * @param block Starting block to write to.
 * @param nblocks Number of blocks to write.
 * @param buffer Buffer to write from.
 *
 * @return Number of blocks written.
 */
static uint32_t ide_write_blocks(uint32_t minor,
                                 uint32_t block,
                                 uint32_t nblocks,
                                 void *buffer)
{
    return ide_read_write_blocks(minor, block, nblocks, buffer, IO_WRITE);
}

/**
 * @brief Reads a number of blocks from the IDE device.
 *
 * @param minor Minor number for the device.
 * @param block Starting block to read from.
 * @param nblocks Number of blocks to read.
 * @param buffer Buffer to read to.
 *
 * @return Number of blocks read.
 */
static uint32_t ide_read_blocks(uint32_t minor,
                                uint32_t block,
                                uint32_t nblocks,
                                void *buffer)
{
    return ide_read_write_blocks(minor, block, nblocks, buffer, IO_READ);
}

/**
 * @brief Handles an interrupt for the given controller.
 *
 * @param controller Controller raising the interrupt.
 *
 */
static void ide_handle_interrupt(ide_controller_t *controller)
{
    // We must read the status register each irq
    volatile uint8_t dummy = read_status_register(controller);

    controller->irq = 1;
}

/**
 * @brief Interrupt handler for the IRQ of the primary IDE controller.
 *
 * @param regs Registers at the time of the interrupt.
 *
 */
static void ide_primary_irq(system_stack_t *regs)
{
    (void)regs;

    ide_handle_interrupt(&controllers[PRIMARY_IDE_CONTROLLER]);
}

/**
 * @brief Interrupt handler for the IRQ of the secondary IDE controller.
 *
 * @param regs Registers at the time of the interrupt.
 *
 */
static void ide_secondary_irq(system_stack_t *regs)
{
    (void)regs;

    ide_handle_interrupt(&controllers[SECONDARY_IDE_CONTROLLER]);
}

/**
 * @brief Read from the count register.
 *
 * @param controller Controller to read from.
 *
 * @return Value of the count register.
 */
static uint8_t read_count_register(ide_controller_t *controller)
{
    uint32_t iobase = get_iobase(controller);

    return inportb(iobase + ATA_REGISTER_NSECTOR);
}

/**
 * @brief Write tot the count register.
 *
 * @param controller Controller to write to.
 * @param value Value to write.
 *
 */
static void write_count_register(ide_controller_t *controller, uint8_t value)
{
    uint32_t iobase = get_iobase(controller);

    outportb(iobase + ATA_REGISTER_NSECTOR, value);
}

/**
 * @brief Checks the current power mode of the controller.
 *
 * @param controller Controller to check.
 *
 * @return Error code indicating the result of the operation.
 */
static ide_error_t check_power_mode(ide_controller_t *controller)
{
    if (!controller)
    {
        return IDE_ERROR;
    }

    write_command(controller, ATA_COMMAND_CHECK_POWER_MODE);

    wait_until_ready(controller, ATA_TIMEOUT);

    if (read_status_register(controller) &
        (ATA_STATUS_ERROR | ATA_STATUS_DEVICE_FAULT))
    {
        log_error("[IDE] Failed to run command CHECK_POWER_MODE");

        return IDE_ERROR;
    }

    uint8_t mode = read_count_register(controller);

    switch (mode)
    {
    case 0x00:
    case 0x01:
        log_info("[IDE] Device is in standby mode");
        break;
    case 0x80:
    case 0x81:
    case 0x82:
    case 0x83:
        log_info("[IDE] Device is in idle mode");
        break;
    case 0xFF:
        log_info("[IDE] Device is active or idle");
        break;
    default:
        log_error("[IDE] Device is in an indeterminate state");
        return IDE_ERROR;
    }

    // TODO: Check LOW_POWER_STANDBY_SUPPORTED and query LBA register too.
    // TODO: Return the power state from this function somehow...

    return IDE_SUCCESS;
}

//=============================================================================
// Interface functions
//=============================================================================

void init_ide_devices()
{
    log_info("[IDE] Initializing");

    int i;
    int j;

    ide_device_t *device;
    ide_controller_t *controller;

    char msg[256];

    if (reg_blockdev_class(
            0, "IDE HDD Driver", &ide_read_blocks, &ide_write_blocks))
    {
        log_error("[IDE] Failed to register IDE HDD Driver");

        return;
    }

    for (i = 0; i < NUM_IDE_CONTROLLERS; ++i)
    {
        controller = &controllers[i];
        controller->iobase = (i == 0) ? PRIMARY_IDE_CONTROLLER_IOBASE
                                      : SECONDARY_IDE_CONTROLLER_IOBASE;
        controller->irq = 0;

        for (j = 0; j < NUM_DEVICES_PER_CONTROLLER; ++j)
        {
            device = &controller->devices[j];
            device->controller = controller;
            device->position = (j == 0) ? MASTER : SLAVE;

            identify_ide_device(device);

            if (!device->present)
            {
                continue;
            }

            if (device->atapi)
            {
                log_warn(
                    "[IDE] ATAPI device found but currently not supported");

                continue;
            }

            format_device_info(device, msg, i);

            reg_blockdev_instance(0,
                                  i * NUM_DEVICES_PER_CONTROLLER + j,
                                  msg,
                                  BLOCK_SIZE,
                                  device->capacity);

            log_info("[IDE] %s", msg);
            check_power_mode(device->controller);

            if (i == 0 && j == 0)
            {
                if (select_device(device, 1) != IDE_SUCCESS)
                {
                    log_error("[IDE] Failed to select device");
                }
            }
        }
    }

    set_irq_handler(14, ide_primary_irq);
    set_irq_handler(15, ide_secondary_irq);

    clear_mask_interrupt(14);
    clear_mask_interrupt(15);

    log_info("[IDE] Done!");
}

//=============================================================================
// End of file
//=============================================================================
