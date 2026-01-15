#pragma once

#include <stdint.h>

#define PAGE_SIZE 4096

void init_pmm(void);

uint64_t *pmm_alloc_page(void);
uint64_t *pmm_alloc_pages(uint64_t n);
void pmm_free_page(void *phys);
void pmm_free_pages(void *phys, uint64_t n);