/**
 * @file e1000.c
 * @author Joakim Bertils
 * @version 0.1
 * @date 2021-07-12
 *
 * @brief
 *
 * @copyright Copyright (C) 2021,
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
#include <drivers/e1000.h>
#include <logging/logging.h>
#include <mm/phys_mem.h>
#include <mm/virt_mem.h>
#include <sync/spinlock.h>
#include <util/hexdump.h>
#include <util/list.h>
#include <util/mmio.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//=============================================================================
// Definitions
//=============================================================================

#define REG_CTRL 0x0000
#define REG_STATUS 0x0008
#define REG_EEPROM 0x0014
#define REG_CTRL_EXT 0x0018
#define REG_ICR 0x00C0

#define REG_RCTRL 0x0100
#define REG_RXDESCLO 0x2800
#define REG_RXDESCHI 0x2804
#define REG_RXDESCLEN 0x2808
#define REG_RXDESCHEAD 0x2810
#define REG_RXDESCTAIL 0x2818

#define REG_TCTRL 0x0400
#define REG_TXDESCLO 0x3800
#define REG_TXDESCHI 0x3804
#define REG_TXDESCLEN 0x3808
#define REG_TXDESCHEAD 0x3810
#define REG_TXDESCTAIL 0x3818

#define REG_MAC 0x5400

#define RCTL_EN (1 << 1)         /* Receiver Enable */
#define RCTL_SBP (1 << 2)        /* Store Bad Packets */
#define RCTL_UPE (1 << 3)        /* Unicast Promiscuous Enabled */
#define RCTL_MPE (1 << 4)        /* Multicast Promiscuous Enabled */
#define RCTL_LPE (1 << 5)        /* Long Packet Reception Enable */
#define RCTL_LBM_NONE (0 << 6)   /* No Loopback */
#define RCTL_LBM_PHY (3 << 6)    /* PHY or external SerDesc loopback */
#define RCTL_RDMTS_HALF (0 << 8) /* Free Buffer Threshold is 1/2 of RDLEN */
#define RCTL_RDMTS_QUARTER                                                  \
    (1 << 8)                       /* Free Buffer Threshold is 1/4 of RDLEN \
                                    */
#define RCTL_RDMTS_EIGHTH (2 << 8) /* Free Buffer Threshold is 1/8 of RDLEN */
#define RCTL_MO_36 (0 << 12)       /* Multicast Offset - bits 47:36 */
#define RCTL_MO_35 (1 << 12)       /* Multicast Offset - bits 46:35 */
#define RCTL_MO_34 (2 << 12)       /* Multicast Offset - bits 45:34 */
#define RCTL_MO_32 (3 << 12)       /* Multicast Offset - bits 43:32 */
#define RCTL_BAM (1 << 15)         /* Broadcast Accept Mode */
#define RCTL_VFE (1 << 18)         /* VLAN Filter Enable */
#define RCTL_CFIEN (1 << 19)       /* Canonical Form Indicator Enable */
#define RCTL_CFI (1 << 20)         /* Canonical Form Indicator Bit Value */
#define RCTL_DPF (1 << 22)         /* Discard Pause Frames */
#define RCTL_PMCF (1 << 23)        /* Pass MAC Control Frames */
#define RCTL_SECRC (1 << 26)       /* Strip Ethernet CRC */

#define TCTL_EN (1 << 1)      /* Transmit Enable */
#define TCTL_PSP (1 << 3)     /* Pad Short Packets */
#define TCTL_CT_SHIFT 4       /* Collision Threshold */
#define TCTL_COLD_SHIFT 12    /* Collision Distance */
#define TCTL_SWXOFF (1 << 22) /* Software XOFF Transmission */
#define TCTL_RTLC (1 << 24)   /* Re-transmit on Late Collision */

