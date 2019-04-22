#include <usb/usb_ehci.h>

#include <usb/usb_device.h>
#include <usb/usb_controller.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <util/link.h>

#include <arch/arch.h>

#include <pci/pci_device.h>
#include <pci/pci.h>
#include <pci/pci_io.h>

#include <debug/backtrace.h>

#include <mm/virt_mem.h>
#include <mm/phys_mem.h>
#include <mm/kheap.h>

#define MAX_QH 8
#define MAX_TD 32

#define USBLEGSUP 0x00    // USB Legacy Support Extended Capability
#define UBSLEGCTLSTS 0x04 // USB Legacy Support Control/Status

#define USBLEGSUP_HC_OS 0x01000000    // HC OS Owned Semaphore
#define USBLEGSUP_HC_BIOS 0x00010000  // HC BIOS Owned Semaphore
#define USBLEGSUP_NCP_MASK 0x0000ff00 // Next EHCI Extended Capability Pointer
#define USBLEGSUP_CAPID 0x000000ff    // Capability ID

typedef struct _ehci_cap_regs_t
{
    uint8_t capLength;
    uint8_t reserved;
    uint16_t hciVersion;
    uint32_t hcsParams;
    uint32_t hccParams;
    uint64_t hcspPortRoute;

} __attribute__((packed)) ehci_cap_regs_t;

#define HCSPARAMS_N_PORTS_MASK (15 << 0) // Number of Ports
#define HCSPARAMS_PPC (1 << 4)           // Port Power Control
#define HCSPARAMS_PORT_ROUTE (1 << 7)    // Port Routing Rules
#define HCSPARAMS_N_PCC_MASK (15 << 8)   // Number of Ports per Companion Controller
#define HCSPARAMS_N_PCC_SHIFT 8
#define HCSPARAMS_N_CC_MASK (15 << 12) // Number of Companion Controllers
#define HCSPARAMS_N_CC_SHIFT 12
#define HCSPARAMS_P_INDICATOR (1 << 16) // Port Indicator
#define HCSPARAMS_DPN_MASK (15 << 20)   // Debug Port Number
#define HCSPARAMS_DPN_SHIFT 20

#define HCCPARAMS_64_BIT (1 << 0)      // 64-bit Addressing Capability
#define HCCPARAMS_PFLF (1 << 1)        // Programmable Frame List Flag
#define HCCPARAMS_ASPC (1 << 2)        // Asynchronous Schedule Park Capability
#define HCCPARAMS_IST_MASK (15 << 4)   // Isochronous Sheduling Threshold
#define HCCPARAMS_EECP_MASK (255 << 8) // EHCI Extended Capabilities Pointer
#define HCCPARAMS_EECP_SHIFT 8

typedef struct _ehci_op_regs_t
{
    volatile uint32_t usbCmd;
    volatile uint32_t usbSts;
    volatile uint32_t usbIntr;
    volatile uint32_t frameIndex;
    volatile uint32_t ctrlDsSegment;
    volatile uint32_t periodicListBase;
    volatile uint32_t asyncListAddr;
    volatile uint32_t reserved[9];
    volatile uint32_t configFlag;
    volatile uint32_t ports[];

} __attribute__((packed)) ehci_op_regs_t;

#define CMD_RS (1 << 0)       // Run/Stop
#define CMD_HCRESET (1 << 1)  // Host Controller Reset
#define CMD_FLS_MASK (3 << 2) // Frame List Size
#define CMD_FLS_SHIFT 2
#define CMD_PSE (1 << 4)        // Periodic Schedule Enable
#define CMD_ASE (1 << 5)        // Asynchronous Schedule Enable
#define CMD_IOAAD (1 << 6)      // Interrupt on Async Advance Doorbell
#define CMD_LHCR (1 << 7)       // Light Host Controller Reset
#define CMD_ASPMC_MASK (3 << 8) // Asynchronous Schedule Park Mode Count
#define CMD_ASPMC_SHIFT 8
#define CMD_ASPME (1 << 11)      // Asynchronous Schedule Park Mode Enable
#define CMD_ITC_MASK (255 << 16) // Interrupt Threshold Control
#define CMD_ITC_SHIFT 16

