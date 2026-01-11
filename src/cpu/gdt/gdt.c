#include "gdt.h"

// GDT entries: null, kernel code, kernel data, user code, user data
uint64_t gdt_entries[5];
struct GDTR gdtr;

// Helper macros for descriptor creation
#define SEG_TYPE_CODE 0b1011  // execute/read, accessed
#define SEG_TYPE_DATA 0b0011  // read/write, accessed
#define SEG_PRESENT   (1 << 15)
#define SEG_LONG_MODE (1 << 21)

// Create descriptor
static uint64_t make_gdt_entry(uint8_t type, uint8_t dpl, int long_mode) {
    uint64_t entry = 0;
    entry |= ((uint64_t)type) << 8;    // type bits
    entry |= (1ULL << 12);             // system=0 (code/data)
    entry |= ((uint64_t)dpl & 0b11) << 13; // DPL
    entry |= (1ULL << 15);             // present
    if (long_mode) entry |= (1ULL << 21); // long mode bit
    return entry << 32;                // upper 32 bits
}

void init_gdt() {
    // Null descriptor
    gdt_entries[0] = 0;

    // Kernel code: selector 0x08, ring 0, long mode
    gdt_entries[1] = make_gdt_entry(SEG_TYPE_CODE, 0, 1);

    // Kernel data: selector 0x10, ring 0
    gdt_entries[2] = make_gdt_entry(SEG_TYPE_DATA, 0, 1);

    // User code: selector 0x18, ring 3, long mode
    gdt_entries[3] = make_gdt_entry(SEG_TYPE_CODE, 3, 1);

    // User data: selector 0x20, ring 3
    gdt_entries[4] = make_gdt_entry(SEG_TYPE_DATA, 3, 1);

    // Set GDTR
    gdtr.limit = sizeof(gdt_entries) - 1;
    gdtr.base  = (uint64_t)&gdt_entries;
}

void flush_gdt() {
    // Load GDTR
    asm volatile("lgdt %0" : : "m"(gdtr));

    // Reload segment registers
    asm volatile(
        "mov $0x10, %%ax\n"   // kernel data selector
        "mov %%ax, %%ds\n"
        "mov %%ax, %%es\n"
        "mov %%ax, %%fs\n"
        "mov %%ax, %%gs\n"
        "mov %%ax, %%ss\n"
        ::: "ax"
    );

    // Reload CS via far return
    asm volatile(
        "pushq $0x08\n"        // kernel code selector
        "lea 1f(%%rip), %%rax\n"
        "pushq %%rax\n"
        "lretq\n"
        "1:\n"
        ::: "rax"
    );
}
