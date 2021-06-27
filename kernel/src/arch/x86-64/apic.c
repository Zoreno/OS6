/**
 * @file apic.c
 * @author Joakim Bertils
 * @version 0.1
 * @date 2021-06-27
 *
 * @brief Handles the processor local and I/O interrupt controllers.
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

#include <acpi/acpi.h>
#include <arch/x86-64/apic.h>
#include <arch/x86-64/cpu.h>
#include <arch/x86-64/msr.h>
#include <logging/logging.h>
#include <mm/virt_mem.h>
#include <util/mmio.h>

//=============================================================================
// Private function forward declarations
//=============================================================================

static void lapic_initialize();

//=============================================================================
// Private variables
//=============================================================================

static uintptr_t lapic_base_physical = 0;
static uintptr_t lapic_base = 0;

//=============================================================================
// Private functions
//=============================================================================

static uint32_t lapic_read(uint32_t reg)
{
    return mmio_read32((uintptr_t)lapic_base + reg);
}

static void lapic_write(uint32_t reg, uint32_t value)
{
    mmio_write32((uintptr_t)lapic_base + reg, value);
}

static void lapic_initialize()
{
    if (!arch_x86_64_cpu_query_feature(CPU_FEAT_APIC))
    {
        log_error("[LAPIC] LAPIC not supported according to CPUID. Halting...");

        for (;;)
            ;
    }

    // TODO: This is a hack mapping it to the page mapping area as it is located
    // in the bottom 1 GB of the main memory area.
    madt_t *madt = (madt_t *)(0xFFFF880000000000 + (uint8_t *)acpi_get_madt());

    log_info("MADT Physical Address: %#016x", madt);

    if (!madt)
    {
        log_error(
            "[LAPIC] Failed to get MADT table. Cannot initialize LAPIC. "
            "Halting...");

        for (;;)
            ;
    }

    lapic_base_physical = madt_get_apic_addr(madt);

    // Map the virtual address to the same location for now...
    lapic_base = lapic_base_physical;

    // Identity map the LAPIC register memory area
    virt_mem_map_page(
        (void *)lapic_base, (void *)lapic_base_physical, VIRT_MEM_WRITABLE);

    log_info("[LAPIC] Mapping LAPIC registers at virtual address %#016x",
             lapic_base);

    uint32_t cpu_id = lapic_read(LAPIC_REG_ID);

    log_info("[LAPIC] CPU ID: %i", cpu_id);
}

//=============================================================================
// Interface functions
//=============================================================================

void apic_initialize()
{
    lapic_initialize();
}

void apic_eoi()
{
    lapic_write(LAPIC_REG_EOI, 0);
}

uint8_t apic_current_processor_id()
{
    return lapic_read(LAPIC_REG_ID) >> 24;
}

void apic_enable()
{
    uint64_t msr_value = rdmsr(MSR_APIC);

    msr_value |= LAPIC_ENABLE;
    msr_value &= ~(1 << 10);  // TODO: Give this bit a name

    wrmsr(MSR_APIC, msr_value);
}

//=============================================================================
// End of file
//=============================================================================
