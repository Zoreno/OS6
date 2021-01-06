/**
 * @file usb_ehci.c
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

#include <usb/usb_ehci.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <arch/arch.h>
#include <debug/backtrace.h>
#include <logging/logging.h>
#include <mm/kheap.h>
#include <mm/phys_mem.h>
#include <mm/virt_mem.h>
#include <pci/pci.h>
#include <pci/pci_device.h>
#include <pci/pci_io.h>
#include <usb/usb_controller.h>
#include <usb/usb_device.h>
#include <util/link.h>

//==============================================================================
// Definitions
//==============================================================================

#define MAX_QH 8
#define MAX_TD 32

#define USBLEGSUP 0x00     // USB Legacy Support Extended Capability
#define UBSLEGCTLSTS 0x04  // USB Legacy Support Control/Status

#define USBLEGSUP_HC_OS 0x01000000     // HC OS Owned Semaphore
#define USBLEGSUP_HC_BIOS 0x00010000   // HC BIOS Owned Semaphore
#define USBLEGSUP_NCP_MASK 0x0000ff00  // Next EHCI Extended Capability Pointer
#define USBLEGSUP_CAPID 0x000000ff     // Capability ID

/**
 * @brief The EHCI Controller Capability registers
 * 
 * 
 */
typedef struct _ehci_cap_regs_t
{
    /**
     * @brief The offset to add to register base to find the beginning of the
     * Operational Register Space.
     *
     *
     */
    uint8_t cap_length;

    /**
     * @brief Reserved
     * 
     * 
     */
    uint8_t reserved;

    /**
     * @brief BCD Encoding of the EHCI revision number supported by this host
     * controller.
     *
     *
     */
    uint16_t hci_version;

    /**
     * @brief Structural parameters of the controller.
     * 
     * 
     */
    uint32_t hcs_params;

    /**
     * @brief Capability parameters of the controller.
     * 
     * 
     */
    uint32_t hcc_params;

    /**
     * @brief 15 array nibble array of physical ports provided by the
     * controller.
     *
     * @note This field is optional and valid only if Port Routing Rules are set
     * in the hcs_params register.
     */
    uint64_t hcsp_port_route;

} __attribute__((packed)) ehci_cap_regs_t;

#define HCSPARAMS_N_PORTS_MASK (15 << 0)  // Number of Ports
#define HCSPARAMS_PPC (1 << 4)            // Port Power Control
#define HCSPARAMS_PORT_ROUTE (1 << 7)     // Port Routing Rules
#define HCSPARAMS_N_PCC_MASK (15 << 8)    // Number of Ports per Companion Controller
#define HCSPARAMS_N_PCC_SHIFT 8
#define HCSPARAMS_N_CC_MASK (15 << 12)  // Number of Companion Controllers
#define HCSPARAMS_N_CC_SHIFT 12
#define HCSPARAMS_P_INDICATOR (1 << 16)  // Port Indicator
#define HCSPARAMS_DPN_MASK (15 << 20)    // Debug Port Number
#define HCSPARAMS_DPN_SHIFT 20

#define HCCPARAMS_64_BIT (1 << 0)       // 64-bit Addressing Capability
#define HCCPARAMS_PFLF (1 << 1)         // Programmable Frame List Flag
#define HCCPARAMS_ASPC (1 << 2)         // Asynchronous Schedule Park Capability
#define HCCPARAMS_IST_MASK (15 << 4)    // Isochronous Sheduling Threshold
#define HCCPARAMS_EECP_MASK (255 << 8)  // EHCI Extended Capabilities Pointer
#define HCCPARAMS_EECP_SHIFT 8

typedef struct _ehci_op_regs_t
{
    /**
     * @brief USB Command Register.
     *
     * Indicates the command to be executed by the serial bus host controller.
     * Writing to the register causes a command to be executed.
     *
     */
    volatile uint32_t usb_cmd;

    /**
     * @brief USB Status Register.
     *
     * Indicates pending interrupts and various states of the host controller.
     * The status resulting from a transaction on the serial bus is not
     * indicated in this register. Software sets a bit to 0 in this register by
     * writing a 1 to it.
     * 
     */
    volatile uint32_t usb_status;

    /**
     * @brief USB Interrupt Enable Register.
     *
     * This register enables and disables reporting of the corresponding
     * interrupt to the software. When a bit is set and the corresponding
     * interrupt is active, an interrupt is generated to the host.
     *
     * Interrupt sources that are disabled in this register still appear in the
     * USB status register to allow software to poll for events.
     *
     */
    volatile uint32_t usb_intr;

    /**
     * @brief Frame Index Register.
     *
     * This register is used by the host controller to index into the periodic
     * frame list. This register updates once per microframe.
     *
     */
    volatile uint32_t frame_index;

    /**
     * @brief Control Data Structure Segment Register.
     *
     * This register corresponds to the most significant address bits of all
     * EHCI data structures, for use by 64 bit hosts.
     *
     */
    volatile uint32_t ctrl_ds_segment;

    /**
     * @brief Periodic Frame List Base Address Register.
     * 
     * This register contains the beginning address of the periodic frame list.
     * 
     */
    volatile uint32_t periodic_list_base;

    /**
     * @brief Current Asynchronous List Address Register.
     * 
     * This register contains the address of the next asynchronous queue head to
     * be executed.
     * 
     */
    volatile uint32_t async_list_addr;

    /**
     * @brief Reserved
     * 
     * 
     */
    volatile uint32_t reserved[9];

    /**
     * @brief Configure Flag Register. 
     *
     * This register is in the auxiliary power well. It is only reset by
     * hardware when the auxiliary power is initially applied or in response to
     * a host controller reset.
     *
     */
    volatile uint32_t config_flag;

    /**
     * @brief Port Status and Control Register.
     *
     * A host controller must implement one or more port registers. The number
     * of port registers implemented by a particular instantiation of a host
     * controller is documented in the hcs_param register. Software uses this
     * information as an input parameter to determine how many ports need to be
     * serviced.
     *
     */
    volatile uint32_t ports[];

} __attribute__((packed)) ehci_op_regs_t;