#define STS_USBINT (1 << 0)       // USB Interrupt
#define STS_ERROR (1 << 1)        // USB Error Interrupt
#define STS_PCD (1 << 2)          // Port Change Detect
#define STS_FLR (1 << 3)          // Frame List Rollover
#define STS_HSE (1 << 4)          // Host System Error
#define STS_IOAA (1 << 5)         // Interrupt on Async Advance
#define STS_HCHALTED (1 << 12)    // Host Controller Halted
#define STS_RECLAMATION (1 << 13) // Reclamation
#define STS_PSS (1 << 14)         // Periodic Schedule Status
#define STS_ASS (1 << 15)         // Asynchronous Schedule Status

#define INTR_USBINT (1 << 0) // USB Interrupt Enable
#define INTR_ERROR (1 << 1)  // USB Error Interrupt Enable
#define INTR_PCD (1 << 2)    // Port Change Interrupt Enable
#define INTR_FLR (1 << 3)    // Frame List Rollover Enable
#define INTR_HSE (1 << 4)    // Host System Error Enable
#define INTR_IOAA (1 << 5)   // Interrupt on Async Advance Enable

#define FR_MASK 0x3fff

#define CF_PORT_ROUTE (1 << 0) // Configure Flag (CF)

#define PORT_CONNECTION (1 << 0)          // Current Connect Status
#define PORT_CONNECTION_CHANGE (1 << 1)   // Connect Status Change
#define PORT_ENABLE (1 << 2)              // Port Enabled
#define PORT_ENABLE_CHANGE (1 << 3)       // Port Enable Change
#define PORT_OVER_CURRENT (1 << 4)        // Over-current Active
#define PORT_OVER_CURRENT_CHANGE (1 << 5) // Over-current Change
#define PORT_FPR (1 << 6)                 // Force Port Resume
#define PORT_SUSPEND (1 << 7)             // Suspend
#define PORT_RESET (1 << 8)               // Port Reset
#define PORT_LS_MASK (3 << 10)            // Line Status
#define PORT_LS_SHIFT 10
#define PORT_POWER (1 << 12)   // Port Power
#define PORT_OWNER (1 << 13)   // Port Owner
#define PORT_IC_MASK (3 << 14) // Port Indicator Control
#define PORT_IC_SHIFT 14
#define PORT_TC_MASK (15 << 16) // Port Test Control
#define PORT_TC_SHIFT 16
#define PORT_WKCNNT_E (1 << 20)   // Wake on Connect Enable
#define PORT_WKDSCNNT_E (1 << 21) // Wake on Disconnect Enable
#define PORT_WKOC_E (1 << 22)     // Wake on Over-current Enable
#define PORT_RWC (PORT_CONNECTION_CHANGE | PORT_ENABLE_CHANGE | PORT_OVER_CURRENT_CHANGE)

typedef struct _ehci_td_t
{
    volatile uint32_t link;
    volatile uint32_t altLink;
    volatile uint32_t token;
    volatile uint32_t buffer[5];
    volatile uint32_t extBuffer[5];

    // internal fields
    uint32_t tdNext;
    uint32_t active;
    uint8_t pad[4];

} __attribute__((packed)) ehci_td_t;

// TD Link Pointer
#define PTR_TERMINATE (1 << 0)

#define PTR_TYPE_MASK (3 << 1)
#define PTR_ITD (0 << 1)
#define PTR_QH (1 << 1)
#define PTR_SITD (2 << 1)
#define PTR_FSTN (3 << 1)

// TD Token
#define TD_TOK_PING (1 << 0)       // Ping State
#define TD_TOK_STS (1 << 1)        // Split Transaction State
#define TD_TOK_MMF (1 << 2)        // Missed Micro-Frame
#define TD_TOK_XACT (1 << 3)       // Transaction Error
#define TD_TOK_BABBLE (1 << 4)     // Babble Detected
#define TD_TOK_DATABUFFER (1 << 5) // Data Buffer Error
#define TD_TOK_HALTED (1 << 6)     // Halted
#define TD_TOK_ACTIVE (1 << 7)     // Active
#define TD_TOK_PID_MASK (3 << 8)   // PID Code
#define TD_TOK_PID_SHIFT 8
#define TD_TOK_CERR_MASK (3 << 10) // Error Counter
#define TD_TOK_CERR_SHIFT 10
#define TD_TOK_C_PAGE_MASK (7 << 12) // Current Page
#define TD_TOK_C_PAGE_SHIFT 12
#define TD_TOK_IOC (1 << 15)       // Interrupt on Complete
#define TD_TOK_LEN_MASK 0x7fff0000 // Total Bytes to Transfer
#define TD_TOK_LEN_SHIFT 16
#define TD_TOK_D (1 << 31) // Data Toggle
#define TD_TOK_D_SHIFT 31

