#ifndef GDT_H
#define GDT_H

#include <stdint.h>

// GDTR struct
struct __attribute__((packed)) GDTR {
    uint16_t limit;
    uint64_t base;
};

// Extern GDT
extern uint64_t gdt_entries[5];
extern struct GDTR gdtr;

// Functions
void init_gdt();
void flush_gdt();

#endif
