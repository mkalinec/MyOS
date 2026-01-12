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

struct gdt_ptr_struct{
    uint16_t limit;
    uintptr_t base;
}__attribute__((packed));

struct tss_struct {
    uint32_t reserved0;
    uint64_t rsp0;
    uint64_t rsp1;
    uint64_t rsp2;
    uint64_t reserved1;
    uint64_t ist1;
    uint64_t ist2;
    uint64_t ist3;
    uint64_t ist4;
    uint64_t ist5;
    uint64_t ist6;
    uint64_t ist7;
    uint64_t reserved2;
    uint16_t reserved3;
    uint16_t iomap_base;
} __attribute__((packed));

void init_gdt();
void set_gdt_gate(uint32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags);
void set_gdt_tss_gate(uint32_t num, uint64_t base, uint32_t limit);