#define CMD_EOP (1 << 0)  /* End of Packet */
#define CMD_IFCS (1 << 1) /* Insert FCS */
#define CMD_IC (1 << 2)   /* Insert Checksum */
#define CMD_RS (1 << 3)   /* Report Status */
#define CMD_RPS (1 << 4)  /* Report Packet Sent */
#define CMD_VLE (1 << 6)  /* VLAN Packet Enable */
#define CMD_IDE (1 << 7)  /* Interrupt Delay Enable */

#define ICR_TXDW (1 << 0)
#define ICR_TXQE (1 << 1)   /* Transmit queue is empty */
#define ICR_LSC (1 << 2)    /* Link status changed */
#define ICR_RXSEQ (1 << 3)  /* Receive sequence count error */
#define ICR_RXDMT0 (1 << 4) /* Receive descriptor minimum threshold */
/* what's 5 (0x20)? */
#define ICR_RXO (1 << 6)  /* Receive overrun */
#define ICR_RXT0 (1 << 7) /* Receive timer interrupt? */

#define RX_DESC_COUNT 32
#define TX_DESC_COUNT 8

#define INTS ((1 << 2) | (1 << 6) | (1 << 7) | (1 << 1) | (1 << 0))

typedef struct rx_descriptor
{
    volatile uint64_t addr;
    volatile uint16_t length;
    volatile uint16_t checksum;
    volatile uint8_t status;
    volatile uint8_t errors;
    volatile uint16_t special;
} __attribute__((packed)) rx_descriptor_t;

typedef struct tx_descriptor
{
    volatile uint64_t addr;
    volatile uint16_t length;
    volatile uint8_t cso;
    volatile uint8_t cmd;
    volatile uint8_t status;
    volatile uint8_t css;
    volatile uint16_t special;
} __attribute__((packed)) tx_descriptor_t;

typedef struct e1000_device
{
    uint64_t mem_base;
    int eeprom;
    uint8_t irq;
    uint8_t mac[6];

    spinlock_t packet_queue_lock;
    list_t *packet_queue;

    rx_descriptor_t *rx;
    tx_descriptor_t *tx;
} e1000_device_t;

//=============================================================================
// Local data
//=============================================================================

static e1000_device_t *device;

//=============================================================================
// Private functions
//=============================================================================

static void queue_push(e1000_device_t *device, void *packet)
{
    spinlock_lock(&device->packet_queue_lock);
    list_insert(device->packet_queue, packet);
    spinlock_unlock(&device->packet_queue_lock);
    log_info("[PACKET_QUEUE] Pushed packet to queue");
}

static void *queue_pop(e1000_device_t *device)
{
    spinlock_lock(&device->packet_queue_lock);
    list_node_t *node = list_dequeue(device->packet_queue);

    void *payload = NULL;

    if (node)
    {
        payload = node->payload;

        free(node);

        log_info("[PACKET_QUEUE] Popped packet from queue");
    }

    spinlock_unlock(&device->packet_queue_lock);
    return payload;
}

static void write_command(e1000_device_t *device,
                          uint16_t address,
                          uint32_t value)
{
    ASSERT(device->mem_base != 0);

    log_debug(
        "[E1000] Writing %#08x to %#04x", value, device->mem_base + address);

    mmio_write32(device->mem_base + address, value);
}

static uint32_t read_command(e1000_device_t *device, uint16_t address)
{
    ASSERT(device->mem_base != 0);

    uint32_t val = mmio_read32(device->mem_base + address);

    log_debug("[E1000] Read %#08x from %#04x", val, device->mem_base + address);

    return val;
}

static void detect_eeprom(e1000_device_t *device)
{
    write_command(device, REG_EEPROM, 0x1);

    int eepromExists = 0;

    for (int i = 0; (i < 1000) && !eepromExists; ++i)
    {
        uint32_t val = read_command(device, REG_EEPROM);

        if (val & 0x10)
        {
            eepromExists = 1;
        }
    }

    device->eeprom = eepromExists;
}