#define CMD_RS (1 << 0)        // Run/Stop
#define CMD_HCRESET (1 << 1)   // Host Controller Reset
#define CMD_FLS_MASK (3 << 2)  // Frame List Size
#define CMD_FLS_SHIFT 2
#define CMD_PSE (1 << 4)         // Periodic Schedule Enable
#define CMD_ASE (1 << 5)         // Asynchronous Schedule Enable
#define CMD_IOAAD (1 << 6)       // Interrupt on Async Advance Doorbell
#define CMD_LHCR (1 << 7)        // Light Host Controller Reset
#define CMD_ASPMC_MASK (3 << 8)  // Asynchronous Schedule Park Mode Count
#define CMD_ASPMC_SHIFT 8
#define CMD_ASPME (1 << 11)       // Asynchronous Schedule Park Mode Enable
#define CMD_ITC_MASK (255 << 16)  // Interrupt Threshold Control
#define CMD_ITC_SHIFT 16

#define STS_USBINT (1 << 0)        // USB Interrupt
#define STS_ERROR (1 << 1)         // USB Error Interrupt
#define STS_PCD (1 << 2)           // Port Change Detect
#define STS_FLR (1 << 3)           // Frame List Rollover
#define STS_HSE (1 << 4)           // Host System Error
#define STS_IOAA (1 << 5)          // Interrupt on Async Advance
#define STS_HCHALTED (1 << 12)     // Host Controller Halted
#define STS_RECLAMATION (1 << 13)  // Reclamation
#define STS_PSS (1 << 14)          // Periodic Schedule Status
#define STS_ASS (1 << 15)          // Asynchronous Schedule Status

#define INTR_USBINT (1 << 0)  // USB Interrupt Enable
#define INTR_ERROR (1 << 1)   // USB Error Interrupt Enable
#define INTR_PCD (1 << 2)     // Port Change Interrupt Enable
#define INTR_FLR (1 << 3)     // Frame List Rollover Enable
#define INTR_HSE (1 << 4)     // Host System Error Enable
#define INTR_IOAA (1 << 5)    // Interrupt on Async Advance Enable

#define FR_MASK 0x3fff

#define CF_PORT_ROUTE (1 << 0)  // Configure Flag (CF)

#define PORT_CONNECTION (1 << 0)           // Current Connect Status
#define PORT_CONNECTION_CHANGE (1 << 1)    // Connect Status Change
#define PORT_ENABLE (1 << 2)               // Port Enabled
#define PORT_ENABLE_CHANGE (1 << 3)        // Port Enable Change
#define PORT_OVER_CURRENT (1 << 4)         // Over-current Active
#define PORT_OVER_CURRENT_CHANGE (1 << 5)  // Over-current Change
#define PORT_FPR (1 << 6)                  // Force Port Resume
#define PORT_SUSPEND (1 << 7)              // Suspend
#define PORT_RESET (1 << 8)                // Port Reset
#define PORT_LS_MASK (3 << 10)             // Line Status
#define PORT_LS_SHIFT 10
#define PORT_POWER (1 << 12)    // Port Power
#define PORT_OWNER (1 << 13)    // Port Owner
#define PORT_IC_MASK (3 << 14)  // Port Indicator Control
#define PORT_IC_SHIFT 14
#define PORT_TC_MASK (15 << 16)  // Port Test Control
#define PORT_TC_SHIFT 16
#define PORT_WKCNNT_E (1 << 20)    // Wake on Connect Enable
#define PORT_WKDSCNNT_E (1 << 21)  // Wake on Disconnect Enable
#define PORT_WKOC_E (1 << 22)      // Wake on Over-current Enable
#define PORT_RWC (PORT_CONNECTION_CHANGE | PORT_ENABLE_CHANGE | PORT_OVER_CURRENT_CHANGE)

/**
 * @brief Isochronous High Speed Transfer Descriptor
 * 
 * This format is only used for high-speed isochronous endpoints.
 * 
 * @note Must be aligned on a 32 byte boundary.
 * 
 */
typedef struct _ehci_itd_t
{
    /**
     * @brief Next Link Pointer.
     * 
     * Pointer to the next schedule data structure.
     * 
     */
    uint32_t next_link;

    /**
     * @brief iTD Transaction Status and Control.
     *
     * The host controller uses the information in each transaction description
     * plus the endpoint information contained in the first three d-words of the
     * Buffer Page Pointer list, to execute a transaction on the USB.
     *
     */
    uint32_t token[8];

    /**
     * @brief iTD Buffer Page Pointer List.
     * 
     * Page-pointers to the data buffer for this transfer descriptor.
     */
    uint32_t buffer[7];

    /**
     * @brief Extra iTD Buffer Page Pointer List.
     * 
     * Page-pointers to the data buffer for this transfer descriptor.
     */
    uint32_t ext_buffer[7];

    /**
     * @brief Physical address of this transfer descriptor.
     * 
     * 
     */
    uint32_t this;

    /**
     * @brief Pointer to the next transfer descriptor.
     * 
     * 
     */
    uint32_t next;

    /**
     * @brief Pointer to the previous transfer descriptor.
     * 
     * 
     */
    uint32_t prev;

    /**
     * @brief Last token of the transfer.
     * 
     * 
     */
    uint32_t last_token;

} __attribute__((packed)) ehci_itd_t;