#define USB_PACKET_OUT 0   // token 0xe1
#define USB_PACKET_IN 1    // token 0x69
#define USB_PACKET_SETUP 2 // token 0x2d

typedef struct _ehci_qh_t
{
    volatile uint32_t qhlp; // Queue Head Horizontal Link Pointer
    volatile uint32_t ch;   // Endpoint Characteristics
    volatile uint32_t caps; // Endpoint Capabilities
    volatile uint32_t curLink;

    // matches a transfer descriptor
    volatile uint32_t nextLink;
    volatile uint32_t altLink;
    volatile uint32_t token;
    volatile uint32_t buffer[5];
    volatile uint32_t extBuffer[5];

    // internal fields
    usb_transfer_t *transfer;
    link_t qhLink;
    uint32_t tdHead;
    uint32_t active;
    uint8_t pad[20];
} __attribute__((packed)) ehci_qh_t;

// Endpoint Characteristics
#define QH_CH_DEVADDR_MASK 0x0000007f // Device Address
#define QH_CH_INACTIVE 0x00000080     // Inactive on Next Transaction
#define QH_CH_ENDP_MASK 0x00000f00    // Endpoint Number
#define QH_CH_ENDP_SHIFT 8
#define QH_CH_EPS_MASK 0x00003000 // Endpoint Speed
#define QH_CH_EPS_SHIFT 12
#define QH_CH_DTC 0x00004000      // Data Toggle Control
#define QH_CH_H 0x00008000        // Head of Reclamation List Flag
#define QH_CH_MPL_MASK 0x07ff0000 // Maximum Packet Length
#define QH_CH_MPL_SHIFT 16
#define QH_CH_CONTROL 0x08000000     // Control Endpoint Flag
#define QH_CH_NAK_RL_MASK 0xf0000000 // Nak Count Reload
#define QH_CH_NAK_RL_SHIFT 28

// Endpoint Capabilities
#define QH_CAP_INT_SCHED_SHIFT 0
#define QH_CAP_INT_SCHED_MASK 0x000000ff // Interrupt Schedule Mask
#define QH_CAP_SPLIT_C_SHIFT 8
#define QH_CAP_SPLIT_C_MASK 0x0000ff00 // Split Completion Mask
#define QH_CAP_HUB_ADDR_SHIFT 16
#define QH_CAP_HUB_ADDR_MASK 0x007f0000 // Hub Address
#define QH_CAP_PORT_MASK 0x3f800000     // Port Number
#define QH_CAP_PORT_SHIFT 23
#define QH_CAP_MULT_MASK 0xc0000000 // High-Bandwidth Pipe Multiplier
#define QH_CAP_MULT_SHIFT 30