static uint16_t read_eeprom(e1000_device_t *device, uint8_t addr)
{
    uint32_t tmp = 0;
    write_command(device, REG_EEPROM, 1 | ((uint32_t)(addr) << 8));

    while (!((tmp = read_command(device, REG_EEPROM)) & (1 << 4)))
        ;

    return (uint16_t)((tmp >> 16) & 0xFFFF);
}

static int read_mac(e1000_device_t *device)
{
    if (device->eeprom)
    {
        uint32_t t;

        for (int i = 0; i < 3; ++i)
        {
            t = read_eeprom(device, i);
            device->mac[2 * i + 0] = t & 0xFF;
            device->mac[2 * i + 1] = t >> 8;
        }
        return 1;
    }

    for (int i = 0; i < 6; ++i)
    {
        device->mac[i] = mmio_read8(device->mem_base + REG_MAC + i);
    }

    return 1;
}

static void init_rx(e1000_device_t *device)
{
    device->rx = phys_mem_alloc_block();
    virt_mem_map_page(
        (void *)device->rx, (void *)device->rx, VIRT_MEM_WRITABLE);
    memset(device->rx, 0, PAGE_SIZE);

    for (int i = 0; i < RX_DESC_COUNT; ++i)
    {
        device->rx[i].addr = (uint64_t)phys_mem_alloc_blocks(2);
        virt_mem_map_pages((void *)device->rx[i].addr,
                           (void *)device->rx[i].addr,
                           2,
                           VIRT_MEM_WRITABLE);
        device->rx[i].status = 0;

        log_info("[E1000] Allocated RX descriptor: %#016x", device->rx[i].addr);
    }

    write_command(device, REG_RXDESCLO, (uint32_t)(uintptr_t)device->rx);
    write_command(device, REG_RXDESCHI, 0);
    write_command(
        device, REG_RXDESCLEN, RX_DESC_COUNT * sizeof(rx_descriptor_t));
    write_command(device, REG_RXDESCHEAD, 0);
    write_command(device, REG_RXDESCTAIL, RX_DESC_COUNT - 1);

    uint32_t rctrl = read_command(device, REG_RCTRL);

    rctrl |= RCTL_EN;
    rctrl |= RCTL_SBP;
    rctrl |= RCTL_MPE;
    rctrl |= RCTL_BAM;
    rctrl |= RCTL_SECRC;

    write_command(device, REG_RCTRL, rctrl);
}

static void init_tx(e1000_device_t *device)
{
    device->tx = phys_mem_alloc_block();
    virt_mem_map_page(
        (void *)device->tx, (void *)device->tx, VIRT_MEM_WRITABLE);
    memset(device->tx, 0, PAGE_SIZE);

    for (int i = 0; i < TX_DESC_COUNT; ++i)
    {
        device->tx[i].addr = (uint64_t)phys_mem_alloc_blocks(2);
        virt_mem_map_pages((void *)device->tx[i].addr,
                           (void *)device->tx[i].addr,
                           2,
                           VIRT_MEM_WRITABLE);
        device->tx[i].status = 0;
        device->tx[i].cmd = (1 << 0);

        log_info("[E1000] Allocated TX descriptor: %#016x", device->tx[i].addr);
    }

    write_command(device, REG_TXDESCLO, (uint32_t)(uintptr_t)device->tx);
    write_command(device, REG_TXDESCHI, 0);
    write_command(
        device, REG_TXDESCLEN, TX_DESC_COUNT * sizeof(tx_descriptor_t));
    write_command(device, REG_TXDESCHEAD, 0);
    write_command(device, REG_TXDESCTAIL, 0);

    uint32_t tctrl = read_command(device, REG_TCTRL);

    tctrl |= TCTL_EN;
    tctrl |= TCTL_PSP;

    write_command(device, REG_TCTRL, tctrl);
}

