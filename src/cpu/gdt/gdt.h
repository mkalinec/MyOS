#pragma once

#include <stdint.h>

#define GDT_FLAG_GRANULARITY 0b1000
#define GDT_FLAG_DB          0b0100
#define GDT_FLAG_LONG        0b0010

#define GDT_ACCESS_PRESENT_BIT                      0b10000000
#define GDT_ACCESS_DESCRIPTOR_PRIVILEGE_HIGH        0b00000000
#define GDT_ACCESS_DESCRIPTOR_PRIVILEGE_MED         0b01000000
#define GDT_ACCESS_DESCRIPTOR_PRIVILEGE_LOW         0b01100000
#define GDT_ACCESS_DESCRIPTOR_TYPE_SYS_SEG          0b00000000
#define GDT_ACCESS_DESCRIPTOR_TYPE_CODE_DATA_SEG    0b00010000
#define GDT_ACCESS_EXECUTABLE_BIT                   0b00001000
#define GDT_ACCESS_DC_BIT                           0b00000100
#define GDT_ACCESS_RW_BIT                           0b00000010
#define GDT_ACCESS_ACCESS_BIT                       0b00000001


struct gdt_entry_struct {
    uint16_t limit;
    uint16_t base_low;
    uint8_t  base_middle;
    uint8_t  access;
    uint8_t  flags; //this is actualy limit + flags
    uint8_t  base_high;
} __attribute__((packed));

struct gdt_ptr_struct{
    uint16_t limit;
    uintptr_t base;
}__attribute__((packed));


struct gdt_tss_entry_struct{
    uint16_t limit;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t access;
    uint8_t flags; //this is actualy limit + flags
    uint8_t base_high;
    uint32_t base_higher;
    uint32_t reserved;

}__attribute__((packed));

struct gdt_tss_ptr_struct{
    uint16_t limit;
    uintptr_t base;
}__attribute__((packed));

void init_gdt();
void set_gdt_gate(uint32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran);
void set_gdt_tss_gate(uint32_t num, uint64_t base, uint32_t limit, uint8_t access, uint8_t gran);