#define EHCI_ITD_TOFFSET_SHIFT 0
#define EHCI_ITD_TOFFSET_MASK 0x0fff
#define EHCI_ITD_IOC (1 << 15)
#define EHCI_ITD_PG_SHIFT 12
#define EHCI_ITD_PG_MASK 0x07
#define EHCI_ITD_TLENGTH_SHIFT 16
#define EHCI_ITD_TLENGTH_MASK 0x0fff
#define EHCI_ITD_STATUS_SHIFT 28
#define EHCI_ITD_STATUS_MASK 0xf
#define EHCI_ITD_STATUS_ACTIVE (1 << 3)  // Active
#define EHCI_ITD_STATUS_BUFFER (1 << 2)  // Data Buffer Error
#define EHCI_ITD_STATUS_BABBLE (1 << 1)  // Babble Detected
#define EHCI_ITD_STATUS_TERROR (1 << 0)  // Transaction Error
#define EHCI_ITD_ADDRESS_SHIFT 0
#define EHCI_ITD_ADDRESS_MASK 0x7f
#define EHCI_ITD_ENDPOINT_SHIFT 8
#define EHCI_ITD_ENDPOINT_MASK 0xf
#define EHCI_ITD_DIR_SHIFT 11
#define EHCI_ITD_MUL_SHIFT 0
#define EHCI_ITD_MUL_MASK 0x3
#define EHCI_ITD_BUFFERPOINTER_SHIFT 12
#define EHCI_ITD_BUFFERPOINTER_MASK 0xfffff
#define EHCI_ITD_MAXPACKETSIZE_SHIFT 0
#define EHCI_ITD_MAXPACKETSIZE_MASK 0x7ff
#define EHCI_ITD_MAXPACKETSIZE_LENGTH 11

/**
 * @brief Split Transaction Isochronous Transfer Descriptor
 *
 * All full-speed isochronous transfer through Transaction Translators are
 * managed using the siTD data structure. The data structure satisfies the
 * operational requirements for managing the split transaction protocol.
 * 
 */
typedef struct _ehci_sitd_t
{
    /**
     * @brief Next Link Pointer.
     * 
     * Pointer to the next schedule data structure.
     * 
     */
    uint32_t next_link;

    /**
     * @brief Port number.
     * 
     * Port number of the recipient transaction translator.
     * 
     */
    uint8_t port_number;

    /**
     * @brief Hub address.
     * 
     * Device address of the transaction translation's hub.
     * 
     */
    uint8_t hub_address;

    /**
     * @brief Endpoint number.
     *
     * Selects the particular endpoint number on the device serving as the
     * data source or sink.
     * 
     */
    uint8_t endpoint;

    /**
     * @brief Device address.
     * 
     * Selects the specific device serving as the data source or sink.
     */
    uint8_t device_address;

    /**
     * @brief Reserved.
     * 
     * 
     */
    uint16_t reserved1;

    /**
     * @brief Completion mask.
     *
     * This field is used to determine during which micro-frames the host
     * controller should execute complete-split transactions.
     *
     */
    uint8_t c_mask;

    /**
     * @brief Start mask.
     *
     * This field is used to determine during which micro-frames the host
     * controller should execute start-split transactions.
     * 
     */
    uint8_t s_mask;

    /**
     * @brief Transfer length.
     * 
     * The number of bytes expected in this transfer.
     * 
     */
    uint16_t transfer_length;

    /**
     * @brief Complete-split progress mask
     *
     * This field is used by the host controller to record which split-completes
     * have been executed.
     *
     */
    uint8_t c_prog_mask;

    /**
     * @brief Status.
     *
     * This field records the status of the transaction executed by the host
     * controller for this slot. This field is a bit vector.
     *
     */
    uint8_t status;

    /**
     * @brief Buffer Pointer List.
     * 
     * Page-aligned buffer pointers.
     * 
     */
    uint32_t buffer[2];

    /**
     * @brief Back Link Pointer.
     * 
     * Physical memory of an ehci_sitd_t;
     */
    uint32_t back;

    /**
     * @brief Extra Buffer Pointer List.
     * 
     * Page-aligned buffer pointers.
     * 
     */
    uint32_t ext_buffer;

    /**
     * @brief Physical memory of this structure.
     * 
     * 
     */
    uint32_t this;

    /**
     * @brief Pointer to the next transfer descriptor
     * 
     * 
     */
    uint32_t next;

    /**
     * @brief Pointer to the previous transfer descriptor.
     * 
     * 
     */
    uint32_t prev;

    /**
     * @brief Size of the buffer.
     * 
     * 
     */
    size_t buffer_size;

    /**
     * @brief Buffer log.
     * 
     * 
     */
    void *buffer_log;

} __attribute__((packed)) ehci_sitd_t;

/**
 * @brief Queue Element Transfer Descriptor
 *
 * This data structure is only used with one queue head. It is used for one or
 * more USB transactions. It can be used to transfer up to 20480 (5 * 4096)
 * bytes. This structure contains two structure pointers used for queue
 * advancement, a d-word for transfer state and a five-element array of data
 * buffer pointers.
 *
 * @note Must be aligned on a 32 byte boundary.
 * 
 */