static inline uint32_t get_rx_head(e1000_device_t *device)
{
    return read_command(device, REG_RXDESCHEAD);
}

static inline uint32_t get_rx_tail(e1000_device_t *device)
{
    return read_command(device, REG_RXDESCTAIL);
}

static inline void set_rx_tail(e1000_device_t *device, uint32_t value)
{
    write_command(device, REG_RXDESCTAIL, value);
}

static void e1000_handle(e1000_device_t *device, uint32_t status)
{
    if (status & ICR_LSC)
    {
        uint32_t link_status = read_command(device, REG_STATUS) & (1 << 1);

        log_info("[E1000] Link status changed: %i", link_status > 0);
    }

    if (status & (ICR_RXO | ICR_RXT0))
    {
        while (get_rx_head(device) != get_rx_tail(device))
        {
            uint32_t rx_index = get_rx_tail(device);
            rx_index = (rx_index + 1) % RX_DESC_COUNT;

            if (device->rx[rx_index].status & 0x01)
            {
                // TODO: Validate checksum of packet (maybe add as an
                // option/config)
                size_t packet_size = device->rx[rx_index].length;
                uint64_t packet_addr = device->rx[rx_index].addr;

                hexdump(packet_addr, packet_size);

                // void *packet = malloc(packet_size);
                // memcpy(packet, (void *)packet_addr, packet_size);
                // queue_push(device, packet);

                device->rx[rx_index].status = 0;
            }
            set_rx_tail(device, rx_index);
        }
    }
}

static void e1000_irq_handler(system_stack_t *regs)
{
    log_info("[E1000] Got interrupt");

    uint32_t status = read_command(device, REG_ICR);

    if (status)
    {
        write_command(device, 0x00D8, INTS);
        e1000_handle(device, status);
        read_command(device, REG_ICR);
        write_command(device, 0x00D0, INTS);
    }
}

static void send_packet(e1000_device_t *device,
                        void *payload,
                        size_t payload_size)
{
    log_info("[E1000] Sending packet");
    uint32_t tx_index = read_command(device, REG_TXDESCTAIL);

    memcpy((void *)device->tx[tx_index].addr, payload, payload_size);
    device->tx[tx_index].length = payload_size;
    device->tx[tx_index].cmd = CMD_EOP | CMD_IFCS | CMD_RS | CMD_RPS;
    device->tx[tx_index].status = 0;

    tx_index = (tx_index + 1) % TX_DESC_COUNT;
    write_command(device, REG_TXDESCTAIL, tx_index);
}

//=============================================================================
// Interface functions
//=============================================================================

// TODO: Move this to net/ when implementing the network stack.
typedef struct eth_header
{
    uint8_t mac_des[6];
    uint8_t mac_src[6];
    uint16_t ether_type;
    unsigned char payload[];
} __attribute__((packed)) eth_header_t;

