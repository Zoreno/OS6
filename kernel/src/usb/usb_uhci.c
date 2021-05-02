/**
 * @file usb_uhci.c
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-04-20
 *
 * @brief USB UHCI Host Controller Driver
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
#include <logging/logging.h>
#include <mm/virt_mem.h>
#include <pci/pci.h>
#include <pci/pci_device.h>
#include <pci/pci_io.h>
#include <usb/usb_controller.h>
#include <usb/usb_device.h>
#include <usb/usb_uhci.h>
#include <util/link.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//=============================================================================
// Limits
//=============================================================================

#define MAX_QH 8
#define MAX_TD 32

//=============================================================================
// UHCI Controller I/O Registers
//=============================================================================

#define REG_CMD 0x00        // USB Command
#define REG_STS 0x02        // USB Status
#define REG_INTR 0x04       // USB Interrupt Enable
#define REG_FRNUM 0x06      // Frame Number
#define REG_FRBASEADD 0x08  // Frame List Base Address
#define REG_SOFMOD 0x0C     // Start of Frame Modify
#define REG_PORT1 0x10      // Port 1 Status/Control
#define REG_PORT2 0x12      // Port 2 Status/Control
#define REG_LEGSUP 0xc0     // Legacy Support

//=============================================================================
// USB Command Register
//=============================================================================

#define CMD_RS (1 << 0)       // Run/Stop
#define CMD_HCRESET (1 << 1)  // Host Controller Reset
#define CMD_GRESET (1 << 2)   // Global Reset
#define CMD_EGSM (1 << 3)     // Enter Global Suspend Resume
#define CMD_FGR (1 << 4)      // Force Global Resume
#define CMD_SWDBG (1 << 5)    // Software Debug
#define CMD_CF (1 << 6)       // Configure Flag
#define CMD_MAXP (1 << 7)     // Max Packet (0 = 32, 1 = 64)

//=============================================================================
// USB Status Register
//=============================================================================

#define STS_USBINT (1 << 0)  // USB Interrupt
#define STS_ERROR (1 << 1)   // USB Error Interrupt
#define STS_RD (1 << 2)      // Resume Detect
#define STS_HSE (1 << 3)     // Host System Error
#define STS_HCPE (1 << 4)    // Host Controller Process Error
#define STS_HCH (1 << 5)     // HC Halted

//=============================================================================
// USB Interrupt Enable Register
//=============================================================================

#define INTR_TIMEOUT (1 << 0)  // Timeout/CRC Interrupt Enable
#define INTR_RESUME (1 << 1)   // Resume Interrupt Enable
#define INTR_IOC (1 << 2)      // Interrupt on Complete Enable
#define INTR_SP (1 << 3)       // Short Packet Interrupt Enable

//=============================================================================
// USB Port Status and Control Registers
//=============================================================================

#define PORT_CONNECTION (1 << 0)         // Current Connect Status
#define PORT_CONNECTION_CHANGE (1 << 1)  // Connect Status Change
#define PORT_ENABLE (1 << 2)             // Port Enabled
#define PORT_ENABLE_CHANGE (1 << 3)      // Port Enable Change
#define PORT_LS (3 << 4)                 // Line Status
#define PORT_RD (1 << 6)                 // Resume Detect
#define PORT_LSDA (1 << 8)               // Low Speed Device Attached
#define PORT_RESET (1 << 9)              // Port Reset
#define PORT_SUSP (1 << 12)              // Suspend
#define PORT_RWC (PORT_CONNECTION_CHANGE | PORT_ENABLE_CHANGE)

//=============================================================================
// USB Transfer descriptor
//=============================================================================

typedef struct _uhci_td_t
{
    volatile uint32_t link;
    volatile uint32_t cs;
    volatile uint32_t token;
    volatile uint32_t buffer;

    uint32_t td_next;
    uint8_t active;
    uint8_t pad[11];
} __attribute__((packed)) uhci_td_t;

//=============================================================================
// TD Link Pointer
//=============================================================================

#define TD_PTR_TERMINATE (1 << 0)
#define TD_PTR_QH (1 << 1)
#define TD_PTR_DEPTH (1 << 2)

//=============================================================================
// TD Control and Status
//=============================================================================

#define TD_CS_ACTLEN 0x000007ff
#define TD_CS_BITSTUFF (1 << 17)     // Bitstuff Error
#define TD_CS_CRC_TIMEOUT (1 << 18)  // CRC/Time Out Error
#define TD_CS_NAK (1 << 19)          // NAK Received
#define TD_CS_BABBLE (1 << 20)       // Babble Detected
#define TD_CS_DATABUFFER (1 << 21)   // Data Buffer Error
#define TD_CS_STALLED (1 << 22)      // Stalled
#define TD_CS_ACTIVE (1 << 23)       // Active
#define TD_CS_IOC (1 << 24)          // Interrupt on Complete
#define TD_CS_IOS (1 << 25)          // Isochronous Select
#define TD_CS_LOW_SPEED (1 << 26)    // Low Speed Device
#define TD_CS_ERROR_MASK (3 << 27)   // Error counter
#define TD_CS_ERROR_SHIFT 27
#define TD_CS_SPD (1 << 29)  // Short Packet Detect

//=============================================================================
// TD Token
//=============================================================================

#define TD_TOK_PID_MASK 0x000000ff      // Packet Identification
#define TD_TOK_DEVADDR_MASK 0x00007f00  // Device Address
#define TD_TOK_DEVADDR_SHIFT 8
#define TD_TOK_ENDP_MASK 0x00078000  // Endpoint
#define TD_TOK_ENDP_SHIFT 15
#define TD_TOK_D 0x00080000  // Data Toggle
#define TD_TOK_D_SHIFT 19
#define TD_TOK_MAXLEN_MASK 0xffe00000  // Maximum Length
#define TD_TOK_MAXLEN_SHIFT 21

//=============================================================================
// TD Packet
//=============================================================================

#define TD_PACKET_IN 0x69
#define TD_PACKET_OUT 0xe1
#define TD_PACKET_SETUP 0x2d

//=============================================================================
// USB Queue Head
//=============================================================================

typedef struct _uhci_qh_t
{
    volatile uint32_t head;
    volatile uint32_t element;

    usb_transfer_t *transfer;
    link_t qh_link;

    uint32_t td_head;
    uint32_t active;
} __attribute__((packed)) uhci_qh_t;

//=============================================================================
// USB UHCI Controller
//=============================================================================

typedef struct _uhci_controller_t
{
    uint32_t ioAddr;
    uint32_t *frame_list;
    uhci_qh_t *qh_pool;
    uhci_td_t *td_pool;
    uhci_qh_t *async_qh;
} uhci_controller_t;

//=============================================================================
// Alloc
//=============================================================================

static uhci_td_t *uhci_alloc_td(uhci_controller_t *controller)
{
    uhci_td_t *end = controller->td_pool + MAX_TD;

    for (uhci_td_t *td = controller->td_pool; td != end; ++td)
    {
        if (!td->active)
        {
            td->active = 1;
            return td;
        }
    }

    return NULL;
}

static uhci_qh_t *uhci_alloc_qh(uhci_controller_t *controller)
{
    uhci_qh_t *end = controller->qh_pool + MAX_QH;

    for (uhci_qh_t *qh = controller->qh_pool; qh != end; ++qh)
    {
        if (!qh->active)
        {
            qh->active = 1;
            return qh;
        }
    }

    return NULL;
}

static void uhci_free_td(uhci_td_t *td)
{
    td->active = 0;
}

static void uhci_free_qh(uhci_qh_t *qh)
{
    qh->active = 0;
}

static void uhci_insert_qh(uhci_controller_t *controller, uhci_qh_t *qh)
{
    uhci_qh_t *list = controller->async_qh;
    uhci_qh_t *end = link_data(list->qh_link.prev, uhci_qh_t, qh_link);

    qh->head = TD_PTR_TERMINATE;
    end->head = (uint32_t)(uintptr_t)qh | TD_PTR_QH;

    link_before(&list->qh_link, &qh->qh_link);
}

static void uhci_remove_qh(uhci_qh_t *qh)
{
    uhci_qh_t *prev = link_data(qh->qh_link.prev, uhci_qh_t, qh_link);

    prev->head = qh->head;
    link_remove(&qh->qh_link);
}

static void uhci_port_set(uint16_t port, uint16_t data)
{
    uint16_t status = inportw(port);
    status |= data;
    status &= ~PORT_RWC;
    outportw(port, status);
}

static void uhci_port_clr(uint32_t port, uint16_t data)
{
    uint16_t status = inportw(port);
    status &= ~PORT_RWC;
    status &= ~data;
    status |= PORT_RWC & data;
    outportw(port, status);
}

static void uhci_init_td(uhci_td_t *td,
                         uhci_td_t *prev,
                         uint32_t speed,
                         uint32_t addr,
                         uint32_t endp,
                         uint32_t toggle,
                         uint32_t packetType,
                         uint32_t len,
                         const void *data)
{
    len = (len - 1) & 0x7FF;

    if (prev)
    {
        prev->link = (uint32_t)(uintptr_t)td | TD_PTR_DEPTH;
        prev->td_next = (uint32_t)(uintptr_t)td;
    }

    td->link = TD_PTR_TERMINATE;
    td->td_next = 0;

    td->cs = (3 << TD_CS_ERROR_SHIFT) | TD_CS_ACTIVE;

    if (speed == USB_LOW_SPEED)
    {
        td->cs |= TD_CS_LOW_SPEED;
    }

    td->token = (len << TD_TOK_MAXLEN_SHIFT) | (toggle << TD_TOK_D_SHIFT) |
                (endp << TD_TOK_ENDP_SHIFT) | (addr << TD_TOK_DEVADDR_SHIFT) |
                packetType;

    td->buffer = (uint32_t)(uintptr_t)data;
}

static void uhci_init_qh(uhci_qh_t *qh, usb_transfer_t *t, uhci_td_t *td)
{
    qh->transfer = t;
    qh->td_head = (uint32_t)(uintptr_t)td;
    qh->element = (uint32_t)(uintptr_t)td;
}

static void uhci_process_qh(uhci_controller_t *controller, uhci_qh_t *qh)
{
    usb_transfer_t *t = qh->transfer;

    uhci_td_t *td = (uhci_td_t *)(uintptr_t)(qh->element & ~0xF);

    if (!td)
    {
        t->success = 1;
        t->complete = 1;
    }
    else if (~td->cs & TD_CS_ACTIVE)
    {
        if (td->cs & TD_CS_NAK)
        {
            log_error("[UCHI] NAK");
        }

        if (td->cs & TD_CS_STALLED)
        {
            log_error("[UHCI] TD is stalled");
            t->success = 0, t->complete = 1;
        }

        if (td->cs & TD_CS_DATABUFFER)
        {
            log_error("[UHCI] TD data buffer error");
        }

        if (td->cs & TD_CS_BABBLE)
        {
            log_error("[UHCI] TD babble error");
        }

        if (td->cs & TD_CS_CRC_TIMEOUT)
        {
            log_error("[UHCI] TD timeout error");
        }

        if (td->cs & TD_CS_BITSTUFF)
        {
            log_error("[UHCI] TD bitstuff error");
        }
    }

    if (t->complete)
    {
        qh->transfer = 0;

        if (t->success && t->endp)
        {
            t->endp->toggle ^= 1;
        }

        uhci_remove_qh(qh);

        while (td)
        {
            uhci_td_t *next = (uhci_td_t *)(uintptr_t)td->td_next;

            uhci_free_td(td);

            td = next;
        }

        uhci_free_qh(qh);
    }
}

static void uhci_wait_for_qh(uhci_controller_t *controller, uhci_qh_t *qh)
{
    usb_transfer_t *t = qh->transfer;

    while (!t->complete)
    {
        uhci_process_qh(controller, qh);
    }
}

static uint16_t uhci_reset_port(uhci_controller_t *controller, uint16_t port)
{
    uint16_t reg = REG_PORT1 + port * 2;

    uhci_port_set(controller->ioAddr + reg, PORT_RESET);
    mdelay(50);
    uhci_port_clr(controller->ioAddr + reg, PORT_RESET);

    uhci_port_set(controller->ioAddr + reg,
                  PORT_ENABLE | PORT_CONNECTION_CHANGE | PORT_ENABLE_CHANGE);

    uint16_t status = 0;

    for (uint32_t i = 0; i < 10; ++i)
    {
        mdelay(10);

        status = inportw(controller->ioAddr + reg);

        if (~status & PORT_CONNECTION)
        {
            log_warn("[UHCI] No device at port %i", port);
            break;
        }

        if (status & (PORT_ENABLE_CHANGE | PORT_CONNECTION_CHANGE))
        {
            log_warn("[UHCI] Port change ack %i", port);
            uhci_port_clr(controller->ioAddr + reg,
                          PORT_ENABLE_CHANGE | PORT_CONNECTION_CHANGE);
            continue;
        }

        if (status & PORT_ENABLE)
        {
            log_debug("[UHCI] Enabled port %i", port);
            break;
        }

        uhci_port_set(controller->ioAddr + reg, PORT_ENABLE);
    }

    return status;
}

static void uhci_dev_control(usb_device_t *dev, usb_transfer_t *t)
{
    uhci_controller_t *controller = (uhci_controller_t *)dev->hc;

    usb_dev_req_t *req = t->req;

    uint32_t speed = dev->speed;
    uint32_t addr = dev->addr;
    uint32_t endp = 0;
    uint32_t maxSize = dev->max_packet_size;
    uint32_t type = req->type;
    uint32_t len = req->len;

    uhci_td_t *td = uhci_alloc_td(controller);

    if (!td)
    {
        return;
    }

    uhci_td_t *head = td;
    uhci_td_t *prev = 0;

    uint32_t toggle = 0;
    uint32_t packetType = TD_PACKET_SETUP;
    uint32_t packetSize = sizeof(usb_dev_req_t);

    uhci_init_td(
        td, prev, speed, addr, endp, toggle, packetType, packetSize, req);
    prev = td;

    packetType = type & RT_DEV_TO_HOST ? TD_PACKET_IN : TD_PACKET_OUT;

    uint8_t *it = (uint8_t *)t->data;
    uint8_t *end = it + len;

    while (it < end)
    {
        td = uhci_alloc_td(controller);

        if (!td)
        {
            return;
        }

        toggle ^= 1;
        packetSize = end - it;

        if (packetSize > maxSize)
        {
            packetSize = maxSize;
        }

        uhci_init_td(
            td, prev, speed, addr, endp, toggle, packetType, packetSize, it);

        it += packetSize;
        prev = td;
    }

    td = uhci_alloc_td(controller);

    if (!td)
    {
        return;
    }

    toggle = 1;

    packetType = type & RT_DEV_TO_HOST ? TD_PACKET_OUT : TD_PACKET_IN;

    uhci_init_td(td, prev, speed, addr, endp, toggle, packetType, 0, 0);

    uhci_qh_t *qh = uhci_alloc_qh(controller);

    uhci_init_qh(qh, t, head);

    uhci_insert_qh(controller, qh);

    uhci_wait_for_qh(controller, qh);
}

static void uhci_dev_intr(usb_device_t *dev, usb_transfer_t *t)
{
    uhci_controller_t *controller = (uhci_controller_t *)dev->hc;

    uint32_t speed = dev->speed;
    uint32_t addr = dev->addr;
    uint32_t endp = dev->endp.desc.addr & 0xF;

    uhci_td_t *td = uhci_alloc_td(controller);

    if (!td)
    {
        t->success = 0;
        t->complete = 1;
        return;
    }

    uhci_td_t *head = td;
    uhci_td_t *prev = 0;

    uint32_t toggle = dev->endp.toggle;
    uint32_t packetType = TD_PACKET_IN;
    uint32_t packetSize = t->len;

    uhci_init_td(
        td, prev, speed, addr, endp, toggle, packetType, packetSize, t->data);

    uhci_qh_t *qh = uhci_alloc_qh(controller);

    uhci_init_qh(qh, t, head);

    uhci_insert_qh(controller, qh);
}

static void uhci_probe(uhci_controller_t *controller)
{
    uint16_t port_count = 2;
    for (uint16_t port = 0; port < port_count; ++port)
    {
        uint16_t status = uhci_reset_port(controller, port);

        if (status & PORT_ENABLE)
        {
            uint32_t speed =
                (status & PORT_LSDA) ? USB_LOW_SPEED : USB_FULL_SPEED;

            usb_device_t *dev = usb_dev_create();

            if (dev)
            {
                dev->parent = 0;
                dev->hc = controller;
                dev->port = port;
                dev->speed = speed;
                dev->max_packet_size = 8;

                dev->hc_control = uhci_dev_control;
                dev->hc_intr = uhci_dev_intr;

                backtrace();

                if (!usb_dev_init(dev))
                {
                }
            }
        }
    }
}

static void uhci_controller_poll(usb_controller_t *controller)
{
    uhci_controller_t *hc = (uhci_controller_t *)controller->hc;

    uhci_qh_t *qh;
    uhci_qh_t *next;

    list_for_each_safe(qh, next, hc->async_qh->qh_link, qh_link, uhci_qh_t)
    {
        if (qh->transfer)
        {
            uhci_process_qh(hc, qh);
        }
    }
}

void usb_uhci_init(uint32_t id, PciDeviceInfo_t *devInfo)
{
    if (!((((devInfo->classCode << 8) | devInfo->subClass) == PCI_SERIAL_USB) &&
          (devInfo->progIntf == PCI_SERIAL_USB_UHCI)))
    {
        return;
    }

    return;

    log_info("[UHCI] Initializing UHCI");

    PciBAR_t *bar = (PciBAR_t *)&devInfo->type0.BaseAddresses[4];

    uint16_t ioAddr = bar->port;

    uhci_controller_t *hc = malloc(sizeof(uhci_controller_t));

    if (!hc)
    {
        log_error("[UHCI] Failed to allocate memory for host controller");
        return;
    }

    hc->ioAddr = ioAddr;

    hc->frame_list = malloc(1024 * sizeof(uint32_t));

    log_debug("[UHCI] Framelist: 0x%016x", hc->frame_list);

    if (!hc->frame_list)
    {
        log_error("[UCHI] Failed to allocate memory for framelist");
        return;
    }

    hc->qh_pool = malloc(sizeof(uhci_qh_t) * MAX_QH);

    if (!hc->qh_pool)
    {
        log_error("[UCHI] Failed to allocate memory for QH pool");
        return;
    }

    hc->td_pool = malloc(sizeof(uhci_td_t) * MAX_TD);

    if (!hc->td_pool)
    {
        log_error("[UCHI] Failed to allocate memory for TD pool");
        return;
    }

    memset(hc->qh_pool, 0, sizeof(uhci_qh_t) * MAX_QH);
    memset(hc->td_pool, 0, sizeof(uhci_td_t) * MAX_TD);

    outportw(hc->ioAddr + REG_CMD, CMD_GRESET);
    mdelay(50);
    outportw(hc->ioAddr + REG_CMD, 0);

    uhci_qh_t *qh = uhci_alloc_qh(hc);

    if (!qh)
    {
        log_error("[UHCI] Failed to allocate memory for QH");
    }

    qh->head = TD_PTR_TERMINATE;
    qh->element = TD_PTR_TERMINATE;
    qh->transfer = 0;
    qh->qh_link.prev = &qh->qh_link;
    qh->qh_link.next = &qh->qh_link;

    hc->async_qh = qh;

    for (uint32_t i = 0; i < 1024; ++i)
    {
        hc->frame_list[i] = TD_PTR_QH | (uint32_t)(uintptr_t)qh;
    }

    outportw(hc->ioAddr + REG_LEGSUP, 0x8F00);
    outportw(hc->ioAddr + REG_INTR, 0);
    outportw(hc->ioAddr + REG_FRNUM, 0);
    outportl(hc->ioAddr + REG_FRBASEADD,
             (uint32_t)((uint64_t)(virt_mem_get_physical_addr(
                            hc->frame_list, virt_mem_get_current_dir())) &
                        0xFFFFFFFF));
    outportw(hc->ioAddr + REG_SOFMOD, 0x40);
    outportw(hc->ioAddr + REG_STS, 0xFFFF);
    outportw(hc->ioAddr + REG_CMD, CMD_RS | CMD_CF | CMD_MAXP);
    outportw(hc->ioAddr + REG_CMD, CMD_RS | CMD_CF | CMD_MAXP | CMD_FGR);

    mdelay(20);

    outportw(hc->ioAddr + REG_CMD, CMD_RS | CMD_CF | CMD_MAXP);

    mdelay(100);

    uhci_probe(hc);

    usb_controller_t *controller = malloc(sizeof(usb_controller_t));

    if (!controller)
    {
        log_error("[UHCI] Failed to allocate memory for USB controller");
        return;
    }

    controller->next = usb_get_controller_list();
    controller->hc = hc;
    controller->poll = uhci_controller_poll;

    usb_set_controller_list(controller);

    log_info("[UHCI] Done");
}

//=============================================================================
// End of file
//=============================================================================
