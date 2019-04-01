#ifndef _VIRT_MEM_H
#define _VIRT_MEM_H

#include <stdint.h>
#include <mm/phys_mem.h>

//==============================================================================
// Page Table Entry
//==============================================================================

enum PTE_FLAGS
{
    PTE_PRESENT = 1,
    PTE_WRITABLE = 2,
    PTE_USER = 4,
    PTE_WRITETHROUGH = 8,
    PTE_NOT_CAHCEABLE = 0x10,
    PTE_ACCESS = 0x20,
    PTE_DIRTY = 0x40,
    PTE_PAT = 0x80, // Page attribute table
    PTE_FRAME = 0x7FFFFFFFFFFFF000
};

typedef uint64_t pt_entry_t;

void pt_entry_add_attrib(pt_entry_t *e, uint64_t attrib);
void pt_entry_del_attrib(pt_entry_t *e, uint64_t attrib);
void pt_entry_set_frame(pt_entry_t *e, phys_addr addr);

int pt_entry_is_present(pt_entry_t e);
int pt_entry_is_writable(pt_entry_t e);
int pt_entry_is_user(pt_entry_t e);
int pt_entry_is_writethrough(pt_entry_t e);
int pt_entry_is_cacheable(pt_entry_t e);
int pt_entry_is_accessed(pt_entry_t e);
int pt_entry_is_dirty(pt_entry_t e);
int pt_entry_is_PAT(pt_entry_t e);
phys_addr pt_entry_pfn(pt_entry_t e);

//==============================================================================
// Page Directory Entry
//==============================================================================

enum PDE_FLAGS
{
    PDE_PRESENT = 1,
    PDE_WRITABLE = 2,
    PDE_USER = 4,
    PDE_PWT = 8,
    PDE_PCD = 0x10,
    PDE_ACCESSED = 0x20,
    PDE_DIRTY = 0x40,
    PDE_4MB = 0x80,
    PDE_CPU_GLOBAL = 0x100,
    PDE_LV4_GLOBAL = 0x200,
    PDE_FRAME = 0x7FFFFFFFFFFFF000
};

typedef uint64_t pd_entry_t;

void pd_entry_add_attrib(pd_entry_t *e, uint64_t attrib);
void pd_entry_del_attrib(pd_entry_t *e, uint64_t attrib);
void pd_entry_set_frame(pd_entry_t *e, phys_addr addr);

int pd_entry_is_present(pd_entry_t e);
int pd_entry_is_writable(pd_entry_t e);
int pd_entry_is_user(pd_entry_t e);
int pd_entry_is_pwt(pd_entry_t e);
int pd_entry_is_pcd(pd_entry_t e);
int pd_entry_is_accessed(pd_entry_t e);
phys_addr pd_entry_pfn(pd_entry_t e);

//==============================================================================
// Page Directory Pointer Entry
//==============================================================================

enum PDPE_FLAGS
{
    PDPE_PRESENT = 1,
    PDPE_WRITABLE = 2,
    PDPE_USER = 4,
    PDPE_PWT = 8,
    PDPE_PCD = 0x10,
    PDPE_ACCESSED = 0x20,
    PDPE_FRAME = 0x7FFFFFFFFFFFF000
};

typedef uint64_t pdp_entry_t;

void pdp_entry_add_attrib(pdp_entry_t *e, uint64_t attrib);
void pdp_entry_del_attrib(pdp_entry_t *e, uint64_t attrib);
void pdp_entry_set_frame(pdp_entry_t *e, phys_addr addr);

int pdp_entry_is_present(pdp_entry_t e);
int pdp_entry_is_writable(pdp_entry_t e);
int pdp_entry_is_user(pdp_entry_t e);
int pdp_entry_is_pwt(pdp_entry_t e);
int pdp_entry_is_pcd(pdp_entry_t e);
int pdp_entry_is_accessed(pdp_entry_t e);
phys_addr pdp_entry_pfn(pdp_entry_t e);

//==============================================================================
// Page Directory Pointer Entry
//==============================================================================

enum PML4E_FLAGS
{
    PML4E_PRESENT = 1,
    PML4E_WRITABLE = 2,
    PML4E_USER = 4,
    PML4E_PWT = 8,
    PML4E_PCD = 0x10,
    PML4E_ACCESSED = 0x20,
    PML4E_FRAME = 0x7FFFFFFFFFFFF000
};

typedef uint64_t pml4_entry_t;

void pml4_entry_add_attrib(pml4_entry_t *e, uint64_t attrib);
void pml4_entry_del_attrib(pml4_entry_t *e, uint64_t attrib);
void pml4_entry_set_frame(pml4_entry_t *e, phys_addr addr);

int pml4_entry_is_present(pml4_entry_t e);
int pml4_entry_is_writable(pml4_entry_t e);
int pml4_entry_is_user(pml4_entry_t e);
int pml4_entry_is_pwt(pml4_entry_t e);
int pml4_entry_is_pcd(pml4_entry_t e);
int pml4_entry_is_accessed(pml4_entry_t e);
phys_addr pml4_entry_pfn(pml4_entry_t e);

//==============================================================================
// Virtual Memory Manager
//==============================================================================

typedef uint64_t virt_addr;

#define PT_ENTRIES 512
#define PD_ENTRIES 512
#define PDP_ENTRIES 512
#define PML4_ENTRIES 512

#define PT_INDEX(x) (((x) >> 12) & 0x1FF)
#define PD_INDEX(x) (((x) >> 21) & 0x1FF)
#define PDP_INDEX(x) (((x) >> 30) & 0x1FF)
#define PML4_INDEX(x) (((x) >> 39) & 0x1FF)

typedef struct
{
    pt_entry_t entries[PT_ENTRIES];
} ptable_t;

typedef struct
{
    pd_entry_t entries[PD_ENTRIES];
} pdirectory_t;

typedef struct
{
    pdp_entry_t entries[PDP_ENTRIES];
} pdp_t;

typedef struct
{
    pml4_entry_t entries[PML4_ENTRIES];
} pml4_t;

void virt_mem_initialize();

int virt_mem_switch_dir(pml4_t *dir);
pml4_t *virt_mem_get_current_dir();
void virt_mem_flush_tlb(virt_addr addr);

void *virt_mem_get_physical_addr(void *addr, pml4_t *dir);

pml4_t *virt_mem_create_address_space();

void virt_mem_clear_pt(ptable_t *table);
void virt_mem_clear_pd(pdirectory_t *pdir);
void virt_mem_clear_pdp(pdp_t *pdp);
void virt_mem_destroy_address_space(pml4_t *dir);

ptable_t *virt_mem_alloc_ptable();
pdirectory_t *virt_mem_alloc_pdirectory();
pdp_t *virt_mem_alloc_pdp();

// TODO: Add flags for user-mode, writable, cache, remap, shared
int virt_mem_map_page_p(void *phys, void *virt, uint64_t flags, pml4_t *dir);
int virt_mem_map_page(void *phys, void *virt, uint64_t flags);
int virt_mem_map_pages_p(void *phys, void *virt, size_t n_pages, uint64_t flags, pml4_t *dir);
int virt_mem_map_pages(void *phys, void *virt, size_t n_pages, uint64_t flags);
int virt_mem_unmap_page(void *virt);
int virt_mem_unmap_pages(void *virt, size_t n_pages);

#endif