typedef struct _ehci_td_t
{
    /**
     * @brief Pointer to the next transfer descriptor in chain.
     *
     * Contains the physical memory address of the next transfer descriptor to
     * be processed, unless LSB is set, which indicates that this transfer
     * descriptor is the end of the chain.
     *
     *
     */
    volatile uint32_t link;

    /**
     * @brief Alternative pointer to the next transfer descriptor.
     * 
     * Used for hardware advancement of the data stream to the next
     * client buffer on short packet.
     *
     *
     */
    volatile uint32_t alt_link;

    /**
     * @brief Queue Transfer Descriptor State.
     * 
     * Contains most of the information needed for the USB host controller to execute the USB transaction.
     */
    volatile uint32_t token;

    /**
     * @brief Buffer Page Pointer List.
     * 
     * Array of physical memory address pointers.
     */
    volatile uint32_t buffer[5];

    /**
     * @brief Extended Buffer Page Pointer List.
     * 
     * Array of physical memory address pointers.
     */
    volatile uint32_t ext_buffer[5];

    // internal fields

    /**
     * @brief Index of the next transfer descriptor.
     * 
     * 
     */
    uint32_t td_next;

    /**
     * @brief Non-zero if the TD is currently active.
     * 
     * 
     */
    uint32_t active;

    /**
     * @brief Padding to ensure the alignment of the field
     * 
     * 
     */
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
#define TD_TOK_PING (1 << 0)        // Ping State
#define TD_TOK_STS (1 << 1)         // Split Transaction State
#define TD_TOK_MMF (1 << 2)         // Missed Micro-Frame
#define TD_TOK_XACT (1 << 3)        // Transaction Error
#define TD_TOK_BABBLE (1 << 4)      // Babble Detected
#define TD_TOK_DATABUFFER (1 << 5)  // Data Buffer Error
#define TD_TOK_HALTED (1 << 6)      // Halted
#define TD_TOK_ACTIVE (1 << 7)      // Active
#define TD_TOK_PID_MASK (3 << 8)    // PID Code
#define TD_TOK_PID_SHIFT 8
#define TD_TOK_CERR_MASK (3 << 10)  // Error Counter
#define TD_TOK_CERR_SHIFT 10
#define TD_TOK_C_PAGE_MASK (7 << 12)  // Current Page
#define TD_TOK_C_PAGE_SHIFT 12
#define TD_TOK_IOC (1 << 15)        // Interrupt on Complete
#define TD_TOK_LEN_MASK 0x7fff0000  // Total Bytes to Transfer
#define TD_TOK_LEN_SHIFT 16
#define TD_TOK_D (1 << 31)  // Data Toggle
#define TD_TOK_D_SHIFT 31

#define USB_PACKET_OUT 0    // token 0xe1
#define USB_PACKET_IN 1     // token 0x69
#define USB_PACKET_SETUP 2  // token 0x2d

typedef struct _ehci_qh_t
{
    /**
     * @brief Queue Head Horizontal Link Pointer.
     *
     * Contains a link pointer to the next data object to be processed after any
     * required processing in the queue has been completed, as well as some
     * control bits.
     * 
     */
    volatile uint32_t qhlp;

    /**
     * @brief Queue Head Endpoint Characteristics
     *
     * These are the USB endpoint characteristics including addressing, maximum
     * packet size and endpoint speed.
     */
    volatile uint32_t ch;

    /**
     * @brief Queue Head Endpoint Capabilites
     *
     * These are the adjustable parameters of the endpoint. They effect how the
     * endpoint data stream is managed by the host controller.
     */
    volatile uint32_t caps;

    /**
     * @brief Current Transfer Descriptor Link Pointer.
     * 
     * 
     */
    volatile uint32_t cur_link;

    // matches a transfer descriptor
    volatile uint32_t next_link;
    volatile uint32_t alt_link;
    volatile uint32_t token;
    volatile uint32_t buffer[5];
    volatile uint32_t ext_buffer[5];

    // internal fields

    /**
     * @brief USB Transfer descriptor pointer.
     * 
     * 
     */
    usb_transfer_t *transfer;

    /**
     * @brief Link node for this Queue Head.
     * 
     * 
     */
    link_t qh_link;

    /**
     * @brief Transfer Descriptor Head Pointer.
     * 
     * 
     */
    uint32_t td_head;

    /**
     * @brief Non-zero if the Queue Head is active at the moment.
     * 
     * 
     */
    uint32_t active;

    /**
     * @brief Padding to ensure that the data structure is 32-byte aligned.
     * 
     * 
     */
    uint8_t pad[28];
} __attribute__((packed)) ehci_qh_t;

// Endpoint Characteristics
#define QH_CH_DEVADDR_MASK 0x0000007f  // Device Address
#define QH_CH_INACTIVE 0x00000080      // Inactive on Next Transaction
#define QH_CH_ENDP_MASK 0x00000f00     // Endpoint Number
#define QH_CH_ENDP_SHIFT 8
#define QH_CH_EPS_MASK 0x00003000  // Endpoint Speed
#define QH_CH_EPS_SHIFT 12
#define QH_CH_DTC 0x00004000       // Data Toggle Control
#define QH_CH_H 0x00008000         // Head of Reclamation List Flag
#define QH_CH_MPL_MASK 0x07ff0000  // Maximum Packet Length
#define QH_CH_MPL_SHIFT 16
#define QH_CH_CONTROL 0x08000000      // Control Endpoint Flag
#define QH_CH_NAK_RL_MASK 0xf0000000  // Nak Count Reload
#define QH_CH_NAK_RL_SHIFT 28

// Endpoint Capabilities
#define QH_CAP_INT_SCHED_SHIFT 0
#define QH_CAP_INT_SCHED_MASK 0x000000ff  // Interrupt Schedule Mask
#define QH_CAP_SPLIT_C_SHIFT 8
#define QH_CAP_SPLIT_C_MASK 0x0000ff00  // Split Completion Mask
#define QH_CAP_HUB_ADDR_SHIFT 16
#define QH_CAP_HUB_ADDR_MASK 0x007f0000  // Hub Address
#define QH_CAP_PORT_MASK 0x3f800000      // Port Number
#define QH_CAP_PORT_SHIFT 23
#define QH_CAP_MULT_MASK 0xc0000000  // High-Bandwidth Pipe Multiplier
#define QH_CAP_MULT_SHIFT 30

typedef struct _ehci_controller_t
{
    ehci_cap_regs_t *cap_regs;
    ehci_op_regs_t *op_regs;
    uint32_t *frame_list;
    ehci_qh_t *qh_pool;
    ehci_td_t *td_pool;
    ehci_qh_t *async_qh;
    ehci_qh_t *periodic_qh;

} ehci_controller_t;

//==============================================================================
// Local variables
//==============================================================================

//==============================================================================
// Forward declarations
//==============================================================================

static void ehci_print_queue_head(ehci_qh_t *qh);
static uint32_t ehci_qh_is_active(ehci_qh_t *qh);
static void ehci_qh_set_active(ehci_qh_t *qh, uint32_t active);
static uint32_t ehci_td_is_active(ehci_td_t *td);
static void ehci_td_set_active(ehci_td_t *td, uint32_t active);
static ehci_td_t *ehci_alloc_td(ehci_controller_t *hc);
static ehci_qh_t *ehci_alloc_qh(ehci_controller_t *hc);
static void ehci_free_td(ehci_td_t *td);
static void ehci_free_qh(ehci_qh_t *qh);
static void ehci_insert_async_qh(ehci_qh_t *list, ehci_qh_t *qh);
static void ehci_insert_periodic_qh(ehci_qh_t *list, ehci_qh_t *qh);
static void ehci_remove_qh(ehci_qh_t *qh);
static void ehci_port_set(volatile uint32_t *port_reg, uint32_t data);
static void ehci_port_clr(volatile uint32_t *port_reg, uint32_t data);
static void ehci_init_td(ehci_td_t *td,
                         ehci_td_t *prev,
                         uint32_t toggle,
                         uint32_t packet_type,
                         uint32_t len,
                         const void *data);
static void ehci_init_qh(ehci_qh_t *qh,
                         usb_transfer_t *t,
                         ehci_td_t *td,
                         usb_device_t *parent,
                         int interrupt,
                         uint32_t speed,
                         uint32_t addr,
                         uint32_t endp,
                         uint32_t max_size);
static void ehci_process_qh(ehci_controller_t *hc, ehci_qh_t *qh);
static void ehci_wait_for_qh(ehci_controller_t *hc, ehci_qh_t *qh);
static uint32_t ehci_port_reset(ehci_controller_t *hc, uint32_t port);
static void ehci_dev_control(usb_device_t *dev, usb_transfer_t *t);
static void ehci_dev_intr(usb_device_t *dev, usb_transfer_t *t);
static void ehci_probe(ehci_controller_t *hc);
static void ehci_controller_poll_list(ehci_controller_t *hc, link_t *list);
static void ehci_controller_poll(usb_controller_t *controller);
static uint64_t ehci_get_port_address(PciDeviceInfo_t *dev_info);
static void *ehci_alloc_physmem();
static ehci_qh_t *ehci_alloc_async_qh(ehci_controller_t *hc);
static ehci_qh_t *ehci_alloc_periodic_qh(ehci_controller_t *hc);
static void ehci_reset_controller(ehci_controller_t *hc);
static void ehci_set_64_bit_mode(ehci_controller_t *hc);
static void ehci_disable_legacy_support(uint32_t id, ehci_controller_t *hc);
static void ehci_controller_init_op_regs(ehci_controller_t *hc);
static ehci_controller_t *ehci_init_hc(uint32_t id, uint64_t port_addr);

//==============================================================================
// Local functions
//==============================================================================

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

static uint32_t ehci_qh_is_active(ehci_qh_t *qh)
{
    return qh->active;
}

static void ehci_qh_set_active(ehci_qh_t *qh, uint32_t active)
{
    qh->active = active ? 1 : 0;
}

static uint32_t ehci_td_is_active(ehci_td_t *td)
{
    return td->active;
}

static void ehci_td_set_active(ehci_td_t *td, uint32_t active)
{
    td->active = active ? 1 : 0;
}

static ehci_td_t *ehci_alloc_td(ehci_controller_t *hc)
{
    ehci_td_t *end = hc->td_pool + MAX_TD;

    for (ehci_td_t *td = hc->td_pool; td != end; ++td)
    {
        if (!ehci_td_is_active(td))
        {
            ehci_td_set_active(td, 1);
            return td;
        }
    }

    log_error("[EHCI] Alloc TD failed");

    return NULL;
}

static ehci_qh_t *ehci_alloc_qh(ehci_controller_t *hc)
{
    ehci_qh_t *end = hc->qh_pool + MAX_QH;

    for (ehci_qh_t *qh = hc->qh_pool; qh != end; ++qh)
    {
        if (!ehci_qh_is_active(qh))
        {
            ehci_qh_set_active(qh, 1);
            return qh;
        }
    }

    log_error("[EHCI] Alloc QH failed");

    return NULL;
}

static void ehci_free_td(ehci_td_t *td)
{
    ehci_td_set_active(td, 0);
}

static void ehci_free_qh(ehci_qh_t *qh)
{
    ehci_qh_set_active(qh, 0);
}

static void ehci_insert_async_qh(ehci_qh_t *list, ehci_qh_t *qh)
{
    log_debug("[EHCI] Inserting async QH");
    ehci_qh_t *end = link_data(list->qh_link.prev, ehci_qh_t, qh_link);

    qh->qhlp = (uint32_t)(uintptr_t)list | PTR_QH;
    end->qhlp = (uint32_t)(uintptr_t)qh | PTR_QH;

    link_before(&list->qh_link, &qh->qh_link);
}

static void ehci_insert_periodic_qh(ehci_qh_t *list, ehci_qh_t *qh)
{
    log_debug("[EHCI] Inserting periodic QH");

    ehci_qh_t *end = link_data(list->qh_link.prev, ehci_qh_t, qh_link);

    qh->qhlp = PTR_TERMINATE;
    end->qhlp = (uint32_t)(uintptr_t)qh | PTR_QH;

    link_before(&list->qh_link, &qh->qh_link);
}

static void ehci_remove_qh(ehci_qh_t *qh)
{
    log_debug("[EHCI] Removing QH");

    ehci_qh_t *prev = link_data(qh->qh_link.prev, ehci_qh_t, qh_link);

    prev->qhlp = qh->qhlp;

    link_remove(&qh->qh_link);
}

static void ehci_port_set(volatile uint32_t *port_reg, uint32_t data)
{
    uint32_t status = *port_reg;
    status |= data;
    status &= ~PORT_RWC;
    *port_reg = status;
}

static void ehci_port_clr(volatile uint32_t *port_reg, uint32_t data)
{
    uint32_t status = *port_reg;
    status &= ~PORT_RWC;
    status &= ~data;
    status |= PORT_RWC & data;
    *port_reg = status;
}

static void ehci_init_td(ehci_td_t *td,
                         ehci_td_t *prev,
                         uint32_t toggle,
                         uint32_t packet_type,
                         uint32_t len,
                         const void *data)
{
    log_debug("[EHCI] Creating Transfer descriptor");

    if (prev)
    {
        prev->link = (uint32_t)(uintptr_t)td;
        prev->td_next = (uint32_t)(uintptr_t)td;
    }

    td->link = PTR_TERMINATE;
    td->alt_link = PTR_TERMINATE;
    td->td_next = 0;

    td->token = (toggle << TD_TOK_D_SHIFT) |
                (len << TD_TOK_LEN_SHIFT) |
                (3 << TD_TOK_CERR_SHIFT) |
                (packet_type << TD_TOK_PID_SHIFT) |
                TD_TOK_ACTIVE;

    uintptr_t p = (uintptr_t)data;

    td->buffer[0] = (uint32_t)p;
    td->ext_buffer[0] = (uint32_t)(p >> 32);
    p &= ~0xFFF;

    for (uint32_t i = 1; i < 4; ++i)
    {
        p += 0x1000;
        td->buffer[i] = (uint32_t)(p);
        td->ext_buffer[i] = (uint32_t)((uint64_t)p >> 32);
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
                         uint32_t max_size)
{
    log_debug("[EHCI] Creating Queue Head");

    qh->transfer = t;

    uint32_t ch =
        (max_size << QH_CH_MPL_SHIFT) |
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

    qh->td_head = (uint32_t)(uintptr_t)td;
    qh->next_link = (uint32_t)(uintptr_t)td;

    qh->token = 0;
}

static void ehci_process_qh(ehci_controller_t *hc, ehci_qh_t *qh)
{
    usb_transfer_t *t = qh->transfer;

    if (qh->token & TD_TOK_HALTED)
    {
        log_warn("[EHCI] TD_TOK_HALTED");

        t->success = 0;
        t->complete = 1;
    }
    else if (qh->next_link & PTR_TERMINATE)
    {
        if (~qh->token & TD_TOK_ACTIVE)
        {
            if (qh->token & TD_TOK_DATABUFFER)
            {
                log_error("[EHCI] Error: Data buffer");
            }

            if (qh->token & TD_TOK_BABBLE)
            {
                log_error("[EHCI] Error: Babble");
            }

            if (qh->token & TD_TOK_XACT)
            {
                log_error("[EHCI] Error: Transaction error");
            }

            if (qh->token & TD_TOK_MMF)
            {
                log_error("[EHCI] Error: Missed Micro-frame");
            }

            t->success = 1;
            t->complete = 1;
        }
    }

    if (t->complete)
    {
        log_debug("[EHCI] Complete");
        qh->transfer = 0;

        if (t->success && t->endp)
        {
            t->endp->toggle ^= 1;
        }

        ehci_remove_qh(qh);

        ehci_td_t *td = (ehci_td_t *)(uintptr_t)qh->td_head;

        while (td)
        {
            ehci_td_t *next = (ehci_td_t *)(uintptr_t)td->td_next;
            ehci_free_td(td);
            td = next;
        }

        ehci_free_qh(qh);
    }
}

static void ehci_wait_for_qh(ehci_controller_t *hc, ehci_qh_t *qh)
{
    log_debug("[EHCI] Waiting for QH");

    usb_transfer_t *t = qh->transfer;

    while (!t->complete)
    {
        ehci_process_qh(hc, qh);
    }
}

static uint32_t ehci_port_reset(ehci_controller_t *hc, uint32_t port)
{
    volatile uint32_t *reg = &hc->op_regs->ports[port];

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
            log_info("[EHCI] No connection at port %i", port);
            break;
        }

        if (status & (PORT_ENABLE_CHANGE | PORT_CONNECTION_CHANGE))
        {
            log_warn("[EHCI] Change ack at port %i", port);
            ehci_port_clr(reg, PORT_ENABLE_CHANGE | PORT_CONNECTION_CHANGE);
            continue;
        }

        if (status & PORT_ENABLE)
        {
            log_debug("[EHCI] Port %i enabled", port);
            break;
        }
    }

    return status;
}

