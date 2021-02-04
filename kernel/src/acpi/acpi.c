/**
 * @file acpi.c
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

#include <acpi/acpi.h>

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <acpi/acpi_header.h>
#include <acpi/dsdt.h>
#include <acpi/fadt.h>
#include <acpi/madt.h>
#include <acpi/rsdp.h>
#include <acpi/rsdt.h>
#include <acpi/xsdt.h>
#include <arch/arch.h>
#include <logging/logging.h>
#include <util/hexdump.h>

//=============================================================================
// Macros
//=============================================================================

#ifndef DEBUG_ACPI
#define DEBUG_ACPI 0
#endif

//=============================================================================
// Data types
//=============================================================================

/**
 * @brief Describes a system power state.
 * 
 * 
 */
typedef struct
{
    /**
     * @brief Non-zero if valid, that is if the values has been initialized.
     * 
     * 
     */
    int valid;

    /**
     * @brief Copy of the signature of the state.
     * 
     * 
     */
    char signature[5];

    /**
     * @brief Value to store in the PM1a_CNT register.
     * 
     * 
     */
    uint16_t SLP_TYPa;

    /**
     * @brief Value to store in the PM1b_CNT register.
     * 
     * 
     */
    uint16_t SLP_TYPb;
} acpi_system_state_t;

//=============================================================================
// Variables
//=============================================================================

uint32_t SMI_CMD;
uint8_t ACPI_ENABLE;
uint8_t ACPI_DISABLE;
uint32_t PM1a_CNT;
uint32_t PM1b_CNT;
uint16_t SLP_EN;
uint16_t SCI_EN;
uint8_t PM1_CNT_LEN;
uint8_t century_register;

acpi_system_state_t S3_state;
acpi_system_state_t S4_state;
acpi_system_state_t S5_state;

//=============================================================================
// Forward declarations
//=============================================================================

#if DEBUG_ACPI == 1
static void acpi_print_rsdp(rsdp_t *rsdp);
static void acpi_print_acpi_header(const acpi_header_t *header);
static const char *acpi_gas_address_space_to_string(uint8_t as);
static const char *acpi_gas_access_size_to_string(uint8_t access_size);
static void acpi_print_gas(const generic_address_structure_t *gas);
#endif

static rsdp_t *acpi_check_rsdp(unsigned int *ptr);
static rsdp_t *acpi_get_rsdp(void);

static int acpi_parse_fadt(fadt_t *fadt);
static int acpi_parse_dsdt(dsdt_t *dsdt);

static int acpi_check_header(acpi_header_t *header, char *signature);
static uint8_t acpi_calculate_checksum(acpi_header_t *header);

static int acpi_enable(void);

//=============================================================================
// Private functions
//=============================================================================

#if DEBUG_ACPI == 1
static void acpi_print_rsdp(rsdp_t *rsdp)
{
    char sig[9];
    char oem[7];

    memset(sig, 0, sizeof(sig));
    memset(oem, 0, sizeof(oem));
    memcpy(sig, rsdp->signature, 8);
    memcpy(oem, rsdp->oemid, 6);

    printf("Signature: %s\n", sig);
    printf("Checksum: %i\n", rsdp->checksum);
    printf("OEM_ID: %s\n", oem);
    printf("Revision: %i\n", rsdp->revision);
    printf("RSDT Address: 0x%08x\n", rsdp->rsdt_address);
    printf("Length: %i\n", rsdp->length);
    printf("XSDT Address: 0x%016x\n", rsdp->xsdt_address);
    printf("Extended checksum: %i\n", rsdp->extended_checksum);
}

static void acpi_print_acpi_header(const acpi_header_t *header)
{
    char sig[5];
    char oem[7];

    memset(sig, 0, sizeof(sig));
    memset(oem, 0, sizeof(oem));
    memcpy(sig, header->signature, 4);
    memcpy(oem, header->oem_id, 6);

    printf("Signature: %s\n", sig);
    printf("Length: %i\n", header->length);
    printf("Revision: %i\n", header->revision);
    printf("Checksum: %i\n", header->checksum);
    printf("OEM_ID: %s\n", oem);
    printf("OEM table id: %i\n", header->oem_table_id);
    printf("OEM revision: %i\n", header->oem_revision);
    printf("Creator id: %i\n", header->creator_id);
    printf("Creator revision: %i\n", header->creator_revision);
}