static void ehci_print_queue_head(ehci_qh_t *qh)
{
    printf("=========================================================================\n");
    printf("[EHCI] Queue Head %#08x\n\n", qh);

    printf("Queue Head Horizontal pointer: %#08x\n", qh->qhlp & ~0xF);
    printf("Type: %i\n", (qh->qhlp & 0b110) >> 1);
    printf("Terminate: %i\n\n", qh->qhlp & 0b1);

    printf("NAK Count reload: %i\n", (qh->ch & QH_CH_NAK_RL_MASK) >> QH_CH_NAK_RL_SHIFT);
    printf("Control endpoint: %i\n", (qh->ch & QH_CH_CONTROL) > 0 ? 1 : 0);
    printf("Max packet length: %i\n", (qh->ch & QH_CH_MPL_MASK) >> QH_CH_MPL_SHIFT);
    printf("Head of reclamation list: %i\n", (qh->ch & QH_CH_H) > 0 ? 1 : 0);
    printf("Data toggle control: %i\n", (qh->ch & QH_CH_DTC) > 0 ? 1 : 0);
    printf("Endpoint speed: %i\n", (qh->ch & QH_CH_ENDP_MASK) >> QH_CH_ENDP_SHIFT);
    printf("Inactive on next transaction: %i\n", (qh->ch & QH_CH_INACTIVE) > 0 ? 1 : 0);
    printf("Device address: %i\n\n", (qh->ch & QH_CH_DEVADDR_MASK));

    printf("High BW Multiplier: %i\n", (qh->ch & QH_CAP_MULT_MASK) >> QH_CAP_MULT_SHIFT);
    printf("Port number: %i\n", (qh->ch & QH_CAP_PORT_MASK) >> QH_CAP_PORT_SHIFT);
    printf("Hub addr: %i\n", (qh->ch & QH_CAP_HUB_ADDR_MASK) >> QH_CAP_HUB_ADDR_SHIFT);
    printf("Split completion: %i\n", (qh->ch & QH_CAP_SPLIT_C_MASK) >> QH_CAP_SPLIT_C_SHIFT);
    printf("Interrupt schedule mask: %i\n\n", (qh->ch & QH_CAP_INT_SCHED_MASK) >> QH_CAP_INT_SCHED_SHIFT);

    printf("Ping: %i\n", (qh->token & TD_TOK_PING) > 0 ? 1 : 0);
    printf("STS: %i\n", (qh->token & TD_TOK_STS) > 0 ? 1 : 0);
    printf("MMF: %i\n", (qh->token & TD_TOK_MMF) > 0 ? 1 : 0);
    printf("XACT: %i\n", (qh->token & TD_TOK_XACT) > 0 ? 1 : 0);
    printf("Babble: %i\n", (qh->token & TD_TOK_BABBLE) > 0 ? 1 : 0);
    printf("Databuffer: %i\n", (qh->token & TD_TOK_DATABUFFER) > 0 ? 1 : 0);
    printf("Halted: %i\n", (qh->token & TD_TOK_HALTED) > 0 ? 1 : 0);
    printf("Active: %i\n", (qh->token & TD_TOK_ACTIVE) > 0 ? 1 : 0);
    printf("PID: %i\n", (qh->token & TD_TOK_PID_MASK) >> TD_TOK_PID_SHIFT);
    printf("Cerr: %i\n", (qh->token & TD_TOK_CERR_MASK) >> TD_TOK_CERR_SHIFT);
    printf("Len: %i\n", (qh->token & TD_TOK_LEN_MASK) >> TD_TOK_LEN_SHIFT);
    printf("=========================================================================\n");
}

typedef struct _ehci_controller_t
{
    ehci_cap_regs_t *capRegs;
    ehci_op_regs_t *opRegs;
    uint32_t *frameList;
    ehci_qh_t *qhPool;
    ehci_td_t *tdPool;
    ehci_qh_t *asyncQH;
    ehci_qh_t *periodicQH;

} ehci_controller_t;

static ehci_td_t *ehci_alloc_td(ehci_controller_t *hc)
{
    ehci_td_t *end = hc->tdPool + MAX_TD;

    for (ehci_td_t *td = hc->tdPool; td != end; ++td)
    {
        if (!td->active)
        {
            td->active = 1;
            return td;
        }
    }

    return NULL;
}

static ehci_qh_t *ehci_alloc_qh(ehci_controller_t *hc)
{
    ehci_qh_t *end = hc->qhPool + MAX_QH;

    for (ehci_qh_t *qh = hc->qhPool; qh != end; ++qh)
    {
        if (!qh->active)
        {
            qh->active = 1;
            return qh;
        }
    }

    return NULL;
}

static void ehci_free_td(ehci_td_t *td)
{
    td->active = 0;
}

static void ehci_free_qh(ehci_qh_t *qh)
{
    qh->active = 0;
}

static void ehci_insert_async_qh(ehci_qh_t *list, ehci_qh_t *qh)
{
    printf("[EHCI] Inserting async QH\n");
    ehci_qh_t *end = link_data(list->qhLink.prev, ehci_qh_t, qhLink);

    qh->qhlp = (uint32_t)(uintptr_t)list | PTR_QH;
    end->qhlp = (uint32_t)(uintptr_t)qh | PTR_QH;

    link_before(&list->qhLink, &qh->qhLink);

    ehci_print_queue_head(qh);
    ehci_print_queue_head(end);
}

static void ehci_insert_periodic_qh(ehci_qh_t *list, ehci_qh_t *qh)
{
    printf("[EHCI] Inserting periodic QH\n");

    ehci_qh_t *end = link_data(list->qhLink.prev, ehci_qh_t, qhLink);

    qh->qhlp = PTR_TERMINATE;
    end->qhlp = (uint32_t)(uintptr_t)qh | PTR_QH;

    link_before(&list->qhLink, &qh->qhLink);
}

static void ehci_remove_qh(ehci_qh_t *qh)
{
    printf("[EHCI] Removing QH\n");

    ehci_qh_t *prev = link_data(qh->qhLink.prev, ehci_qh_t, qhLink);

    prev->qhlp = qh->qhlp;

    link_remove(&qh->qhLink);
}