static void ehci_dev_control(usb_device_t *dev, usb_transfer_t *t)
{
    log_debug("[EHCI] dev control");

    ehci_controller_t *hc = (ehci_controller_t *)dev->hc;

    usb_dev_req_t *req = t->req;

    uint32_t speed = dev->speed;
    uint32_t addr = dev->addr;
    uint32_t max_size = dev->max_packet_size;
    uint32_t type = req->type;
    uint32_t len = req->len;

    ehci_td_t *td = ehci_alloc_td(hc);

    if (!td)
    {
        log_error("[EHCI] Could not allocate TD");
        backtrace();
        return;
    }

    ehci_td_t *head = td;
    ehci_td_t *prev = 0;

    uint32_t toggle = 0;
    uint32_t packet_type = USB_PACKET_SETUP;
    uint32_t packet_size = sizeof(usb_dev_req_t);

    ehci_init_td(td, prev, toggle, packet_type, packet_size, req);
    prev = td;

    packet_type = type & RT_DEV_TO_HOST ? USB_PACKET_IN : USB_PACKET_OUT;

    uint8_t *it = (uint8_t *)t->data;
    uint8_t *end = it + len;

    while (it < end)
    {
        td = ehci_alloc_td(hc);

        if (!td)
        {
            log_error("[EHCI] Could not allocate TD");
            backtrace();
            return;
        }

        toggle ^= 1;
        packet_size = end - it;

        if (packet_size > max_size)
        {
            packet_size = max_size;
        }

        ehci_init_td(td, prev, toggle, packet_type, packet_size, it);

        it += packet_size;

        prev = td;
    }

    td = ehci_alloc_td(hc);

    if (!td)
    {
        log_error("[EHCI] Could not allocate TD");
        backtrace();
        return;
    }

    toggle = 1;

    packet_type = type & RT_DEV_TO_HOST ? USB_PACKET_OUT : USB_PACKET_IN;

    ehci_init_td(td, prev, toggle, packet_type, 0, 0);

    ehci_qh_t *qh = ehci_alloc_qh(hc);

    ehci_init_qh(qh, t, head, dev->parent, 0, speed, addr, 0, max_size);

    ehci_insert_async_qh(hc->async_qh, qh);

    ehci_wait_for_qh(hc, qh);
}

