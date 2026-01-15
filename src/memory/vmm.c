#include "memory/vmm.h"
#include "memory/pmm.h"
#include <string.h>

static uint64_t hhdm;

// CR3
static inline uint64_t read_cr3(void) {
    uint64_t val;
    asm volatile ("mov %%cr3, %0" : "=r"(val));
    return val;
}

#define PML4_IDX(x) (((x) >> 39) & 0x1FF)
#define PDP_IDX(x)  (((x) >> 30) & 0x1FF)
#define PD_IDX(x)   (((x) >> 21) & 0x1FF)
#define PT_IDX(x)   (((x) >> 12) & 0x1FF)


void init_vmm(uint64_t hhdm_offset) {
    hhdm = hhdm_offset;
}


void vmm_map_page(uint64_t virt, uint64_t phys, uint64_t flags) {
    uint64_t *pml4 = (uint64_t *)(read_cr3() + hhdm);

    // PML4
    if (!(pml4[PML4_IDX(virt)] & PAGE_PRESENT)) {
        uint64_t phys_pdpt = (uint64_t)pmm_alloc_page();
        memset((void *)(phys_pdpt + hhdm), 0, PAGE_SIZE);
        pml4[PML4_IDX(virt)] = phys_pdpt | PAGE_PRESENT | PAGE_WRITE;
    }

    uint64_t *pdpt = (uint64_t *)((pml4[PML4_IDX(virt)] & ~0xFFF) + hhdm);

    // PDPT
    if (!(pdpt[PDP_IDX(virt)] & PAGE_PRESENT)) {
        uint64_t phys_pd = (uint64_t)pmm_alloc_page();
        memset((void *)(phys_pd + hhdm), 0, PAGE_SIZE);
        pdpt[PDP_IDX(virt)] = phys_pd | PAGE_PRESENT | PAGE_WRITE;
    }

    uint64_t *pd = (uint64_t *)((pdpt[PDP_IDX(virt)] & ~0xFFF) + hhdm);

    //* PD
    if (!(pd[PD_IDX(virt)] & PAGE_PRESENT)) {
        uint64_t phys_pt = (uint64_t)pmm_alloc_page();
        memset((void *)(phys_pt + hhdm), 0, PAGE_SIZE);
        pd[PD_IDX(virt)] = phys_pt | PAGE_PRESENT | PAGE_WRITE;
    }

    uint64_t *pt = (uint64_t *)((pd[PD_IDX(virt)] & ~0xFFF) + hhdm);

    // PT
    pt[PT_IDX(virt)] = phys | flags | PAGE_PRESENT;

    asm volatile ("invlpg (%0)" :: "r"(virt) : "memory");
}


void vmm_unmap_page(uint64_t virt) {
    uint64_t *pml4 = (uint64_t *)(read_cr3() + hhdm);

    if (!(pml4[PML4_IDX(virt)] & PAGE_PRESENT)) return;
    uint64_t *pdpt = (uint64_t *)((pml4[PML4_IDX(virt)] & ~0xFFF) + hhdm);

    if (!(pdpt[PDP_IDX(virt)] & PAGE_PRESENT)) return;
    uint64_t *pd = (uint64_t *)((pdpt[PDP_IDX(virt)] & ~0xFFF) + hhdm);

    if (!(pd[PD_IDX(virt)] & PAGE_PRESENT)) return;
    uint64_t *pt = (uint64_t *)((pd[PD_IDX(virt)] & ~0xFFF) + hhdm);

    pt[PT_IDX(virt)] = 0;
    asm volatile ("invlpg (%0)" :: "r"(virt) : "memory");
}


uint64_t vmm_alloc_page(uint64_t virt, uint64_t flags) {
    uint64_t phys = (uint64_t)pmm_alloc_page();
    if (!phys) return 0;

    vmm_map_page(virt, phys, flags);
    return phys;
}