static void ehci_port_set(volatile uint32_t *portReg, uint32_t data)
{
    uint32_t status = *portReg;
    status |= data;
    status &= ~PORT_RWC;
    *portReg = status;
}

static void ehci_port_clr(volatile uint32_t *portReg, uint32_t data)
{
    uint32_t status = *portReg;
    status &= ~PORT_RWC;
    status &= ~data;
    status |= PORT_RWC & data;
    *portReg = status;
}

static void ehci_init_td(ehci_td_t *td,
                         ehci_td_t *prev,
                         uint32_t toggle,
                         uint32_t packetType,
                         uint32_t len,
                         const void *data)
{
    printf("[EHCI] Creating Transfer descriptor\n");

    if (prev)
    {
        prev->link = (uint32_t)(uintptr_t)td;
        prev->tdNext = (uint32_t)(uintptr_t)td;
    }

    td->link = PTR_TERMINATE;
    td->altLink = PTR_TERMINATE;
    td->tdNext = 0;

    td->token = (toggle << TD_TOK_D_SHIFT) |
                (len << TD_TOK_LEN_SHIFT) |
                (3 << TD_TOK_CERR_SHIFT) |
                (packetType << TD_TOK_PID_SHIFT) |
                TD_TOK_ACTIVE;

    uintptr_t p = (uintptr_t)data;

    td->buffer[0] = (uint32_t)p;
    td->extBuffer[0] = (uint32_t)(p >> 32);
    p &= ~0xFFF;

    for (uint32_t i = 1; i < 4; ++i)
    {
        p += 0x1000;
        td->buffer[i] = (uint32_t)(p);
        td->extBuffer[i] = (uint32_t)((uint64_t)p >> 32);
    }
}

static void ehci_init_qh(ehci_qh_t *qh,
                         usb_transfer_t *t,
                         ehci_td_t *td,
                         usb_device_t *parent,
                         int interrupt,
                         uint32_t speed,
                         uint32_t addr,
                         uint32_t endp,
                         uint32_t maxSize)
{
    printf("[EHCI] Creating Queue Head\n");

    qh->transfer = t;

    uint32_t ch =
        (maxSize << QH_CH_MPL_SHIFT) |
        QH_CH_DTC |
        (speed << QH_CH_EPS_SHIFT) |
        (endp << QH_CH_ENDP_SHIFT) |
        addr;

    uint32_t caps =
        (1 << QH_CAP_MULT_SHIFT);

    if (!interrupt)
    {
        ch |= 5 << QH_CH_NAK_RL_SHIFT;
    }

    if (speed != USB_HIGH_SPEED && parent)
    {
        if (interrupt)
        {
            caps |= (0x1C << QH_CAP_SPLIT_C_SHIFT);
        }
        else
        {
            ch |= QH_CH_CONTROL;
        }

        caps |=
            (parent->port << QH_CAP_PORT_SHIFT) |
            (parent->addr << QH_CAP_HUB_ADDR_SHIFT);
    }

    if (interrupt)
    {
        caps |= (0x01 << QH_CAP_INT_SCHED_SHIFT);
    }

    qh->ch = ch;
    qh->caps = caps;

    qh->tdHead = (uint32_t)(uintptr_t)td;
    qh->nextLink = (uint32_t)(uintptr_t)td;

    qh->token = 0;
}

static void ehci_process_qh(ehci_controller_t *hc, ehci_qh_t *qh)
{
    usb_transfer_t *t = qh->transfer;

    if (qh->token & TD_TOK_HALTED)
    {
        printf("[EHCI] TD_TOK_HALTED\n");

        t->success = 0;
        t->complete = 1;
    }
    else if (qh->nextLink & PTR_TERMINATE)
    {
        printf("[EHCI] PTR_TERMINATE\n");

        if (~qh->token & TD_TOK_ACTIVE)
        {
            if (qh->token & TD_TOK_DATABUFFER)
            {
                printf("[EHCI] Error: Data buffer\n");
            }

            if (qh->token & TD_TOK_BABBLE)
            {
                printf("[EHCI] Error: Babble\n");
            }

            if (qh->token & TD_TOK_XACT)
            {
                printf("[EHCI] Error: Transaction error\n");
            }

            if (qh->token & TD_TOK_MMF)
            {
                printf("[EHCI] Error: Missed Micro-frame\n");
            }

            t->success = 1;
            t->complete = 1;
        }
    }

    if (t->complete)
    {
        printf("[EHCI] Complete\n");
        qh->transfer = 0;

        if (t->success && t->endp)
        {
            t->endp->toggle ^= 1;
        }

        ehci_remove_qh(qh);

        ehci_td_t *td = (ehci_td_t *)(uintptr_t)qh->tdHead;

        while (td)
        {
            ehci_td_t *next = (ehci_td_t *)(uintptr_t)td->tdNext;
            ehci_free_td(td);
            td = next;
        }

        ehci_free_qh(qh);
    }
}