static const char *acpi_gas_address_space_to_string(uint8_t as)
{
    struct
    {
        uint8_t lower_bound; // Inclusive
        uint8_t upper_bound; // Inclusive
        const char *string;  // Pointer to statically allocated string.
    } values[] =
        {
            {0, 0, "System Memory"},
            {1, 1, "System I/O"},
            {2, 2, "PCI Configuration Space"},
            {3, 3, "Embedded Controller"},
            {4, 4, "SMBus"},
            {5, 0x09, "Reserved"},
            {0x0A, 0x0A, "Platform Communications Channel (PCC)"},
            {0x0B, 0x7E, "Reserved"},
            {0x7F, 0x7F, "Functional Fixed Hardware"},
            {0x80, 0xBF, "Reserved"},
            {0xC0, 0xFF, "OEM Defined"},
        };

    int values_size = sizeof(values) / sizeof(values[0]);

    for (int i = 0; i < values_size; ++i)
    {
        if (as >= values[i].lower_bound && as <= values[i].upper_bound)
        {
            return values[i].string;
        }
    }

    return "Unknown";
}

static const char *acpi_gas_access_size_to_string(uint8_t access_size)
{
    switch (access_size)
    {
    case 0:
        return "Undefined";
    case 1:
        return "Byte";
    case 2:
        return "Word";
    case 3:
        return "Dword";
    case 4:
        return "Qword";
    default:
        break;
    }

    return "Unknown";
}

static void acpi_print_gas(const generic_address_structure_t *gas)
{
    printf("Address space: %i (%s)\n", gas->address_space,
           acpi_gas_address_space_to_string(gas->address_space));
    printf("Bit width: %i\n", gas->bit_width);
    printf("Bit offset: %i\n", gas->bit_offset);
    printf("Access size: %i (%s)\n", gas->access_size,
           acpi_gas_access_size_to_string(gas->access_size));
    printf("Address: 0x%016x\n", gas->address);
}
#endif

static rsdp_t *acpi_check_rsdp(unsigned int *ptr)
{
    char *signature = "RSD PTR ";

    rsdp_t *rsdp = (rsdp_t *)ptr;

    // Check the signature
    if (memcmp(signature, rsdp, 8) == 0)
    {
#if DEBUG_ACPI == 1
        log_debug("[ACPI] Found matching signature");
        acpi_print_rsdp(rsdp);
#endif

        // If matching signature, check checksum
        uint8_t *bptr = (uint8_t *)ptr;
        uint8_t checksum = 0;

        for (int i = 0; i < 24; ++i)
        {
            checksum += *(bptr++);
        }

        if (checksum == 0)
        {
            return rsdp;
        }
#if DEBUG_ACPI == 1
        else
        {
            log_warn("[ACPI] Got checksum: %i", checksum);
        }
#endif
    }

    return 0;
}

static rsdp_t *acpi_get_rsdp(void)
{
    for (unsigned int *addr = (unsigned int *)0x000E0000;
         (long long int)addr < 0x00100000;
         addr += 0x10 / sizeof(addr))
    {
        rsdp_t *rsdp = acpi_check_rsdp(addr);

        if (rsdp)
        {
            return rsdp;
        }
    }

    long long int ebda = *((short *)0x40E);
    ebda = ebda * 0x10 & 0x000FFFFF;

    for (unsigned int *addr = (unsigned int *)ebda;
         (long long int)addr < ebda + 1024;
         addr += 0x10 / sizeof(addr))

    {
        rsdp_t *rsdp = acpi_check_rsdp(addr);

        if (rsdp)
        {
            return rsdp;
        }
    }

    return 0;
}

static int acpi_check_signature(acpi_header_t *header, char *signature)
{
    return memcmp(header->signature, signature, 4);
}

static int acpi_check_header(acpi_header_t *header, char *signature)
{
    // Check against the given signature
    if (acpi_check_signature(header, signature) != 0)
    {
        return -1;
    }

    // Calculate checksum
    if (acpi_calculate_checksum(header) != 0)
    {
        return -1;
    }

    return 0;
}

static uint8_t acpi_calculate_checksum(acpi_header_t *header)
{
    uint8_t *bptr = (uint8_t *)header;
    uint8_t sum = 0;

    for (int i = 0; i < header->length; ++i)
    {
        sum += *(bptr++);
    }

    return sum;
}

static int acpi_parse_fadt(fadt_t *fadt)
{
#if DEBUG_ACPI == 1
    log_debug("[ACPI] Found FADT!");
#endif

    century_register = fadt->century;
    SMI_CMD = fadt->SMI_command_port;
    ACPI_ENABLE = fadt->acpi_enable;
    ACPI_DISABLE = fadt->acpi_disable;
    PM1a_CNT = fadt->PM1a_control_block;
    PM1b_CNT = fadt->PM1b_control_block;
    PM1_CNT_LEN = fadt->PM1_control_length;
    SLP_EN = 1 << 13;
    SCI_EN = 1;

    acpi_header_t *dsdt_header = (acpi_header_t *)(uint64_t)fadt->dsdt;

    if (acpi_check_signature(dsdt_header, "DSDT") == 0)
    {
        acpi_parse_dsdt((dsdt_t *)dsdt_header);
    }

    return 0;
}