void e1000_init(uint32_t id, PciDeviceInfo_t *deviceInfo)
{
    log_info("[E1000] Initializing E1000 driver");

    device = malloc(sizeof(e1000_device_t));

    if (!device)
    {
        log_error("[E1000] Failed to allocate memory");
        return;
    }

    uint64_t bar0 = deviceInfo->type0.BaseAddresses[0];
    uint64_t barType = bar0 & 1;
    device->mem_base = bar0 & ~3;

    log_info("[E1000] Bar type: %s", barType == 0 ? "Memory" : "IO");

    // This will map 16 pages (64 kb) of memory mapped IO registers.
    // Too much?
    virt_mem_map_pages((void *)device->mem_base,
                       (void *)device->mem_base,
                       16,
                       VIRT_MEM_WRITABLE);

    detect_eeprom(device);

    log_info("[E1000] EEPROM: %i", device->eeprom);

    memset(device->mac, 0, sizeof(device->mac));

    read_mac(device);

    char buf[32];

    sprintf(buf,
            "%02x:%02x:%02x:%02x:%02x:%02x",
            device->mac[0],
            device->mac[1],
            device->mac[2],
            device->mac[3],
            device->mac[4],
            device->mac[5]);

    log_info("[E1000] MAC address: %s", buf);

    // Enable Bus-Mastering
    uint32_t command_reg = pciReadConfigField(id, PCI_CONFIG_COMMAND, 2);
    command_reg |= (1 << 2);
    command_reg |= (1 << 0);
    pciWriteConfigField(id, PCI_CONFIG_COMMAND, 2, command_reg);

    spinlock_init(&device->packet_queue_lock);
    device->packet_queue = list_create();

    if (!device->packet_queue)
    {
        log_error("[E1000] Failed to create packet queue");
    }

    init_rx(device);
    init_tx(device);

    uint32_t ctrl = read_command(device, REG_CTRL);

    write_command(device, REG_CTRL, ctrl | 0x80000000);
    read_command(device, REG_STATUS);
    mdelay(10);
    write_command(device, REG_CTRL, ctrl | 0x04000000);
    read_command(device, REG_STATUS);
    mdelay(10);
    write_command(device, REG_CTRL, ctrl | 0x00002000);
    read_command(device, REG_STATUS);
    mdelay(20);
    write_command(device, REG_CTRL, ctrl | (1 << 26));
    mdelay(10);

    uint32_t status = read_command(device, REG_CTRL);
    status |= (1 << 5);       /* set auto speed detection */
    status |= (1 << 6);       /* set link up */
    status &= ~(1 << 3);      /* unset link reset */
    status &= ~(1UL << 31UL); /* unset phy reset */
    status &= ~(1 << 7);      /* unset invert loss-of-signal */
    write_command(device, REG_CTRL, status);

    /* Disables flow control */
    write_command(device, 0x0028, 0);
    write_command(device, 0x002c, 0);
    write_command(device, 0x0030, 0);
    write_command(device, 0x0170, 0);

    /* Unset flow control */
    status = read_command(device, REG_CTRL);
    status &= ~(1 << 30);
    write_command(device, REG_CTRL, status);

    device->irq = deviceInfo->type0.InterruptLine;

    uint8_t irq_pin = deviceInfo->type0.InterruptPin;
    uint8_t irq_line = deviceInfo->type0.InterruptLine;

    log_info(
        "[E1000] Interrupt pin: %i, Interrupt line: %i", irq_pin, irq_line);

    set_irq_handler(device->irq, e1000_irq_handler);
    clear_mask_interrupt(device->irq);

    for (int i = 0; i < 128; ++i)
    {
        write_command(device, 0x5200 + i * 4, 0);
    }

    for (int i = 0; i < 64; ++i)
    {
        read_command(device, 0x4000 + i * 4);
    }

    write_command(device, 0x00D0, 0xFFFFFFFF);
    write_command(device, 0x00D8, 0xFFFFFFFF);
    write_command(device, 0x00D0, INTS);
    read_command(device, 0x00C0);

    uint32_t link_status = read_command(device, REG_STATUS) & (1 << 1);

    log_info("[E1000] Link status: %i", link_status > 0);

    const char *payload = "This is a packet that is to be sent.";

    uint8_t packet[64];

    eth_header_t *header = (eth_header_t *)packet;
    memcpy(header->mac_src, device->mac, 6);
    memcpy(header->payload, payload, strlen(payload));
    header->ether_type = 0x0008;
    sti();

    for (;;)
    {
        void *packet = queue_pop(device);

        if (packet)
        {
            log_info("[E1000] Got packet");

            free(packet);
        }
        else
        {
            uint32_t head = get_rx_head(device);
            uint32_t tail = get_rx_tail(device);
            log_info("[E1000] No packet: %i, %i", head, tail);
        }

        mdelay(100);

        send_packet(device, packet, 14 + strlen(payload));
    };
}

//=============================================================================
// End of file
//=============================================================================