static void ehci_dev_intr(usb_device_t *dev, usb_transfer_t *t)
{
    log_debug("[EHCI] dev intr");

    ehci_controller_t *hc = (ehci_controller_t *)dev->hc;

    uint32_t speed = dev->speed;
    uint32_t addr = dev->addr;
    uint32_t max_size = dev->max_packet_size;
    uint32_t endp = dev->endp.desc.addr & 0xF;

    ehci_td_t *td = ehci_alloc_td(hc);

    if (!td)
    {
        log_error("[EHCI] Could not allocate TD");

        t->success = 0;
        t->complete = 1;

        return;
    }

    ehci_td_t *head = td;
    ehci_td_t *prev = 0;

    uint32_t toggle = dev->endp.toggle;
    uint32_t packet_type = USB_PACKET_IN;
    uint32_t packet_size = t->len;

    ehci_init_td(td, prev, toggle, packet_type, packet_size, t->data);

    ehci_qh_t *qh = ehci_alloc_qh(hc);
    ehci_init_qh(qh, t, head, dev->parent, 1, speed, addr, endp, max_size);

    ehci_insert_periodic_qh(hc->periodic_qh, qh);
}

static void ehci_probe(ehci_controller_t *hc)
{
    uint32_t port_count = hc->cap_regs->hcs_params & HCSPARAMS_N_PORTS_MASK;

    for (uint32_t port = 0; port < port_count; ++port)
    {
        uint32_t status = ehci_port_reset(hc, port);

        if (status & PORT_ENABLE)
        {
            uint32_t speed = USB_HIGH_SPEED;

            log_debug("[EHCI] Allocating device for port %i", port);

            usb_device_t *dev = usb_dev_create();

            if (dev)
            {
                log_debug("[EHCI] USB dev for port %i created", port);

                dev->parent = 0;
                dev->hc = hc;
                dev->port = port;
                dev->speed = speed;
                dev->max_packet_size = 8;

                dev->hc_control = ehci_dev_control;
                dev->hc_intr = ehci_dev_intr;

                if (!usb_dev_init(dev))
                {
                    log_warn("[EHCI] USB device failed to initiate for %i", port);
                }
            }
        }
    }
}

