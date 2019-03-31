#include <mm/virt_mem.h>

#include <string.h>
#include <stdio.h>

// https://github.com/thibault-reigner/userland_slab

#define PAGE_SIZE 4096

static pml4_t *_cur_dir = 0;

void pt_entry_add_attrib(pt_entry *e, uint64_t attrib)
{
    *e |= attrib;
}

void pt_entry_del_attrib(pt_entry *e, uint64_t attrib)
{
    *e &= ~attrib;
}

void pt_entry_set_frame(pt_entry *e, phys_addr addr)
{
    *e = (*e & ~PTE_FRAME) | addr;
}

void pd_entry_add_attrib(pd_entry *e, uint64_t attrib)
{
    *e |= attrib;
}

void pd_entry_del_attrib(pd_entry *e, uint64_t attrib)
{
    *e &= ~attrib;
}

void pd_entry_set_frame(pd_entry *e, phys_addr addr)
{
    *e = (*e & ~PDE_FRAME) | addr;
}

void pdp_entry_add_attrib(pdp_entry *e, uint64_t attrib)
{
    *e |= attrib;
}

void pdp_entry_del_attrib(pdp_entry *e, uint64_t attrib)
{
    *e &= ~attrib;
}

void pdp_entry_set_frame(pdp_entry *e, phys_addr addr)
{
    *e = (*e & ~PDPE_FRAME) | addr;
}

void pml4_entry_add_attrib(pml4_entry *e, uint64_t attrib)
{
    *e |= attrib;
}

void pml4_entry_del_attrib(pml4_entry *e, uint64_t attrib)
{
    *e &= ~attrib;
}

void pml4_entry_set_frame(pml4_entry *e, phys_addr addr)
{
    *e = (*e & ~PML4E_FRAME) | addr;
}

void virt_mem_initialize()
{
    printf("[VMM] Initializing Virtual memory manager...\n");

    // Allocate a page table for the 2MB identity mapping

    ptable_t *table = (ptable_t *)phys_mem_alloc_block();
    if (!table)
    {
        // TODO: This should be a kernel panic
        return;
    }

    memset(table, 0, sizeof(ptable_t));

    uint64_t frame = 0;
    uint64_t virt = 0;

    for (uint64_t i = 0; i < PT_ENTRIES; ++i, frame += 0x1000, virt += 0x1000)
    {
        pt_entry entry = 0;
        pt_entry_add_attrib(&entry, PTE_PRESENT);
        pt_entry_add_attrib(&entry, PTE_WRITABLE);
        pt_entry_set_frame(&entry, (phys_addr)frame);

        table->entries[PT_INDEX(virt)] = entry;
    }

    // Allocate a page directory

    pdirectory_t *dir = (pdirectory_t *)phys_mem_alloc_block();

    if (!dir)
    {
        // TODO: This should be a kernel panic
        return;
    }

    memset(dir, 0, sizeof(pdirectory_t));

    pd_entry *entry_pd = &dir->entries[0];
    pd_entry_add_attrib(entry_pd, PDE_PRESENT);
    pd_entry_add_attrib(entry_pd, PDE_WRITABLE);
    pd_entry_set_frame(entry_pd, (phys_addr)table);

    // Allocate a PDP

    pdp_t *pdp = (pdp_t *)phys_mem_alloc_block();

    if (!pdp)
    {
        // TODO: This should be a kernel panic
        return;
    }

    memset(pdp, 0, sizeof(pdp_t));

    pdp_entry *entry_pdp = &pdp->entries[0];
    pdp_entry_add_attrib(entry_pdp, PDPE_PRESENT);
    pdp_entry_add_attrib(entry_pdp, PDPE_WRITABLE);
    pdp_entry_set_frame(entry_pdp, (phys_addr)dir);

    // Allocate a PML4

    pml4_t *pml4 = (pml4_t *)phys_mem_alloc_block();

    memset(pml4, 0, sizeof(pml4_t));

    if (!pml4)
    {
        // TODO: This should be a kernel panic
        return;
    }

    pml4_entry *entry_pml4 = &pml4->entries[0];
    pml4_entry_add_attrib(entry_pml4, PML4E_PRESENT);
    pml4_entry_add_attrib(entry_pml4, PML4E_WRITABLE);
    pml4_entry_set_frame(entry_pml4, (phys_addr)pdp);

    // Switch to the newly created page mapping structure

    virt_mem_switch_dir(pml4);

    printf("[VMM] VMM initialized!\n");
}

int virt_mem_switch_dir(pml4_t *dir)
{
    if (!dir)
    {
        return 0;
    }

    _cur_dir = dir;

    __asm__ volatile("mov %0,%%rax" ::"r"(dir)
                     : "%rax"); // Move dir to eax

    __asm__ volatile("mov %%rax,%%cr3" ::
                         : "%rax"); // move eax to cr3

    return 1;
}

pml4_t *virt_mem_get_current_dir()
{
    return _cur_dir;
}