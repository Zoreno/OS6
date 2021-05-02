/**
 * @file ide.c
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-06-22
 *
 * @brief
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

#include <drivers/ide.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <arch/arch.h>
#include <debug/backtrace.h>
#include <drivers/blockdev.h>
#include <logging/logging.h>

#define NUM_IDE_CONTROLLERS 2
#define NUM_DEVICES_PER_CONTROLLER 2

#define PRIMARY_IDE_CONTROLLER 0
#define SECONDARY_IDE_CONTROLLER 1

// TODO: These should be read from the PCI, however the specification says
// that the controller always boots in legacy mode, which have these IOBASE
// registers as default.
#define PRIMARY_IDE_CONTROLLER_IOBASE 0x1F0
#define SECONDARY_IDE_CONTROLLER_IOBASE 0x170

#define ATA_DATA 0
#define ATA_ERROR 1
#define ATA_NSECTOR 2
#define ATA_SECTOR 3
#define ATA_LCYL 4
#define ATA_HCYL 5
#define ATA_DRV_HEAD 6
#define ATA_STATUS 7
#define ATA_COMMAND 7
#define ATA_DEV_CTL 0x206

#define ATA_IDENTIFY 0xEC
#define ATAPI_IDENTIFY 0xA1
#define ATA_READ_BLOCK 0x20
#define ATA_READ_MULTIPLE 0xC4
#define ATA_WRITE_BLOCK 0x30

#define ATA_STATUS_BSY 0x80
#define ATA_STATUS_DRDY 0x40
#define ATA_STATUS_DRQ 0x08
#define ATA_STATUS_ERR 0x01

#define ATA_CTL_SRST 0x04
#define ATA_CTL_nIEN 0x02

#define ATA_TIMEOUT 30000000

#define IO_READ 0
#define IO_WRITE 1

#define MASTER 0
#define SLAVE 1

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

static ide_controller_t controllers[NUM_IDE_CONTROLLERS] = {0};

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

static uint8_t wait_for_controller(ide_controller_t *controller,
                                   uint8_t mask,
                                   uint8_t value,
                                   uint32_t timeout)
{
    uint8_t status;

    do
    {
        udelay(1);

        status = inportb(controller->iobase + ATA_STATUS);

    } while ((status & mask) != value && --timeout);

    return timeout;
}

static uint8_t reset_controller(ide_controller_t *controller)
{
    uint32_t iobase = controller->iobase;

    outportb(iobase + ATA_DEV_CTL, ATA_CTL_SRST);

    mdelay(2);

    outportb(iobase + ATA_DEV_CTL, 0);

    if (!wait_for_controller(controller, ATA_STATUS_BSY, 0, ATA_TIMEOUT))
    {
        return 0;
    }

    return 1;
}

static uint8_t select_device(ide_device_t *device, int force)
{
    uint32_t iobase = device->controller->iobase;

    int num_reties = 100;

    int i = 0;

    uint8_t tmp = inportb(iobase + ATA_DRV_HEAD);

    if (!force && ((tmp ^ (device->position << 4)) == 0x10))
    {
        log_warn("[IDE] Device already selected");

        return 1;
    }

    while ((inportb(iobase + ATA_STATUS) & (ATA_STATUS_BSY | ATA_STATUS_DRQ)))
    {
        udelay(1);
        ++i;

        if (i >= num_reties)
        {
            log_warn("[IDE] Resetting controller");

            reset_controller(device->controller);
            i = 0;
        }
    }

    outportb(iobase + ATA_DRV_HEAD,
             0xA0 | (device->lba << 6) | (device->position << 4));

    for (int i = 0; i < 4; ++i)
    {
        inportb(iobase + ATA_STATUS);
    }

    uint8_t status;

    while ((status = (inportb(iobase + ATA_STATUS) &
                      (ATA_STATUS_BSY | ATA_STATUS_DRQ))))
    {
        log_trace("[IDE] Status: %#02x", status);

        udelay(1);
    }

    return 1;
}

static void identify_ide_device(ide_device_t *device)
{
    int i;
    uint32_t iobase = device->controller->iobase;

    uint8_t status;
    uint8_t cl;
    uint8_t ch;
    uint8_t cmd;
    uint16_t info[256];

    device->present = 0;

    outportb(iobase + ATA_NSECTOR, 0xab);

    if (inportb(iobase + ATA_NSECTOR) != 0xab)
    {
        return;
    }

    int reset = reset_controller(device->controller);

    if (!reset)
    {
        return;
    }

    if (!select_device(device, 1))
    {
        return;
    }

    uint8_t res1 = inportb(iobase + ATA_NSECTOR);
    uint8_t res2 = inportb(iobase + ATA_SECTOR);

    if (res1 == 0x01 && res2 == 0x01)
    {
        cl = inportb(iobase + ATA_LCYL);
        ch = inportb(iobase + ATA_HCYL);

        status = inportb(iobase + ATA_STATUS);

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

    cmd = device->atapi ? ATAPI_IDENTIFY : ATA_IDENTIFY;

    outportb(iobase + ATA_COMMAND, cmd);

    udelay(1);

    if (!wait_for_controller(device->controller,
                             ATA_STATUS_BSY | ATA_STATUS_DRQ | ATA_STATUS_ERR,
                             ATA_STATUS_DRQ,
                             ATA_TIMEOUT))
    {
        device->present = 0;
        return;
    }

    for (i = 0; i < 256; ++i)
    {
        info[i] = inportw(iobase + ATA_DATA);
    }

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

    uint16_t *buf = (uint16_t *)buffer;
    uint8_t sc;
    uint8_t cl;
    uint8_t ch;
    uint8_t hd;
    uint8_t cmd;

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

    if (!select_device(device, 1))
    {
        log_error("[IDE] Could not select device");
        return 0;
    }

    if (device->lba)
    {
        sc = block & 0xFF;
        cl = (block >> 8) & 0xFF;
        ch = (block >> 16) & 0xFF;
        hd = (block >> 24) & 0xF;
    }
    else
    {
        int cyl = block / (device->heads * device->sectors);
        int tmp = block % (device->heads * device->sectors);

        sc = tmp % device->sectors + 1;
        cl = cyl & 0xFF;
        ch = (cyl >> 8) & 0xFF;
        hd = tmp / device->sectors;
    }

    cmd = (direction == IO_READ) ? ATA_READ_BLOCK : ATA_WRITE_BLOCK;

    controller->irq = 0;

    outportb(iobase + ATA_DRV_HEAD,
             0xE0 | (device->lba << 6) | (device->position << 4) | hd);
    outportb(iobase + ATA_ERROR, 0);
    outportb(iobase + ATA_NSECTOR, nblocks);
    outportb(iobase + ATA_SECTOR, sc);
    outportb(iobase + ATA_LCYL, cl);
    outportb(iobase + ATA_HCYL, ch);

    outportb(iobase + ATA_COMMAND, cmd);

    for (int i = 0; i < 4; ++i)
    {
        inportb(iobase + ATA_STATUS);
    }

    if (inportb(iobase + ATA_STATUS) & ATA_STATUS_ERR)
    {
        log_error("[IDE] Device reported error");
        return 0;
    }

    if (direction == IO_WRITE)
    {
        for (int block = 0; block < nblocks; ++block)
        {
            for (i = 0; i < 256; ++i)
            {
                outportw(iobase + ATA_DATA, *buf++);
            }

            while (!controller->irq)
                ;

            controller->irq = 0;
        }
    }

    if (direction == IO_READ)
    {
        for (int block = 0; block < nblocks; ++block)
        {
            while (!controller->irq)
                ;

            controller->irq = 0;

            for (i = 0; i < 256; ++i)
            {
                *buf++ = inportw(iobase + ATA_DATA);
            }
        }
    }

    if (inportb(iobase + ATA_STATUS) & ATA_STATUS_ERR)
    {
        log_error("[IDE] Device reported error");
        return 0;
    }

    if (direction == IO_WRITE)
    {
        // Send flush cache command
        outportb(iobase + ATA_COMMAND, 0xE7);

        if (!wait_for_controller(controller, ATA_STATUS_BSY, 0, ATA_TIMEOUT))
        {
            log_error(
                "[IDE] Error waiting for controller after sending flush "
                "command");
            return 0;
        }
    }

    return nblocks;
}

static uint32_t ide_write_blocks(uint32_t minor,
                                 uint32_t block,
                                 uint32_t nblocks,
                                 void *buffer)
{
    return ide_read_write_blocks(minor, block, nblocks, buffer, IO_WRITE);
}

static uint32_t ide_read_blocks(uint32_t minor,
                                uint32_t block,
                                uint32_t nblocks,
                                void *buffer)
{
    return ide_read_write_blocks(minor, block, nblocks, buffer, IO_READ);
}

static void ide_handle_interrupt(ide_controller_t *controller)
{
    // We must read the status register each irq
    volatile uint8_t dummy = inportb(controller->iobase + ATA_STATUS);

    controller->irq = 1;
}

static void ide_primary_irq(system_stack_t *regs)
{
    (void)regs;

    ide_handle_interrupt(&controllers[PRIMARY_IDE_CONTROLLER]);
}

static void ide_secondary_irq(system_stack_t *regs)
{
    (void)regs;

    ide_handle_interrupt(&controllers[SECONDARY_IDE_CONTROLLER]);
}

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

            if (!device->present || device->atapi)
            {
                continue;
            }

            sprintf(msg,
                    "%s [%i-%i]: %s (%i/%i/%i - %i sectors) LBA:%s - DMA:%s",
                    device->atapi ? "CD-ROM" : "Hard Disk",
                    i,
                    device->position,
                    device->model,
                    device->cylinders,
                    device->heads,
                    device->sectors,
                    device->capacity,
                    device->lba ? "YES" : "NO",
                    device->dma ? "YES" : "NO");

            reg_blockdev_instance(0,
                                  i * NUM_DEVICES_PER_CONTROLLER + j,
                                  msg,
                                  BLOCK_SIZE,
                                  device->capacity);

            if (i == 0 && j == 0)
            {
                select_device(device, 1);
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