static void ehci_controller_poll_list(ehci_controller_t *hc, link_t *list)
{
    ehci_qh_t *qh;
    ehci_qh_t *next;

    list_for_each_safe(qh, next, *list, qh_link, ehci_qh_t)
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

    ehci_controller_poll_list(hc, &hc->async_qh->qh_link);
    ehci_controller_poll_list(hc, &hc->periodic_qh->qh_link);
}

static uint64_t ehci_get_port_address(PciDeviceInfo_t *dev_info)
{
    PciBAR_t *bar = (PciBAR_t *)&dev_info->type0.BaseAddresses[0];

    uint64_t port_addr = (uint64_t)bar->address;

    virt_mem_map_page((void *)port_addr, (void *)port_addr, 0);

    return port_addr;
}

static void *ehci_alloc_physmem()
{
    void *addr = phys_mem_alloc_block();

    virt_mem_map_page(addr, addr, 0);

    return addr;
}

static ehci_qh_t *ehci_alloc_async_qh(ehci_controller_t *hc)
{
    log_debug("[EHCI] Allocating ASYNC QH");

    ehci_qh_t *qh = ehci_alloc_qh(hc);

    if (!qh)
    {
        log_error("[EHCI] Failed to allocate ASYNC QH memory");
        return NULL;
    }

    qh->qhlp = (uint32_t)((uintptr_t)qh | PTR_QH);
    qh->ch = QH_CH_H;
    qh->caps = 0;
    qh->cur_link = 0;
    qh->next_link = PTR_TERMINATE;
    qh->alt_link = 0;
    qh->token = 0;

    for (uint32_t i = 0; i < 5; ++i)
    {
        qh->buffer[i] = 0;
        qh->ext_buffer[i] = 0;
    }

    qh->transfer = 0;
    qh->qh_link.prev = &qh->qh_link;
    qh->qh_link.next = &qh->qh_link;

    return qh;
}

static ehci_qh_t *ehci_alloc_periodic_qh(ehci_controller_t *hc)
{
    log_debug("[EHCI] Allocating PERIODIC QH");

    ehci_qh_t *qh = ehci_alloc_qh(hc);

    if (!qh)
    {
        log_error("[EHCI] Failed to allocate PERIODIC QH memory");
        return qh;
    }

    qh->qhlp = PTR_TERMINATE;
    qh->ch = 0;
    qh->caps = 0;
    qh->cur_link = 0;
    qh->next_link = PTR_TERMINATE;
    qh->alt_link = 0;
    qh->token = 0;

    for (uint32_t i = 0; i < 5; ++i)
    {
        qh->buffer[i] = 0;
        qh->ext_buffer[i] = 0;
    }

    qh->transfer = 0;
    qh->qh_link.prev = &qh->qh_link;
    qh->qh_link.next = &qh->qh_link;

    return qh;
}

static void ehci_reset_controller(ehci_controller_t *hc)
{
    log_debug("[EHCI] Resetting controller");

    hc->op_regs->usb_cmd &= ~CMD_RS;

    while (!(hc->op_regs->usb_status & STS_HCHALTED))
    {
        mdelay(10);
    }

    hc->op_regs->usb_cmd |= CMD_HCRESET;

    while (hc->op_regs->usb_cmd & CMD_HCRESET)
    {
        mdelay(10);
    }

    log_debug("[EHCI] Controller reset");
}