static void ehci_wait_for_qh(ehci_controller_t *hc, ehci_qh_t *qh)
{
    printf("[EHCI] Waiting for QH\n");

    //backtrace();

    usb_transfer_t *t = qh->transfer;

    while (!t->complete)
    {
        ehci_process_qh(hc, qh);
    }
}

static uint32_t ehci_port_reset(ehci_controller_t *hc, uint32_t port)
{
    volatile uint32_t *reg = &hc->opRegs->ports[port];

    //ehci_port_set(reg, PORT_POWER);
    //ehci_port_clr(reg, PORT_ENABLE);

    ehci_port_set(reg, PORT_RESET);
    mdelay(50);
    ehci_port_clr(reg, PORT_RESET);

    uint32_t status = 0;

    for (uint32_t i = 0; i < 10; ++i)
    {
        mdelay(10);

        status = *reg;

        if (~status & PORT_CONNECTION)
        {
            printf("No connection at port %i\n", port);
            break;
        }

        if (status & (PORT_ENABLE_CHANGE | PORT_CONNECTION_CHANGE))
        {
            printf("Change ack at port %i\n", port);
            ehci_port_clr(reg, PORT_ENABLE_CHANGE | PORT_CONNECTION_CHANGE);
            continue;
        }

        if (status & PORT_ENABLE)
        {
            printf("Port %i enabled\n", port);
            break;
        }
    }

    return status;
}

static void ehci_dev_control(usb_device_t *dev, usb_transfer_t *t)
{
    printf("[EHCI] dev control\n");

    ehci_controller_t *hc = (ehci_controller_t *)dev->hc;

    usb_dev_req_t *req = t->req;

    uint32_t speed = dev->speed;
    uint32_t addr = dev->addr;
    uint32_t maxSize = dev->maxPacketSize;
    uint32_t type = req->type;
    uint32_t len = req->len;

    ehci_td_t *td = ehci_alloc_td(hc);

    if (!td)
    {
        backtrace();
        return;
    }

    ehci_td_t *head = td;
    ehci_td_t *prev = 0;

    uint32_t toggle = 0;
    uint32_t packetType = USB_PACKET_SETUP;
    uint32_t packetSize = sizeof(usb_dev_req_t);

    ehci_init_td(td, prev, toggle, packetType, packetSize, req);
    prev = td;

    packetType = type & RT_DEV_TO_HOST ? USB_PACKET_IN : USB_PACKET_OUT;

    uint8_t *it = (uint8_t *)t->data;
    uint8_t *end = it + len;

    while (it < end)
    {
        td = ehci_alloc_td(hc);

        if (!td)
        {
            backtrace();
            return;
        }

        toggle ^= 1;
        packetSize = end - it;

        if (packetSize > maxSize)
        {
            packetSize = maxSize;
        }

        ehci_init_td(td, prev, toggle, packetType, packetSize, it);

        it += packetSize;

        prev = td;
    }

    td = ehci_alloc_td(hc);

    if (!td)
    {
        backtrace();
        return;
    }

    toggle = 1;

    packetType = type & RT_DEV_TO_HOST ? USB_PACKET_OUT : USB_PACKET_IN;

    ehci_init_td(td, prev, toggle, packetType, 0, 0);

    ehci_qh_t *qh = ehci_alloc_qh(hc);

    ehci_init_qh(qh, t, head, dev->parent, 0, speed, addr, 0, maxSize);

    ehci_insert_async_qh(hc->asyncQH, qh);

    ehci_wait_for_qh(hc, qh);
}

