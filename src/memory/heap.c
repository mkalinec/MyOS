#include "memory/heap.h"
#include "memory/vmm.h"
#include "memory/pmm.h"
#include <stdint.h>

#define PAGE_SIZE 4096
#define KERNEL_HEAP_START 0xFFFF900000100000
#define KERNEL_HEAP_MAX   0xFFFF900010000000 // 16 MiB heap

static uint64_t heap_top = KERNEL_HEAP_START;

void heap_grow(void) {
    if (heap_top >= KERNEL_HEAP_MAX) {
        for (;;) {} // panic: out of heap
    }

    uint64_t *phys_page = pmm_alloc_page();
    if (!phys_page) {
        for (;;) {} // panic: out of memory
    }

    vmm_map_page(heap_top, (uint64_t)phys_page, PAGE_WRITE);
    heap_top += PAGE_SIZE;
}

void heap_init(void) {
    heap_top = KERNEL_HEAP_START;
}

void *heap_alloc(size_t size) {
    uint64_t ptr = heap_top;
    size_t pages_needed = (size + PAGE_SIZE - 1) / PAGE_SIZE;

    for (size_t i = 0; i < pages_needed; i++) heap_grow();

    return (void *)ptr;
}