static int acpi_find_system_state(const char *signature,
                                  char *dsdt_block,
                                  int dsdt_length,
                                  acpi_system_state_t *state)
{
    while (0 < dsdt_length--)
    {
        if (memcmp(dsdt_block, signature, strlen(signature)) == 0)
        {
            break;
        }
        dsdt_block++;
    }

    if (dsdt_length > 0)
    {
        if ((*(dsdt_block - 1) == 0x08 ||
             (*(dsdt_block - 2) == 0x08 && *(dsdt_block - 1) == '\\')) &&
            *(dsdt_block + 4) == 0x12)
        {
            dsdt_block += 5;
            dsdt_block += ((*dsdt_block & 0xC0) >> 6) + 2;
            if (*dsdt_block == 0x0A)
            {
                ++dsdt_block;
            }
            state->SLP_TYPa = *(dsdt_block) << 10;
            ++dsdt_block;
            if (*dsdt_block == 0x0A)
            {
                ++dsdt_block;
            }
            state->SLP_TYPb = *(dsdt_block) << 10;
            memset(state->signature, 0, sizeof(state->signature));
            memcpy(state->signature, signature, strlen(signature));
            state->valid = 1;

#if DEBUG_ACPI == 1
            log_trace("[ACPI] %s SLP_TYPa: %i, SLP_TYPb: %i",
                      signature, state->SLP_TYPa, state->SLP_TYPb);
#endif
        }
        else
        {
            log_error("[ACPI] %s parse error", signature);
            return 1;
        }
    }
    else
    {
        log_warn("[ACPI] %s not present", signature);
        return 1;
    }

    return 0;
}

static int acpi_parse_dsdt(dsdt_t *dsdt)
{
#if DEBUG_ACPI == 1
    log_debug("[ACPI] Found DSDT!");
    acpi_print_acpi_header(&dsdt->header);
#endif

    char *dsdt_block = (char *)dsdt->definition_block;
    int dsdt_length = dsdt->header.length - sizeof(acpi_header_t);

#if DEBUG_ACPI == 1
    hexdump(dsdt_block, dsdt_length);
#endif

    acpi_find_system_state("_S5_", dsdt_block, dsdt_length, &S5_state);

    return 0;
}

static int acpi_parse_madt(madt_t *madt)
{
    if (!madt)
    {
        log_error("[ACPI] MADT was NULL");
        return 1;
    }

    if (acpi_check_header(&madt->header, "APIC") != 0)
    {
        log_error("[ACPI] MADT header signature was invalid");
        return 1;
    }

#if DEBUG_ACPI == 1
    log_debug("[ACPI] Found valid MADT");
#endif

    log_debug("[ACPI] Local ACPI addr: %#08x", madt->local_apic_addr);
    log_debug("[ACPI] Flags: %#08x", madt->flags);

    uint8_t *p = (uint8_t *)madt;
    p += sizeof(madt_t);
    uint8_t *p_end = (uint8_t *)madt;
    p_end += madt->header.length;

    while (p < p_end)
    {
        madt_entry_header_t* entry = (madt_entry_header_t *)p;
        switch(entry->entry_type)
        {
            case APIC_TYPE_LOCAL_APIC:
            {
                madt_entry_lapic_t* lapic = (madt_entry_lapic_t *)p;
                log_info("LAPIC processor ID: %i", lapic->acpi_processor_id);
                log_info("LAPIC APIC ID: %i", lapic->apic_id);
                log_info("LAPIC flags: %#08x", lapic->flags);
            }
            break;
        default:
            log_warn("[ACPI] Invalid madt entry type");
            break;
        }
        p += entry->record_length;
    }

    return 0;
}

static int acpi_init_xsdt(xsdt_t *xsdt)
{
    if (!xsdt)
    {
        log_error("[ACPI] XSDT was NULL");
        return 1;
    }

    if (acpi_check_header(&xsdt->header, "XSDT") != 0)
    {
        log_error("[ACPI] XSDT header signature invalid");
        return 1;
    }

#if DEBUG_ACPI == 1
    log_debug("[ACPI] XSDT valid!");
    acpi_print_acpi_header(&xsdt->header);
#endif

    int entries = (xsdt->header.length - sizeof(acpi_header_t)) / sizeof(xsdt->tables[0]);

    for (int i = 0; i < entries; ++i)
    {
        acpi_header_t *header = (acpi_header_t *)(uint64_t)xsdt->tables[i];
#if DEBUG_ACPI == 1
        log_debug("[ACPI] Header %i:", i);
        acpi_print_acpi_header(header);
#endif
        if (acpi_check_signature(header, "FACP") == 0)
        {
            acpi_parse_fadt((fadt_t *)header);
        }

        if (acpi_check_signature(header, "APIC") == 0)
        {
            acpi_parse_madt((madt_t *)header);
        }
    }

    return 0;
}

