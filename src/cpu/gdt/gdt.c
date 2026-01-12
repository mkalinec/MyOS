#include "gdt.h"
#include <stdint.h>
#include <string.h>

extern void gdt_flush(uintptr_t addr);

// 5 + 1 tss
uint8_t gdt[7 * 8];
struct gdt_ptr_struct gdt_ptr;
struct tss_struct kernel_tss;

void init_gdt(){
    memset(gdt, 0, sizeof(gdt));
    memset(&kernel_tss, 0, sizeof(kernel_tss));

    gdt_ptr.limit = sizeof(gdt) - 1;
    gdt_ptr.base  = (uintptr_t)&gdt;

    // Null segment
    set_gdt_gate(0, 0, 0, 0, 0);

    // Kernel code segment
    set_gdt_gate(1, 0, 0xFFFFF, GDT_ACCESS_PRESENT_BIT |
                                GDT_ACCESS_DESCRIPTOR_TYPE_CODE_DATA_SEG |
                                GDT_ACCESS_EXECUTABLE_BIT |
                                GDT_ACCESS_DESCRIPTOR_PRIVILEGE_HIGH |
                                GDT_ACCESS_RW_BIT,
                                GDT_FLAG_GRANULARITY | GDT_FLAG_LONG);

    // Kernel data segment
    set_gdt_gate(2, 0, 0xFFFFF, GDT_ACCESS_PRESENT_BIT |
                                GDT_ACCESS_DESCRIPTOR_TYPE_CODE_DATA_SEG |
                                GDT_ACCESS_DESCRIPTOR_PRIVILEGE_HIGH |
                                GDT_ACCESS_RW_BIT,
                                GDT_FLAG_GRANULARITY);

    // User code segment
    set_gdt_gate(3, 0, 0xFFFFF, GDT_ACCESS_PRESENT_BIT |
                                GDT_ACCESS_DESCRIPTOR_PRIVILEGE_LOW |
                                GDT_ACCESS_DESCRIPTOR_TYPE_CODE_DATA_SEG |
                                GDT_ACCESS_EXECUTABLE_BIT |
                                GDT_ACCESS_RW_BIT,
                                GDT_FLAG_GRANULARITY | GDT_FLAG_LONG);

    // User data segment
    set_gdt_gate(4, 0, 0xFFFFF, GDT_ACCESS_PRESENT_BIT |
                                GDT_ACCESS_DESCRIPTOR_PRIVILEGE_LOW |
                                GDT_ACCESS_DESCRIPTOR_TYPE_CODE_DATA_SEG |
                                GDT_ACCESS_RW_BIT,
                                GDT_FLAG_GRANULARITY);

    // Note
    // We are in long mode so base and limit is ignored

    kernel_tss.iomap_base = sizeof(kernel_tss);
    set_gdt_tss_gate(5, (uint64_t)&kernel_tss, sizeof(kernel_tss) - 1);

    gdt_flush((uintptr_t)&gdt_ptr);

    asm volatile ("ltr %0" : : "r"((uint16_t)(5 << 3)));
}

void set_gdt_gate(uint32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags){
    uint8_t *entry = &gdt[num * 8];

    entry[0] =  limit        & 0xFF;
    entry[1] = (limit >> 8)  & 0xFF;
    entry[2] =  base         & 0xFF;
    entry[3] = (base >> 8)   & 0xFF;
    entry[4] = (base >> 16)  & 0xFF;
    entry[5] =  access;
    entry[6] = ((limit >> 16) & 0x0F) | ((flags & 0x0F) << 4);
    entry[7] = (base >> 24)  & 0xFF;
}

void set_gdt_tss_gate(uint32_t num, uint64_t base, uint32_t limit){
    uint8_t *entry = &gdt[num * 8];

    entry[0] =  limit        & 0xFF;
    entry[1] = (limit >> 8)  & 0xFF;
    entry[2] =  base         & 0xFF;
    entry[3] = (base >> 8)   & 0xFF;
    entry[4] = (base >> 16)  & 0xFF;
    entry[5] =  0x89; // Present | system | available 64-bit TSS
    entry[6] = (limit >> 16) & 0x0F;
    entry[7] = (base >> 24)  & 0xFF;

    entry[8]  = (base >> 32) & 0xFF;
    entry[9]  = (base >> 40) & 0xFF;
    entry[10] = (base >> 48) & 0xFF;
    entry[11] = (base >> 56) & 0xFF;
    entry[12] = 0;
    entry[13] = 0;
    entry[14] = 0;
    entry[15] = 0;
}
