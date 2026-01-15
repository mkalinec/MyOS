#include "memory/malloc.h"
#include "memory/heap.h"
#include <stdint.h>

typedef struct heap_block {
    size_t size;
    struct heap_block *next;
    int free;
} heap_block_t;

static heap_block_t *free_list = NULL;

void malloc_init(void) {
    free_list = NULL;
}

static heap_block_t *find_block(size_t size) {
    heap_block_t *curr = free_list;
    while (curr) {
        if (curr->free && curr->size >= size) {
            return curr;
        }
        curr = curr->next;
    }
    return NULL;
}

static void split_block(heap_block_t *block, size_t size) {
    if (block->size >= size + sizeof(heap_block_t) + 8) {
        heap_block_t *new_block = (heap_block_t *)((uint8_t *)block + sizeof(heap_block_t) + size);
        new_block->size = block->size - sizeof(heap_block_t) - size;
        new_block->free = 1;
        new_block->next = block->next;
        block->size = size;
        block->next = new_block;
    }
}

void *malloc(size_t size) {
    if (size == 0) return NULL;

    heap_block_t *block = find_block(size);
    if (block) {
        block->free = 0;
        split_block(block, size);
        return (void *)((uint8_t *)block + sizeof(heap_block_t));
    }

    // new block from heap
    heap_block_t *new_block = (heap_block_t *)heap_alloc(sizeof(heap_block_t) + size);
    new_block->size = size;
    new_block->free = 0;
    new_block->next = NULL;


    if (!free_list) {
        free_list = new_block;
    } else {
        heap_block_t *last = free_list;
        while (last->next) last = last->next;
        last->next = new_block;
    }

    return (void *)((uint8_t *)new_block + sizeof(heap_block_t));
}

void free(void *ptr) {
    if (!ptr) return;

    heap_block_t *block = (heap_block_t *)((uint8_t *)ptr - sizeof(heap_block_t));
    block->free = 1;

    // merge with neighbords blocks
    heap_block_t *curr = free_list;
    while (curr) {
        if (curr->free && curr->next && curr->next->free) {
            curr->size += sizeof(heap_block_t) + curr->next->size;
            curr->next = curr->next->next;
        } else {
            curr = curr->next;
        }
    }
}