static int acpi_init_rsdt(rsdt_t *rsdt)
{
    if (!rsdt)
    {
        log_error("[ACPI] RSDT was NULL");
        return 1;
    }

    if (acpi_check_header(&rsdt->header, "RSDT") != 0)
    {
        log_error("[ACPI] RSDT header signature invalid");
        return 1;
    }

#if DEBUG_ACPI == 1
    log_debug("[ACPI] RSDT valid!");
    acpi_print_acpi_header(&rsdt->header);
#endif

    int entries = (rsdt->header.length - sizeof(acpi_header_t)) / sizeof(rsdt->tables[0]);

    for (int i = 0; i < entries; ++i)
    {
        acpi_header_t *header = (acpi_header_t *)(uint64_t)rsdt->tables[i];
#if DEBUG_ACPI == 1
        log_debug("[ACPI] Header %i:", i);
        acpi_print_acpi_header(header);
#endif
        if (acpi_check_signature(header, "FACP") == 0)
        {
            acpi_parse_fadt((fadt_t *)header);
        }

        if (acpi_check_signature(header, "APIC") == 0)
        {
            acpi_parse_madt((madt_t *)header);
        }
    }

    return 0;
}

static int acpi_is_enabled()
{
    return (inportw((uint64_t)PM1a_CNT) & SCI_EN) != 0;
}

static void acpi_set_enabled(int enabled)
{
    uint8_t command = enabled ? ACPI_ENABLE : ACPI_DISABLE;
    outportb((uint64_t)SMI_CMD, command);
}

static void acpi_sleep(int ms)
{
    mdelay(ms);
}

static int acpi_enable(void)
{
    int tries = 300;

    if (!acpi_is_enabled())
    {
        if (SMI_CMD == 0 || SCI_EN == 0)
        {
            log_warn("[ACPI] No known way to enable ACPI");
            return 1;
        }

        acpi_set_enabled(1);

        for (int i = 0; i < tries; ++i)
        {
            if (acpi_is_enabled())
            {
                log_info("[ACPI] ACPI enabled!");
                return 0;
            }

            acpi_sleep(10);
        }

        log_error("[ACPI] ACPI could not be enabled");
        return 1;
    }

    log_info("[ACPI] ACPI already enabled");

    return 0;
}

static void acpi_set_state(acpi_system_state_t *state)
{
    if (PM1a_CNT != 0)
    {
        outportw((uint64_t)PM1a_CNT, state->SLP_TYPa | SLP_EN);
    }

    if (PM1b_CNT != 0)
    {
        outportw((uint64_t)PM1b_CNT, state->SLP_TYPb | SLP_EN);
    }
}

//=============================================================================
// Interface functions
//=============================================================================

int acpi_init()
{
    rsdp_t *rsdp = acpi_get_rsdp();

    if (rsdp)
    {
#if DEBUG_ACPI == 1
        log_debug("[ACPI] Found RSDP!");
#endif

        // Always try with the XSDT address first if present, otherwise
        // use the RSDT address.
        if (rsdp->xsdt_address != 0)
        {
            int ret = acpi_init_xsdt((xsdt_t *)(uint64_t)rsdp->xsdt_address);

            if (!ret)
            {
                goto done;
            }
        }

        if (rsdp->rsdt_address != 0)
        {
            int ret = acpi_init_rsdt((rsdt_t *)(uint64_t)rsdp->rsdt_address);

            if (!ret)
            {
                goto done;
            }
        }

        log_warn("[ACPI] Failed to find either XSDT or RSDT");
        return 1;
    }
    else
    {
        log_warn("[ACPI] Did not find RSDP!");
        return 1;
    }

done:

    log_info("[ACPI] ACPI Initialized!");
    return 0;
}

void acpi_power_off()
{
    if (SCI_EN == 0)
    {
        return;
    }

    acpi_enable();

    arch_shutdown();

    // S5 is the closest we can get to a complete shutdown without pulling
    // pulling the power cord.
    acpi_set_state(&S5_state);

    mdelay(3000);

    log_error("[ACPI] Poweroff failed");
}

uint8_t acpi_get_century_register()
{
    return century_register;
}

//=============================================================================
// End of file
//=============================================================================