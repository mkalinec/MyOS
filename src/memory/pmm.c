#include "memory/pmm.h"

#include "limine.h"
//#include "limine_attribute.h"
#include "global_variables.h"
#include <string.h>
#include "memory/pmm.h"
#include <stdint.h>


__attribute__((used, section(".limine_requests")))
static volatile struct limine_memmap_request memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST_ID,
    .revision = 0
};

uint64_t total_pages;
uint8_t *pmm_bitmap;
uint64_t bitmap_size;

uint64_t bitmap_phys;
uint64_t bitmap_pages;
uint64_t bitmap_start_page;
uint64_t bitmap_end_page;

struct limine_memmap_response *memmap;
static uint64_t hhdm;

void pmm_mark_free(uint64_t base, uint64_t length) {
    uint64_t start_page = base / PAGE_SIZE;
    uint64_t page_count = length / PAGE_SIZE;

    for (uint64_t i = 0; i < page_count; i++) {
        uint64_t page = start_page + i;

        if (page >= total_pages) continue;

        // do not delete bitmap
        if (page >= bitmap_start_page && page < bitmap_end_page)
            continue;

        pmm_bitmap[page / 8] &= ~(1 << (page % 8)); // mark it as free
    }
}



void init_pmm(uint64_t hhdm_offset){
    memmap = memmap_request.response;
    hhdm = hhdm_offset;

    //calculate memory size
    uint64_t max_phys = 0;

    for (uint64_t i = 0; i < memmap->entry_count; i++) {
        struct limine_memmap_entry *e = memmap->entries[i];
        uint64_t end = e->base + e->length;
        if (end > max_phys)
            max_phys = end;
    }

    total_pages = max_phys / PAGE_SIZE;
    bitmap_size = (total_pages + 7) / 8;

    //this is where bitmap will be stored
    for (uint64_t i = 0; i < memmap->entry_count; i++) {
        struct limine_memmap_entry *e = memmap->entries[i];
        if (e->type == LIMINE_MEMMAP_USABLE && e->length >= bitmap_size) {
            // bitmap will live there
            bitmap_phys = e->base;
            pmm_bitmap  = (uint8_t *)(bitmap_phys + hhdm);
            break;
        }
    }



    bitmap_pages      = (bitmap_size + PAGE_SIZE - 1) / PAGE_SIZE;
    bitmap_start_page = bitmap_phys / PAGE_SIZE;
    bitmap_end_page   = bitmap_start_page + bitmap_pages;

    // mark everything as used
    memset(pmm_bitmap, 0xFF, bitmap_size);

    

    // mark safe to use memory as free
    for (uint64_t i = 0; i < memmap->entry_count; i++) {
        struct limine_memmap_entry *e = memmap->entries[i];
        if (e->type == LIMINE_MEMMAP_USABLE) {
            pmm_mark_free(e->base, e->length);
        }
    }

    // mark pages where bitmap is stored as used
    for (uint64_t i = 0; i < bitmap_pages; i++) {
        uint64_t page = bitmap_start_page + i;
        pmm_bitmap[page / 8] |= (1 << (page % 8)); //toto mi crashuje
    }

}



uint64_t* pmm_alloc_page(void) {
    for (uint64_t i = 0; i < total_pages; i++) {
        if (!(pmm_bitmap[i / 8] & (1 << (i % 8)))) { // is this page free?
            pmm_bitmap[i / 8] |= (1 << (i % 8));  // mark it as used
            return (uint64_t*)(i * PAGE_SIZE);
        }
    }
    return 0; // out of memory
}



uint64_t* pmm_alloc_pages(uint64_t n) {
    if (n == 0) return 0;

    for (uint64_t i = 0; i <= total_pages - n; i++) {
        if (!(pmm_bitmap[i / 8] & (1 << (i % 8)))) { // is this page free?
            uint64_t j = 0;
            for (; j < n; j++){
                if (i + j >= total_pages) break;
                if ((pmm_bitmap[(i+j) / 8] & (1 << ((i+j) % 8)))) // check if following pages are free
                    break;
            }
            if (j == n){
                for (uint64_t j = 0; j < n; j++)
                    pmm_bitmap[(i+j) / 8] |= (1 << ((i+j) % 8));  // mark it as used

                return (uint64_t*)(i * PAGE_SIZE);
            }
        }
    }
    return 0; // out of memory
}

void pmm_free_page(void *phys){
    pmm_mark_free((uint64_t)phys, PAGE_SIZE);
}

void pmm_free_pages(void *phys, uint64_t n){
    pmm_mark_free((uint64_t)phys, n * PAGE_SIZE);
}