static void ehci_dev_intr(usb_device_t *dev, usb_transfer_t *t)
{
    backtrace();

    ehci_controller_t *hc = (ehci_controller_t *)dev->hc;

    uint32_t speed = dev->speed;
    uint32_t addr = dev->addr;
    uint32_t maxSize = dev->maxPacketSize;
    uint32_t endp = dev->endp.desc.addr & 0xF;

    ehci_td_t *td = ehci_alloc_td(hc);

    if (!td)
    {
        t->success = 0;
        t->complete = 1;

        return;
    }

    ehci_td_t *head = td;
    ehci_td_t *prev = 0;

    uint32_t toggle = dev->endp.toggle;
    uint32_t packetType = USB_PACKET_IN;
    uint32_t packetSize = t->len;

    ehci_init_td(td, prev, toggle, packetType, packetSize, t->data);

    ehci_qh_t *qh = ehci_alloc_qh(hc);
    ehci_init_qh(qh, t, head, dev->parent, 1, speed, addr, endp, maxSize);

    ehci_insert_periodic_qh(hc->periodicQH, qh);
}

static void ehci_probe(ehci_controller_t *hc)
{
    uint32_t portCount = hc->capRegs->hcsParams & HCSPARAMS_N_PORTS_MASK;

    for (uint32_t port = 0; port < portCount; ++port)
    {
        uint32_t status = ehci_port_reset(hc, port);

        if (status & PORT_ENABLE)
        {
            uint32_t speed = USB_HIGH_SPEED;

            printf("Allocating device for port %i\n", port);

            usb_device_t *dev = usb_dev_create();

            if (dev)
            {
                dev->parent = 0;
                dev->hc = hc;
                dev->port = port;
                dev->speed = speed;
                dev->maxPacketSize = 8;

                dev->hcControl = ehci_dev_control;
                dev->hcIntr = ehci_dev_intr;

                if (!usb_dev_init(dev))
                {
                    printf("USB device failed to initiate for %i\n", port);
                }
            }
        }
    }
}

static void ehci_controller_poll_list(ehci_controller_t *hc, link_t *list)
{
    ehci_qh_t *qh;
    ehci_qh_t *next;

    list_for_each_safe(qh, next, *list, qhLink, ehci_qh_t)
    {
        if (qh->transfer)
        {
            ehci_process_qh(hc, qh);
        }
    }
}

static void ehci_controller_poll(usb_controller_t *controller)
{
    ehci_controller_t *hc = (ehci_controller_t *)controller->hc;

    ehci_controller_poll_list(hc, &hc->asyncQH->qhLink);
    ehci_controller_poll_list(hc, &hc->periodicQH->qhLink);
}

