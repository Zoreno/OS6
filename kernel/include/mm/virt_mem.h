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
    PTE_CPU_GLOBAL = 0x100,
    PTE_LV4_GLOBAL = 0x200,
    PTE_FRAME = 0x7FFFFFFFFFFFF000
};

typedef uint64_t pt_entry;

void pt_entry_add_attrib(pt_entry *e, uint64_t attrib);
void pt_entry_del_attrib(pt_entry *e, uint64_t attrib);
void pt_entry_set_frame(pt_entry *e, phys_addr addr);

int pt_entry_is_present(pt_entry e);
int pt_entry_is_writable(pt_entry e);
int pt_entry_is_user(pt_entry e);
int pt_entry_is_writethrough(pt_entry e);
int pt_entry_is_cacheable(pt_entry e);
int pt_entry_is_accessed(pt_entry e);
int pt_entry_is_dirty(pt_entry e);
int pt_entry_is_PAT(pt_entry e);
int pt_entry_is_CPU_global(pt_entry e);
int pt_entry_is_LV4_global(pt_entry e);
phys_addr pt_entry_pfn(pt_entry e);

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

typedef uint64_t pd_entry;

void pd_entry_add_attrib(pd_entry *e, uint64_t attrib);
void pd_entry_del_attrib(pd_entry *e, uint64_t attrib);
void pd_entry_set_frame(pd_entry *e, phys_addr addr);

int pd_entry_is_present(pd_entry e);
int pd_entry_is_writable(pd_entry e);
int pd_entry_is_user(pd_entry e);
int pd_entry_is_pwt(pd_entry e);
int pd_entry_is_pcd(pd_entry e);
int pd_entry_is_accessed(pd_entry e);
int pd_entry_is_dirty(pd_entry e);
int pd_entry_is_4MB(pd_entry e);
int pd_entry_is_CPU_global(pd_entry e);
int pd_entry_is_LV4_global(pd_entry e);
phys_addr pd_entry_pfn(pd_entry e);

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
    PDPE_DIRTY = 0x40,
    PDPE_4MB = 0x80,
    PDPE_CPU_GLOBAL = 0x100,
    PDPE_LV4_GLOBAL = 0x200,
    PDPE_FRAME = 0x7FFFFFFFFFFFF000
};

typedef uint64_t pdp_entry;

void pdp_entry_add_attrib(pdp_entry *e, uint64_t attrib);
void pdp_entry_del_attrib(pdp_entry *e, uint64_t attrib);
void pdp_entry_set_frame(pdp_entry *e, phys_addr addr);

int pdp_entry_is_present(pdp_entry e);
int pdp_entry_is_writable(pdp_entry e);
int pdp_entry_is_user(pdp_entry e);
int pdp_entry_is_pwt(pdp_entry e);
int pdp_entry_is_pcd(pdp_entry e);
int pdp_entry_is_accessed(pdp_entry e);
int pdp_entry_is_dirty(pdp_entry e);
int pdp_entry_is_4MB(pdp_entry e);
int pdp_entry_is_CPU_global(pdp_entry e);
int pdp_entry_is_LV4_global(pdp_entry e);
phys_addr pdp_entry_pfn(pdp_entry e);

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
    PML4E_DIRTY = 0x40,
    PML4E_4MB = 0x80,
    PML4E_CPU_GLOBAL = 0x100,
    PML4E_LV4_GLOBAL = 0x200,
    PML4E_FRAME = 0x7FFFFFFFFFFFF000
};

typedef uint64_t pml4_entry;

void pml4_entry_add_attrib(pml4_entry *e, uint64_t attrib);
void pml4_entry_del_attrib(pml4_entry *e, uint64_t attrib);
void pml4_entry_set_frame(pml4_entry *e, phys_addr addr);

int pml4_entry_is_present(pml4_entry e);
int pml4_entry_is_writable(pml4_entry e);
int pml4_entry_is_user(pml4_entry e);
int pml4_entry_is_pwt(pml4_entry e);
int pml4_entry_is_pcd(pml4_entry e);
int pml4_entry_is_accessed(pml4_entry e);
int pml4_entry_is_dirty(pml4_entry e);
int pml4_entry_is_4MB(pml4_entry e);
int pml4_entry_is_CPU_global(pml4_entry e);
int pml4_entry_is_LV4_global(pml4_entry e);
phys_addr pml4_entry_pfn(pml4_entry e);

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
    pt_entry entries[PT_ENTRIES];
} ptable_t;

typedef struct
{
    pd_entry entries[PD_ENTRIES];
} pdirectory_t;

typedef struct
{
    pdp_entry entries[PDP_ENTRIES];
} pdp_t;

typedef struct
{
    pml4_entry entries[PML4_ENTRIES];
} pml4_t;

void virt_mem_map_page(void *phys, void *virt);

void virt_mem_initialize();

int virt_mem_switch_dir(pml4_t *dir);
pml4_t *virt_mem_get_current_dir();

#endif