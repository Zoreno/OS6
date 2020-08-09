/**
 * @file virt_mem.c
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

#include <mm/virt_mem.h>

#include <string.h>
#include <stdio.h>

// https://github.com/thibault-reigner/userland_slab

#define PAGE_OFFSET 0xFFFF880000000000

#define SAFE_PAGE_OFFSET 1

#if SAFE_PAGE_OFFSET == 1
#define ADD_PAGE_OFFSET(addr) ((void *)((((void *)addr) >= (void *)PAGE_OFFSET) ? ((uint8_t *)addr) : (((uint8_t *)addr) + PAGE_OFFSET)))
#define REMOVE_PAGE_OFFSET(addr) ((void *)((((void *)addr) < (void *)PAGE_OFFSET) ? ((uint8_t *)addr) : (((uint8_t *)addr) - PAGE_OFFSET)))
#else
#define ADD_PAGE_OFFSET(addr) ((void *)(((uint8_t *)addr) + PAGE_OFFSET))
#define REMOVE_PAGE_OFFSET(addr) ((void *)(((uint8_t *)addr) - PAGE_OFFSET))
#endif

static pml4_t *_cur_dir = 0;

//==============================================================================
// Page Table Entry
//==============================================================================

void pt_entry_add_attrib(pt_entry_t *e, uint64_t attrib)
{
    *e |= attrib;
}

void pt_entry_del_attrib(pt_entry_t *e, uint64_t attrib)
{
    *e &= ~attrib;
}

void pt_entry_set_frame(pt_entry_t *e, phys_addr addr)
{
    *e = (*e & ~PTE_FRAME) | addr;
}

int pt_entry_is_present(pt_entry_t e)
{
    return e & PTE_PRESENT;
}

int pt_entry_is_writable(pt_entry_t e)
{
    return e & PTE_WRITABLE;
}

int pt_entry_is_user(pt_entry_t e)
{
    return e & PTE_USER;
}

int pt_entry_is_writethrough(pt_entry_t e)
{
    return e & PTE_WRITETHROUGH;
}

int pt_entry_is_cacheable(pt_entry_t e)
{
    return !(e & PTE_NOT_CAHCEABLE);
}

int pt_entry_is_accessed(pt_entry_t e)
{
    return e & PTE_ACCESS;
}

int pt_entry_is_dirty(pt_entry_t e)
{
    return e & PTE_DIRTY;
}

int pt_entry_is_PAT(pt_entry_t e)
{
    return e & PTE_PAT;
}

phys_addr pt_entry_pfn(pt_entry_t e)
{
    return e & PTE_FRAME;
}

//==============================================================================
// Page Directory Entry
//==============================================================================

void pd_entry_add_attrib(pd_entry_t *e, uint64_t attrib)
{
    *e |= attrib;
}

void pd_entry_del_attrib(pd_entry_t *e, uint64_t attrib)
{
    *e &= ~attrib;
}

void pd_entry_set_frame(pd_entry_t *e, phys_addr addr)
{
    *e = (*e & ~PDE_FRAME) | addr;
}

int pd_entry_is_present(pd_entry_t e)
{
    return e & PDE_PRESENT;
}

int pd_entry_is_writable(pd_entry_t e)
{
    return e & PDE_WRITABLE;
}

int pd_entry_is_user(pd_entry_t e)
{
    return e & PDE_USER;
}

int pd_entry_is_pwt(pd_entry_t e)
{
    return e & PDE_PWT;
}

int pd_entry_is_pcd(pd_entry_t e)
{
    return e & PDE_PCD;
}

int pd_entry_is_accessed(pd_entry_t e)
{
    return e & PDE_ACCESSED;
}

int pd_entry_is_huge(pd_entry_t e)
{
    return e & PDE_4MB;
}

int pd_entry_is_cpu_global(pd_entry_t e)
{
    return e & PDE_CPU_GLOBAL;
}

int pd_entry_is_lv4_global(pd_entry_t e)
{
    return e & PDE_LV4_GLOBAL;
}

phys_addr pd_entry_pfn(pd_entry_t e)
{
    return e & PDE_FRAME;
}

//==============================================================================
// Page Directory Pointer Entry
//==============================================================================

void pdp_entry_add_attrib(pdp_entry_t *e, uint64_t attrib)
{
    *e |= attrib;
}

void pdp_entry_del_attrib(pdp_entry_t *e, uint64_t attrib)
{
    *e &= ~attrib;
}

void pdp_entry_set_frame(pdp_entry_t *e, phys_addr addr)
{
    *e = (*e & ~PDPE_FRAME) | addr;
}

int pdp_entry_is_present(pdp_entry_t e)
{
    return e & PDPE_PRESENT;
}

int pdp_entry_is_writable(pdp_entry_t e)
{
    return e & PDPE_WRITABLE;
}

int pdp_entry_is_user(pdp_entry_t e)
{
    return e & PDPE_USER;
}

int pdp_entry_is_pwt(pdp_entry_t e)
{
    return e & PDPE_PWT;
}

int pdp_entry_is_pcd(pdp_entry_t e)
{
    return e & PDPE_PCD;
}

int pdp_entry_is_accessed(pdp_entry_t e)
{
    return e & PDPE_ACCESSED;
}

int pdp_entry_is_huge(pdp_entry_t e)
{
    return e & PDPE_1GB;
}

phys_addr pdp_entry_pfn(pdp_entry_t e)
{
    return e & PDPE_FRAME;
}

//==============================================================================
// Page Directory Pointer Entry
//==============================================================================

void pml4_entry_add_attrib(pml4_entry_t *e, uint64_t attrib)
{
    *e |= attrib;
}

void pml4_entry_del_attrib(pml4_entry_t *e, uint64_t attrib)
{
    *e &= ~attrib;
}

void pml4_entry_set_frame(pml4_entry_t *e, phys_addr addr)
{
    *e = (*e & ~PML4E_FRAME) | addr;
}

int pml4_entry_is_present(pml4_entry_t e)
{
    return e & PML4E_PRESENT;
}

int pml4_entry_is_writable(pml4_entry_t e)
{
    return e & PML4E_WRITABLE;
}

int pml4_entry_is_user(pml4_entry_t e)
{
    return e & PML4E_USER;
}

int pml4_entry_is_pwt(pml4_entry_t e)
{
    return e & PML4E_PWT;
}

int pml4_entry_is_pcd(pml4_entry_t e)
{
    return e & PML4E_PCD;
}

int pml4_entry_is_accessed(pml4_entry_t e)
{
    return e & PML4E_ACCESSED;
}

phys_addr pml4_entry_pfn(pml4_entry_t e)
{
    return e & PML4E_FRAME;
}

//==============================================================================
// Memory alignment
//==============================================================================

static uint64_t align_up(uint64_t val, uint64_t align);
static uint64_t align_down(uint64_t val, uint64_t align);

static uint64_t align_up(uint64_t val, uint64_t align)
{
    if (!align)
    {
        return val;
    }

    --align;

    return (val + align) & ~align;
}

static uint64_t align_down(uint64_t val, uint64_t align)
{
    if (!align)
    {
        return val;
    }

    return val & ~(align + 1);
}

//==============================================================================
// Virtual Memory Manager
//==============================================================================

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
        pt_entry_t entry = 0;
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

    pd_entry_t *entry_pd = &dir->entries[0];
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

    pdp_entry_t *entry_pdp = &pdp->entries[0];
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

    pml4_entry_t *entry_pml4 = &pml4->entries[0];
    pml4_entry_add_attrib(entry_pml4, PML4E_PRESENT);
    pml4_entry_add_attrib(entry_pml4, PML4E_WRITABLE);
    pml4_entry_set_frame(entry_pml4, (phys_addr)pdp);

    // This is a hack to map the first GB of memory to the address
    // 0xFFFF880000000000. By adding this offset to any physical page,
    // the kernel can reach any physical memory (up to 1 GB).
    pdp_t *pdp_high = (pdp_t *)phys_mem_alloc_block();
    memset(pdp_high, 0, sizeof(pdp_t));
    pdp_entry_t *pdp_high_e = &pdp_high->entries[PDP_INDEX(PAGE_OFFSET)];
    pdp_entry_add_attrib(pdp_high_e, PDPE_PRESENT);
    pdp_entry_add_attrib(pdp_high_e, PDPE_WRITABLE);
    pdp_entry_add_attrib(pdp_high_e, PDPE_1GB);
    pdp_entry_set_frame(pdp_high_e, (phys_addr)0);
    pml4_entry_t *entry_high = &pml4->entries[PML4_INDEX(PAGE_OFFSET)];
    pml4_entry_add_attrib(entry_high, PML4E_PRESENT);
    pml4_entry_add_attrib(entry_high, PML4E_WRITABLE);
    pml4_entry_set_frame(entry_high, (phys_addr)pdp_high);

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

void virt_mem_flush_tlb(virt_addr addr)
{
    __asm__ volatile("cli");
    __asm__ volatile("invlpg (%0)" ::"b"(addr)
                     : "memory");
    __asm__ volatile("sti");
}

void *virt_mem_get_physical_addr(void *addr, pml4_t *dir)
{
    virt_addr vaddr = (virt_addr)addr;

    //=========================================================================
    // PML4 table
    //=========================================================================

    pml4_t *current_dir = dir == NULL ? dir : _cur_dir;

    if (!current_dir)
    {
        return NULL;
    }

    current_dir = ADD_PAGE_OFFSET(current_dir);

    pml4_entry_t entry_pml4 = current_dir->entries[PML4_INDEX(vaddr)];

    if (!pml4_entry_is_present(entry_pml4))
    {
        return NULL;
    }

    //=========================================================================
    // PDP table
    //=========================================================================

    pdp_t *pdp_table = (pdp_t *)pml4_entry_pfn(entry_pml4);

    if (!pdp_table)
    {
        return NULL;
    }

    pdp_table = ADD_PAGE_OFFSET(pdp_table);

    pdp_entry_t entry_pdp = pdp_table->entries[PDP_INDEX(vaddr)];

    if (!pdp_entry_is_present(entry_pdp))
    {
        return NULL;
    }

    //=========================================================================
    // PD table
    //=========================================================================

    pdirectory_t *pdir = (pdirectory_t *)pdp_entry_pfn(entry_pdp);

    if (!pdir)
    {
        return NULL;
    }

    dir = ADD_PAGE_OFFSET(dir);

    pd_entry_t entry_pd = pdir->entries[PD_INDEX(vaddr)];

    if (!pd_entry_is_present(entry_pd))
    {
        return NULL;
    }

    //=========================================================================
    // Page table
    //=========================================================================

    ptable_t *ptable = (ptable_t *)pd_entry_pfn(entry_pd);

    if (!ptable)
    {
        return NULL;
    }

    ptable = ADD_PAGE_OFFSET(ptable);

    pt_entry_t entry_pt = ptable->entries[PT_INDEX(vaddr)];

    if (!pt_entry_is_present(entry_pt))
    {
        return NULL;
    }

    phys_addr paddr = pt_entry_pfn(entry_pt);

    paddr |= vaddr & 0xFFF;

    return (void *)paddr;
}

void *virt_mem_get_physical_addr_cur(void *addr)
{
    return virt_mem_get_physical_addr(addr, virt_mem_get_current_dir());
}

pml4_t *virt_mem_create_address_space()
{
    pml4_t *dir = (pml4_t *)phys_mem_alloc_block();

    if (!dir)
    {
        return dir;
    }

    dir = ADD_PAGE_OFFSET(dir);

    memset(dir, 0, sizeof(pml4_t));

    return dir;
}

void virt_mem_clear_pt(ptable_t *table)
{
    table = ADD_PAGE_OFFSET(table);

    memset(table, 0, sizeof(ptable_t));
}

void virt_mem_clear_pd(pdirectory_t *pdir)
{
    pdir = ADD_PAGE_OFFSET(pdir);

    for (int i = 0; i < PD_ENTRIES; ++i)
    {
        pd_entry_t entry = pdir->entries[i];

        if (pd_entry_is_present(entry))
        {
            ptable_t *ptable = (ptable_t *)pd_entry_pfn(entry);

            virt_mem_clear_pt(ptable);

            phys_mem_free_block(ptable);
        }
    }

    memset(pdir, 0, sizeof(pdirectory_t));
}

void virt_mem_clear_pdp(pdp_t *pdp)
{
    pdp = ADD_PAGE_OFFSET(pdp);

    for (int i = 0; i < PDP_ENTRIES; ++i)
    {
        pdp_entry_t entry = pdp->entries[i];

        if (pdp_entry_is_present(entry))
        {
            pdirectory_t *pdirectory = (pdirectory_t *)pdp_entry_pfn(entry);

            virt_mem_clear_pd(pdirectory);

            phys_mem_free_block(pdirectory);
        }
    }

    memset(pdp, 0, sizeof(pdp_t));
}

void virt_mem_destroy_address_space(pml4_t *dir)
{
    dir = ADD_PAGE_OFFSET(dir);

    for (int i = 0; i < PML4_ENTRIES; ++i)
    {
        pml4_entry_t e = dir->entries[i];

        if (pml4_entry_is_present(e))
        {
            pdp_t *pdp = (pdp_t *)pml4_entry_pfn(e);

            virt_mem_clear_pdp(pdp);

            phys_mem_free_block(pdp);
        }
    }

    memset(dir, 0, sizeof(pml4_t));

    phys_mem_free_block(dir);
}

ptable_t *virt_mem_alloc_ptable()
{
    ptable_t *p = (ptable_t *)phys_mem_alloc_block();

    if (!p)
    {
        return p;
    }

    memset(ADD_PAGE_OFFSET(p), 0, sizeof(ptable_t));

    return p;
}

pdirectory_t *virt_mem_alloc_pdirectory()
{
    pdirectory_t *p = (pdirectory_t *)phys_mem_alloc_block();

    if (!p)
    {
        return p;
    }

    memset(ADD_PAGE_OFFSET(p), 0, sizeof(pdirectory_t));

    return p;
}

pdp_t *virt_mem_alloc_pdp()
{
    pdp_t *p = (pdp_t *)phys_mem_alloc_block();

    if (!p)
    {
        return p;
    }

    memset(ADD_PAGE_OFFSET(p), 0, sizeof(pdp_t));

    return p;
}

pml4_t *virt_mem_alloc_pml4()
{
    pml4_t *p = (pml4_t *)phys_mem_alloc_block();

    if (!p)
    {
        return p;
    }

    memset(ADD_PAGE_OFFSET(p), 0, sizeof(pml4_t));

    return p;
}

void virt_mem_print_cur_dir()
{
    virt_mem_print_dir(_cur_dir);
}

static void virt_mem_print_pt(ptable_t *pt, uint64_t pml4_entry, uint64_t pdp_entry, uint64_t pd_entry)
{
    pt = ADD_PAGE_OFFSET(pt);

    for (int i = 0; i < PT_ENTRIES; ++i)
    {
        if (pt->entries[i])
        {
            printf("PT Entry %#03x: %s%s%s%s%s",
                   i,
                   pt_entry_is_present(pt->entries[i]) ? "P" : "-",  // Present
                   pt_entry_is_writable(pt->entries[i]) ? "W" : "-", // Writable
                   pt_entry_is_user(pt->entries[i]) ? "U" : "-",     // User
                   pt_entry_is_accessed(pt->entries[i]) ? "A" : "-", // Accessed
                   pt_entry_is_dirty(pt->entries[i]) ? "D" : "-");   // Dirty

            uint64_t virt = (0xFFFFULL << 48) + (pml4_entry << 39) + (pdp_entry << 30) + (pd_entry << 21) + (i << 12);
            uint64_t phys = pt_entry_pfn(pt->entries[i]);

            printf(" %#016x mapped to %#016x (%#016x bytes)\n", virt, phys, 1 << 12);
        }
    }
}

static void virt_mem_print_pd(pdirectory_t *pd, uint64_t pml4_entry, uint64_t pdp_entry)
{
    pd = ADD_PAGE_OFFSET(pd);

    for (int i = 0; i < PD_ENTRIES; ++i)
    {
        if (pd->entries[i])
        {
            printf("PD Entry %#03x: %s%s%s%s%s%s%s%s%s",
                   i,
                   pd_entry_is_present(pd->entries[i]) ? "P" : "-",     // Present
                   pd_entry_is_writable(pd->entries[i]) ? "W" : "-",    // Writable
                   pd_entry_is_user(pd->entries[i]) ? "U" : "-",        // User
                   pd_entry_is_pwt(pd->entries[i]) ? "T" : "-",         // Write through
                   pd_entry_is_pcd(pd->entries[i]) ? "C" : "-",         // Cache disable
                   pd_entry_is_accessed(pd->entries[i]) ? "A" : "-",    // Accessed
                   pd_entry_is_huge(pd->entries[i]) ? "H" : "-",        // Huge
                   pd_entry_is_cpu_global(pd->entries[i]) ? "G" : "-",  // CPU Global
                   pd_entry_is_lv4_global(pd->entries[i]) ? "L" : "-"); // LV4 Global

            if (pd_entry_is_huge(pd->entries[i]))
            {
                uint64_t virt = (0xFFFFULL << 48) + (pml4_entry << 39) + (pdp_entry << 30) + (i << 21);
                uint64_t phys = pd_entry_pfn(pd->entries[i]);

                printf(" %#016x mapped to %#016x (%#016x bytes)\n", virt, phys, 1 << 21);
            }
            else
            {
                printf("\n");

                ptable_t *pt = (ptable_t *)pd_entry_pfn(pd->entries[i]);

                if (pdp_entry == 0x3F)
                {
                    virt_mem_print_pt(pt, pml4_entry, pdp_entry, i);
                }
            }
        }
    }
}

static void virt_mem_print_pdp(pdp_t *pdp, uint64_t pml4_entry)
{
    pdp = ADD_PAGE_OFFSET(pdp);

    for (int i = 0; i < PDP_ENTRIES; ++i)
    {
        if (pdp->entries[i])
        {
            printf("PDP Entry %#03x: %s%s%s%s%s%s%s",
                   i,
                   pdp_entry_is_present(pdp->entries[i]) ? "P" : "-",  // Present
                   pdp_entry_is_writable(pdp->entries[i]) ? "W" : "-", // Writable
                   pdp_entry_is_user(pdp->entries[i]) ? "U" : "-",     // User
                   pdp_entry_is_pwt(pdp->entries[i]) ? "T" : "-",      // Write through
                   pdp_entry_is_pcd(pdp->entries[i]) ? "C" : "-",      // Cache disable
                   pdp_entry_is_accessed(pdp->entries[i]) ? "A" : "-", // Accessed
                   pdp_entry_is_huge(pdp->entries[i]) ? "H" : "-");    // Huge

            if (pdp_entry_is_huge(pdp->entries[i]))
            {
                uint64_t virt = (0xFFFFULL << 48) + (pml4_entry << 39) + (i << 30);
                uint64_t phys = pdp_entry_pfn(pdp->entries[i]);

                printf(" %#016x mapped to %#016x (%#016x bytes)\n", virt, phys, 1 << 30);
            }
            else
            {
                printf("\n");
                pdirectory_t *pd = (pdirectory_t *)pdp_entry_pfn(pdp->entries[i]);

                virt_mem_print_pd(pd, pml4_entry, i);
            }
        }
    }
}

void virt_mem_print_dir(pml4_t *dir)
{
    dir = ADD_PAGE_OFFSET(dir);

    for (int i = 0; i < PML4_ENTRIES; ++i)
    {
        if (dir->entries[i])
        {
            printf("PML4 Entry %#03x: %s%s%s%s%s%s",
                   i,
                   pml4_entry_is_present(dir->entries[i]) ? "P" : "-",   // Present
                   pml4_entry_is_writable(dir->entries[i]) ? "W" : "-",  // Writable
                   pml4_entry_is_user(dir->entries[i]) ? "U" : "-",      // User
                   pml4_entry_is_pwt(dir->entries[i]) ? "T" : "-",       // Write through
                   pml4_entry_is_pcd(dir->entries[i]) ? "C" : "-",       // Cache disable
                   pml4_entry_is_accessed(dir->entries[i]) ? "A" : "-"); // Accessed

            printf("\n");

            pdp_t *pdp = (pdp_t *)pml4_entry_pfn(dir->entries[i]);

            virt_mem_print_pdp(pdp, i);
        }
    }
}

int virt_mem_map_page_p(void *phys, void *virt, uint64_t flags, pml4_t *pml4)
{
    virt_addr vaddr = (virt_addr)virt;

    pdp_t *pdp = 0;
    pdirectory_t *pdir = 0;
    ptable_t *ptable = 0;

    pml4 = ADD_PAGE_OFFSET(pml4);

    //=========================================================================
    // PML4 table
    //=========================================================================

    pml4_entry_t *pml4_entry = &pml4->entries[PML4_INDEX(vaddr)];

    if (!pml4_entry_is_present(*pml4_entry))
    {
        pdp = virt_mem_alloc_pdp();

        pml4_entry_add_attrib(pml4_entry, PML4E_PRESENT);

        if (flags & VIRT_MEM_WRITABLE)
        {
            pml4_entry_add_attrib(pml4_entry, PML4E_WRITABLE);
        }

        if (flags & VIRT_MEM_USER)
        {
            pml4_entry_add_attrib(pml4_entry, PML4E_USER);
        }

        pml4_entry_set_frame(pml4_entry, (phys_addr)pdp);
    }
    else
    {
        if (!pml4_entry_is_writable(*pml4_entry) && flags & VIRT_MEM_WRITABLE)
        {
            pml4_entry_add_attrib(pml4_entry, PML4E_WRITABLE);
        }

        if (!pml4_entry_is_user(*pml4_entry) && flags & VIRT_MEM_USER)
        {
            pml4_entry_add_attrib(pml4_entry, PML4E_USER);
        }

        pdp = (pdp_t *)pml4_entry_pfn(*pml4_entry);
    }

    if (!pdp)
    {
        return -1;
    }

    //=========================================================================
    // PDP table
    //=========================================================================

    pdp = ADD_PAGE_OFFSET(pdp);

    pdp_entry_t *pdp_entry = &pdp->entries[PDP_INDEX(vaddr)];

    if (!pdp_entry_is_present(*pdp_entry))
    {
        pdir = virt_mem_alloc_pdirectory();

        pdp_entry_add_attrib(pdp_entry, PDPE_PRESENT);

        if (flags & VIRT_MEM_WRITABLE)
        {
            pdp_entry_add_attrib(pdp_entry, PDPE_WRITABLE);
        }

        if (flags & VIRT_MEM_USER)
        {
            pdp_entry_add_attrib(pdp_entry, PDPE_USER);
        }

        pdp_entry_set_frame(pdp_entry, (phys_addr)pdir);
    }
    else
    {
        if (!pdp_entry_is_writable(*pdp_entry) && flags & VIRT_MEM_WRITABLE)
        {
            pdp_entry_add_attrib(pdp_entry, PDPE_WRITABLE);
        }

        if (!pdp_entry_is_user(*pdp_entry) && flags & VIRT_MEM_USER)
        {
            pdp_entry_add_attrib(pdp_entry, PDPE_USER);
        }

        pdir = (pdirectory_t *)pdp_entry_pfn(*pdp_entry);
    }

    if (!pdir)
    {
        return -1;
    }

    //=========================================================================
    // PD table
    //=========================================================================

    pdir = ADD_PAGE_OFFSET(pdir);

    pd_entry_t *pd_entry = &pdir->entries[PD_INDEX(vaddr)];

    if (!pd_entry_is_present(*pd_entry))
    {
        ptable = virt_mem_alloc_ptable();

        pd_entry_add_attrib(pd_entry, PDE_PRESENT);

        if (flags & VIRT_MEM_WRITABLE)
        {
            pd_entry_add_attrib(pd_entry, PDE_USER);
        }

        if (flags & VIRT_MEM_USER)
        {
            pd_entry_add_attrib(pd_entry, PDE_USER);
        }

        pd_entry_set_frame(pd_entry, (phys_addr)ptable);
    }
    else
    {
        if (!pd_entry_is_writable(*pd_entry) && flags & VIRT_MEM_WRITABLE)
        {
            pd_entry_add_attrib(pd_entry, PDE_WRITABLE);
        }

        if (!pd_entry_is_user(*pd_entry) && flags & VIRT_MEM_USER)
        {
            pd_entry_add_attrib(pd_entry, PDE_USER);
        }

        ptable = (ptable_t *)pd_entry_pfn(*pd_entry);
    }

    if (!ptable)
    {
        return -1;
    }

    //=========================================================================
    // PT table
    //=========================================================================

    ptable = ADD_PAGE_OFFSET(ptable);

    pt_entry_t *pt_entry = &ptable->entries[PT_INDEX(vaddr)];

    // TODO: Handle remap. If there is already a mapped page, let the flags
    // determine if this page should be overwritten or if an error should be raised.
    memset(pt_entry, 0, sizeof(pt_entry_t));

    pt_entry_add_attrib(pt_entry, PTE_PRESENT);

    if (flags & VIRT_MEM_WRITABLE)
    {
        pt_entry_add_attrib(pt_entry, PTE_WRITABLE);
    }

    if (flags & VIRT_MEM_USER)
    {
        pt_entry_add_attrib(pt_entry, PTE_USER);
    }

    pt_entry_set_frame(pt_entry, (phys_addr)phys);
    return 0;
}

int virt_mem_map_page(void *phys, void *virt, uint64_t flags)
{
    return virt_mem_map_page_p(phys, virt, flags, _cur_dir);
}

int virt_mem_map_pages_p(void *phys, void *virt, size_t n_pages, uint64_t flags, pml4_t *dir)
{
    uint64_t paddr = (uint64_t)phys;
    uint64_t vaddr = (uint64_t)virt;

    // TODO: This may be optimized, but it handles all the edge cases with page
    // mappings over page table boundaries.

    for (size_t i = 0; i < n_pages; ++i, paddr += PAGE_SIZE, vaddr += PAGE_SIZE)
    {
        int ret = virt_mem_map_page_p((void *)paddr, (void *)vaddr, flags, dir);

        if (ret)
        {
            return ret;
        }
    }

    return 0;
}

int virt_mem_map_pages(void *phys, void *virt, size_t n_pages, uint64_t flags)
{
    return virt_mem_map_pages_p(phys, virt, n_pages, flags, _cur_dir);
}

int virt_mem_unmap_page(void *virt)
{
    (void)virt;

    // TODO:

    return -1;
}

int virt_mem_unmap_pages(void *virt, size_t n_pages)
{
    (void)virt;
    (void)n_pages;

    // TODO:

    return -1;
}

static void copy_page(void *dst, const void *src)
{
    memcpy(ADD_PAGE_OFFSET(dst),
           ADD_PAGE_OFFSET(src),
           PAGE_SIZE);
}

static ptable_t *clone_ptable(ptable_t *src)
{
    ptable_t *table = virt_mem_alloc_ptable();

    table = ADD_PAGE_OFFSET(table);
    src = ADD_PAGE_OFFSET(src);

    for (int i = 0; i < PT_ENTRIES; ++i)
    {
        if (!src->entries[i])
        {
            continue;
        }

        if (!pt_entry_is_user(src->entries[i]))
        {
            // TODO: Lookup. This will copy accessed and dirty flag. This may not be desireable.
            table->entries[i] = src->entries[i];
        }
        else
        {
            void *src_block = (void *)pt_entry_pfn(src->entries[i]);
            void *dst_block = phys_mem_alloc_block();

            copy_page(dst_block, src_block);

            pt_entry_set_frame(&table->entries[i], (phys_addr)dst_block);

            // Copy the needed flags
            table->entries[i] |= (src->entries[i] & (PTE_ON_CLONE));
        }
    }

    table = REMOVE_PAGE_OFFSET(table);

    return table;
}

static pdirectory_t *clone_pdirectory(pdirectory_t *src)
{
    pdirectory_t *dir = virt_mem_alloc_pdirectory();

    dir = ADD_PAGE_OFFSET(dir);
    src = ADD_PAGE_OFFSET(src);

    for (int i = 0; i < PD_ENTRIES; ++i)
    {
        if (!src->entries[i])
        {
            continue;
        }

        if (!pd_entry_is_user(src->entries[i]))
        {
            // TODO: Lookup. This will copy accessed and dirty flag. This may not be desireable.
            dir->entries[i] = src->entries[i];
        }
        else
        {
            ptable_t *src_table = (ptable_t *)pd_entry_pfn(src->entries[i]);
            ptable_t *new_table = clone_ptable(src_table);

            pd_entry_set_frame(&dir->entries[i], (phys_addr)new_table);

            dir->entries[i] |= (src->entries[i] & (PDE_ON_CLONE));
        }
    }

    dir = REMOVE_PAGE_OFFSET(dir);

    return dir;
}

static pdp_t *clone_pdp(pdp_t *src)
{
    pdp_t *dir = virt_mem_alloc_pdp();

    dir = ADD_PAGE_OFFSET(dir);
    src = ADD_PAGE_OFFSET(src);

    for (int i = 0; i < PDP_ENTRIES; ++i)
    {
        if (!src->entries[i])
        {
            continue;
        }

        if (!pdp_entry_is_user(src->entries[i]))
        {
            // TODO: Lookup. This will copy accessed and dirty flag. This may not be desireable.
            dir->entries[i] = src->entries[i];
        }
        else
        {
            pdirectory_t *src_table = (pdirectory_t *)pdp_entry_pfn(src->entries[i]);
            pdirectory_t *new_table = clone_pdirectory(src_table);

            pdp_entry_set_frame(&dir->entries[i], (phys_addr)new_table);

            dir->entries[i] |= (src->entries[i] & (PDPE_ON_CLONE));
        }
    }

    dir = REMOVE_PAGE_OFFSET(dir);

    return dir;
}

static pml4_t *clone_pml4(pml4_t *src)
{
    pml4_t *dir = virt_mem_alloc_pml4();

    dir = ADD_PAGE_OFFSET(dir);
    src = ADD_PAGE_OFFSET(src);

    for (int i = 0; i < PML4_ENTRIES; ++i)
    {
        if (!src->entries[i])
        {
            continue;
        }

        // TODO: Check if user flag is enough to trigger copy
        if (!pml4_entry_is_user(src->entries[i]))
        {
            dir->entries[i] = src->entries[i];
        }
        else
        {
            pdp_t *src_table = (pdp_t *)pml4_entry_pfn(src->entries[i]);
            pdp_t *new_table = clone_pdp(src_table);

            pml4_entry_set_frame(&dir->entries[i], (phys_addr)new_table);

            dir->entries[i] |= (src->entries[i] & (PML4E_ON_CLONE));
        }
    }

    dir = REMOVE_PAGE_OFFSET(dir);

    return dir;
}

pml4_t *virt_mem_clone_address_space(pml4_t *src)
{
    return clone_pml4(src);
}

//==============================================================================
// End of file
//==============================================================================