void usb_ehci_init(uint32_t id, PciDeviceInfo_t *devInfo)
{
    if (!((((devInfo->classCode << 8) | devInfo->subClass) == PCI_SERIAL_USB) &&
          (devInfo->progIntf == PCI_SERIAL_USB_EHCI)))
    {
        return;
    }

    printf("[USB] Initializing EHCI\n");

    PciBAR_t *bar = (PciBAR_t *)&devInfo->type0.BaseAddresses[0];

    uint32_t portAddr = bar->address;

    ehci_controller_t *hc = malloc(sizeof(ehci_controller_t));

    virt_mem_map_page(portAddr, portAddr, 0);

    printf("[EHCI] Allocating memory\n");

    uint64_t frameListBlock = phys_mem_alloc_block();
    virt_mem_map_page(frameListBlock, frameListBlock, 0);

    // TODO: This is 8k memory, but only 3k is needed.
    uint64_t qhBlock = phys_mem_alloc_block();
    virt_mem_map_page(qhBlock, qhBlock, 0);

    uint64_t tdBlock = phys_mem_alloc_block();
    virt_mem_map_page(tdBlock, tdBlock, 0);

    printf("[EHCI] Framelist block: %#016x\n", frameListBlock);
    printf("[EHCI] QH block: %#016x\n", qhBlock);
    printf("[EHCI] TD block: %#016x\n", tdBlock);

    hc->capRegs = (ehci_cap_regs_t *)(uintptr_t)portAddr;
    hc->opRegs = (ehci_op_regs_t *)(uintptr_t)(portAddr + hc->capRegs->capLength);
    hc->frameList = frameListBlock;
    hc->qhPool = qhBlock;
    hc->tdPool = tdBlock;

    if ((uint32_t)hc->tdPool & 0x1F > 0)
    {
        printf("TD misaligned\n");
    }

    memset(hc->qhPool, 0, sizeof(ehci_qh_t) * MAX_QH);
    memset(hc->tdPool, 0, sizeof(ehci_td_t) * MAX_TD);

    printf("[EHCI] Allocating ASYNC QH\n");

    ehci_qh_t *qh = ehci_alloc_qh(hc);
    qh->qhlp = (uint32_t)((uintptr_t)qh | PTR_QH);
    qh->ch = QH_CH_H;
    qh->caps = 0;
    qh->curLink = 0;
    qh->nextLink = PTR_TERMINATE;
    qh->altLink = PTR_TERMINATE;
    qh->token = 0;

    for (uint32_t i = 0; i < 5; ++i)
    {
        qh->buffer[i] = 0;
        qh->extBuffer[i] = 0;
    }

    qh->transfer = 0;
    qh->qhLink.prev = &qh->qhLink;
    qh->qhLink.next = &qh->qhLink;

    hc->asyncQH = qh;

    ehci_print_queue_head(hc->asyncQH);

    printf("[EHCI] Allocating PERIODIC QH\n");

    qh = ehci_alloc_qh(hc);
    qh->qhlp = PTR_TERMINATE;
    qh->ch = 0;
    qh->caps = 0;
    qh->curLink = 0;
    qh->nextLink = PTR_TERMINATE;
    qh->altLink = 0;
    qh->token = 0;

    for (uint32_t i = 0; i < 5; ++i)
    {
        qh->buffer[i] = 0;
        qh->extBuffer[i] = 0;
    }

    qh->transfer = 0;
    qh->qhLink.prev = &qh->qhLink;
    qh->qhLink.next = &qh->qhLink;

    hc->periodicQH = qh;

    ehci_print_queue_head(hc->periodicQH);

    printf("[EHCI] Creating framelist\n");

    for (uint32_t i = 0; i < 1024; ++i)
    {
        hc->frameList[i] = PTR_QH | (uint32_t)(uintptr_t)qh;
    }

    printf("[EHCI] Resetting controller\n");

    hc->opRegs->usbCmd &= ~CMD_RS;

    while (!(hc->opRegs->usbSts & STS_HCHALTED))
        mdelay(10);

    hc->opRegs->usbCmd |= CMD_HCRESET;

    while (hc->opRegs->usbCmd & CMD_HCRESET)
        mdelay(10);

    printf("[EHCI] Controller reset\n");

    hc->capRegs->hccParams |= (HCCPARAMS_64_BIT);

    printf("[EHCI] Disabling legacy support\n");

    uint32_t eecp = (hc->capRegs->hccParams & HCCPARAMS_EECP_MASK) >> HCCPARAMS_EECP_SHIFT;

    if (eecp >= 0x40)
    {
        printf("[EHCI] eecp >= 0x40\n");

        uint32_t legsup = pci_read_l(id, eecp + USBLEGSUP);

        if (legsup & USBLEGSUP_HC_BIOS)
        {
            printf("[EHCI] Disabling BIOS semaphore\n");

            pci_write_l(id, eecp + USBLEGSUP, legsup | USBLEGSUP_HC_OS);

            for (;;)
            {
                legsup = pci_read_l(id, eecp + USBLEGSUP);

                if (~legsup & USBLEGSUP_HC_BIOS && legsup & USBLEGSUP_HC_OS)
                {
                    printf("[EHCI] OS semaphore set\n");

                    break;
                }
            }
        }
    }

    printf("[EHCI] Setting OPREGS\n");

    hc->opRegs->usbIntr = 0;

    hc->opRegs->frameIndex = 0;
    hc->opRegs->periodicListBase = (uint32_t)((uintptr_t)hc->frameList);
    hc->opRegs->asyncListAddr = (uint32_t)((uintptr_t)hc->asyncQH);
    hc->opRegs->ctrlDsSegment = 0;

    hc->opRegs->usbSts = 0x3F;

    hc->opRegs->usbCmd = (8 << CMD_ITC_SHIFT) | CMD_PSE | CMD_RS;

    printf("[EHCI] Enabling Host controller\n");

    while (hc->opRegs->usbSts & STS_HCHALTED)
        ;

    hc->opRegs->configFlag = 1;

    mdelay(5);

    printf("[EHCI] Probing devices\n");

    ehci_probe(hc);

    printf("[EHCI] Allocating controller\n");

    for (;;)
        ;

    usb_controller_t *controller = (usb_controller_t *)malloc(sizeof(usb_controller_t));

    controller->next = usb_get_controller_list();
    controller->hc = hc;
    controller->poll = ehci_controller_poll;

    usb_set_controller_list(controller);

    printf("[USB] EHCI Done\n");
}