static void ehci_set_64_bit_mode(ehci_controller_t *hc)
{
    hc->cap_regs->hcc_params |= (HCCPARAMS_64_BIT);
}

static void ehci_disable_legacy_support(uint32_t id, ehci_controller_t *hc)
{
    log_debug("[EHCI] Disabling legacy support");

    uint32_t eecp = (hc->cap_regs->hcc_params & HCCPARAMS_EECP_MASK) >> HCCPARAMS_EECP_SHIFT;

    if (eecp >= 0x40)
    {
        log_debug("[EHCI] eecp >= 0x40");

        uint32_t legsup = pci_read_l(id, eecp + USBLEGSUP);

        if (legsup & USBLEGSUP_HC_BIOS)
        {
            log_debug("[EHCI] Disabling BIOS semaphore");

            pci_write_l(id, eecp + USBLEGSUP, legsup | USBLEGSUP_HC_OS);

            for (;;)
            {
                legsup = pci_read_l(id, eecp + USBLEGSUP);

                if (~legsup & USBLEGSUP_HC_BIOS && legsup & USBLEGSUP_HC_OS)
                {
                    log_debug("[EHCI] OS semaphore set");

                    break;
                }
            }
        }
    }
}

#define WAIT_FOR_NOT_SET(reg, bit) \
    while (reg & bit)              \
        ;

static void ehci_controller_init_op_regs(ehci_controller_t *hc)
{
    hc->op_regs->usb_intr = 0;
    hc->op_regs->frame_index = 0;
    hc->op_regs->periodic_list_base = (uint32_t)((uintptr_t)hc->frame_list);
    hc->op_regs->async_list_addr = (uint32_t)((uintptr_t)hc->async_qh);

    // Note that we always allocate the structs in the <4GB region of the RAM,
    // which always leaves this register empty.
    hc->op_regs->ctrl_ds_segment = 0;
    hc->op_regs->usb_status = 0x3F;
    hc->op_regs->usb_cmd = (8 << CMD_ITC_SHIFT) | CMD_PSE | CMD_ASE | CMD_RS;
    WAIT_FOR_NOT_SET(hc->op_regs->usb_status, STS_HCHALTED);
    hc->op_regs->config_flag = 1;
}

static ehci_controller_t *ehci_init_hc(uint32_t id, uint64_t port_addr)
{
    ehci_controller_t *hc = malloc(sizeof(ehci_controller_t));

    if (!hc)
    {
        return NULL;
    }

    log_debug("[EHCI] Allocating memory");

    void *frameListBlock = ehci_alloc_physmem();
    void *qhBlock = ehci_alloc_physmem();
    void *tdBlock = ehci_alloc_physmem();

    if (!frameListBlock)
    {
        log_error("[EHCI] Failed to allocate physical memory");
        return NULL;
    }

    if (!qhBlock)
    {
        log_error("[EHCI] Failed to allocate physical memory");
        return NULL;
    }

    if (!tdBlock)
    {
        log_error("[EHCI] Failed to allocate physical memory");
        return NULL;
    }

    log_debug("[EHCI] Framelist block: %#016x", frameListBlock);
    log_debug("[EHCI] QH block: %#016x", qhBlock);
    log_debug("[EHCI] TD block: %#016x", tdBlock);

    hc->cap_regs = (ehci_cap_regs_t *)(uintptr_t)port_addr;
    hc->op_regs = (ehci_op_regs_t *)(uintptr_t)(port_addr + hc->cap_regs->cap_length);
    hc->frame_list = (uint32_t *)frameListBlock;
    hc->qh_pool = (ehci_qh_t *)qhBlock;
    hc->td_pool = (ehci_td_t *)tdBlock;

    if (((uint64_t)hc->td_pool & 0x1F) > 0)
    {
        log_warn("TD misaligned");
    }

    memset(hc->qh_pool, 0, sizeof(ehci_qh_t) * MAX_QH);
    memset(hc->td_pool, 0, sizeof(ehci_td_t) * MAX_TD);

    hc->async_qh = ehci_alloc_async_qh(hc);
    hc->periodic_qh = ehci_alloc_periodic_qh(hc);

    log_debug("[EHCI] Creating framelist");

    for (uint32_t i = 0; i < 1024; ++i)
    {
        hc->frame_list[i] = PTR_QH | (uint32_t)(uintptr_t)hc->periodic_qh;
    }

    ehci_reset_controller(hc);

    ehci_set_64_bit_mode(hc);
    ehci_disable_legacy_support(id, hc);

    ehci_controller_init_op_regs(hc);

    mdelay(5);

    log_debug("[EHCI] Probing devices");

    ehci_probe(hc);

    return hc;
}

//==============================================================================
// Interface functions
//==============================================================================

void usb_ehci_init(uint32_t id, PciDeviceInfo_t *dev_info)
{
    if (!((((dev_info->classCode << 8) | dev_info->subClass) == PCI_SERIAL_USB) &&
          (dev_info->progIntf == PCI_SERIAL_USB_EHCI)))
    {
        return;
    }

    log_info("[USB] Initializing EHCI");

    if (sizeof(ehci_qh_t) != 128)
    {
        log_error("[EHCI] Unexpected QH size: %i", sizeof(ehci_qh_t));

        return;
    }

    uint64_t port_addr = ehci_get_port_address(dev_info);

    ehci_controller_t *hc = ehci_init_hc(id, port_addr);

    if (!hc)
    {
        log_error("[EHCI] Failed to initialize HC");
        return;
    }

    log_debug("[EHCI] Allocating controller");

    usb_controller_t *controller = (usb_controller_t *)malloc(sizeof(usb_controller_t));

    if (!controller)
    {
        log_error("[EHCI] Failed to allocate memory for usb controller");
        return;
    }

    controller->next = usb_get_controller_list();
    controller->hc = hc;
    controller->poll = ehci_controller_poll;

    usb_set_controller_list(controller);

    log_info("[USB] EHCI Done");
}

//=============================================================================
// End of file
//=============================================================================