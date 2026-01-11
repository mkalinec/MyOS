// pmm_liballoc.c
#include <stdint.h>
#include <stddef.h>
#include "include/limine.h"

#define PAGE_SIZE 4096
#define MAX_PHYS_PAGES 1048576  // 4GB / 4KB

// -------------------------
// Limine memmap
// -------------------------
extern struct limine_memmap_request memmap_request;

// -------------------------
// PMM bitmap
// -------------------------
static uint64_t bitmap[MAX_PHYS_PAGES / 64];
static size_t phys_mem_pages = 0;
static uintptr_t phys_mem_base = 0;

// -------------------------
// Lock (disable interrupts)
// -------------------------
static uint64_t irq_flags;

static inline uint64_t irq_save(void) {
    uint64_t flags;
    asm volatile(
        "pushfq\n"
        "pop %0\n"
        "cli"
        : "=r"(flags)
        :
        : "memory"
    );
    return flags;
}

static inline void irq_restore(uint64_t flags) {
    asm volatile(
        "push %0\n"
        "popfq"
        :
        : "r"(flags)
        : "memory"
    );
}

// -------------------------
// liballoc lock/unlock
// -------------------------
int liballoc_lock() {
    irq_flags = irq_save();
    return 0;
}

int liballoc_unlock() {
    irq_restore(irq_flags);
    return 0;
}

// -------------------------
// Bitmap helpers
// -------------------------
static inline void set_bit(size_t idx)   { bitmap[idx / 64] |=  (1ULL << (idx % 64)); }
static inline void clear_bit(size_t idx) { bitmap[idx / 64] &= ~(1ULL << (idx % 64)); }
static inline int test_bit(size_t idx)   { return (bitmap[idx / 64] >> (idx % 64)) & 1; }

// -------------------------
// PMM init
// -------------------------
void pmm_init() {
    // predpokladáme, že kernel končí na 1MB
    phys_mem_base = 0x100000; 
    phys_mem_pages = 0;

    for (size_t i = 0; i < memmap_request.response->entry_count; i++) {
        struct limine_memmap_entry* e = memmap_request.response->entries[i];
        if (e->type != LIMINE_MEMMAP_USABLE) continue;

        uintptr_t start = e->base;
        uintptr_t end   = e->base + e->length;

        if (start < phys_mem_base) start = phys_mem_base;

        size_t pages = (end - start) / PAGE_SIZE;
        for (size_t j = 0; j < pages; j++) {
            clear_bit((start / PAGE_SIZE) + j);
        }

        phys_mem_pages += pages;
    }
}

// -------------------------
// Allocate pages
// -------------------------
void* pmm_alloc_pages(int pages) {
    if (pages <= 0) return NULL;

    size_t consec = 0;
    size_t start_idx = 0;

    for (size_t i = 0; i < phys_mem_pages; i++) {
        if (!test_bit(i)) {
            if (consec == 0) start_idx = i;
            consec++;
            if (consec == (size_t)pages) {
                for (size_t j = 0; j < (size_t)pages; j++)
                    set_bit(start_idx + j);
                return (void*)((start_idx) * PAGE_SIZE);
            }
        } else {
            consec = 0;
        }
    }

    return NULL; // nie je dosť voľných stránok
}

// -------------------------
// Free pages
// -------------------------
int pmm_free_pages(void* addr, int pages) {
    if (!addr || pages <= 0) return -1;

    size_t start_idx = ((uintptr_t)addr) / PAGE_SIZE;

    for (size_t i = 0; i < (size_t)pages; i++) {
        clear_bit(start_idx + i);
    }

    return 0;
}

// -------------------------
// liballoc hooks
// -------------------------
void* liballoc_alloc(int pages) {
    return pmm_alloc_pages(pages);
}

int liballoc_free(void* ptr, int pages) {
    return pmm_free_pages(ptr, pages);
}
