#include "gdt.h"
#include <stdint.h>

extern void gdt_flush(uintptr_t addr);

struct gdt_entry_struct gdt_entries[5];
struct gdt_ptr_struct gdt_ptr;


struct gdt_tss_entry_struct gdt_tss_entries[1];
struct gdt_tss_ptr_struct gdt_tss_ptr;

void init_gdt(){
    gdt_ptr.limit = (sizeof(struct gdt_entry_struct) * 5) - 1;
    gdt_ptr.base = (uintptr_t)gdt_entries;

    set_gdt_gate(0, 0, 0, 0, 0); //Null segment

    //Kernel code segment
    set_gdt_gate(1, 0, 0xFFFFF, GDT_ACCESS_PRESENT_BIT |
                                GDT_ACCESS_DESCRIPTOR_TYPE_CODE_DATA_SEG |
                                GDT_ACCESS_EXECUTABLE_BIT  |
                                GDT_ACCESS_DESCRIPTOR_PRIVILEGE_HIGH |
                                GDT_ACCESS_RW_BIT,
                                GDT_FLAG_GRANULARITY | GDT_FLAG_LONG);

    //Kernel data segment
    set_gdt_gate(2, 0, 0xFFFFF, GDT_ACCESS_PRESENT_BIT |
                                GDT_ACCESS_DESCRIPTOR_TYPE_CODE_DATA_SEG |
                                GDT_ACCESS_DESCRIPTOR_PRIVILEGE_HIGH |
                                GDT_ACCESS_RW_BIT,
                                GDT_FLAG_GRANULARITY);

    //User code segment
    set_gdt_gate(3, 0, 0xFFFFF, GDT_ACCESS_PRESENT_BIT |
                                GDT_ACCESS_DESCRIPTOR_PRIVILEGE_LOW |
                                GDT_ACCESS_DESCRIPTOR_TYPE_CODE_DATA_SEG |
                                GDT_ACCESS_EXECUTABLE_BIT |
                                GDT_ACCESS_RW_BIT,
                                GDT_FLAG_GRANULARITY | GDT_FLAG_LONG);
            
    //User data segment
    set_gdt_gate(4, 0, 0xFFFFF, GDT_ACCESS_PRESENT_BIT |
                                GDT_ACCESS_DESCRIPTOR_PRIVILEGE_LOW |
                                GDT_ACCESS_DESCRIPTOR_TYPE_CODE_DATA_SEG |
                                GDT_ACCESS_RW_BIT,
                                GDT_FLAG_GRANULARITY);

    // Note
    // We are in long mode so base and limit is ignored

    gdt_flush((uintptr_t)&gdt_ptr);
}


void set_gdt_gate(uint32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags){
    gdt_entries[num].base_low    = (base & 0xFFFF);
    gdt_entries[num].base_middle = (base >> 16) & 0xFF;
    gdt_entries[num].base_high   = (base >> 24) & 0xFF;

    // lower half of flags is actualy limit
    gdt_entries[num].limit  = (limit & 0xFFFF);
    gdt_entries[num].flags  = (limit >> 16) & 0x0F;

    gdt_entries[num].flags |= (flags & 0x0F);

    gdt_entries[num].access = access;
}



void set_gdt_tss_gate(uint32_t num, uint64_t base, uint32_t limit, uint8_t access, uint8_t flags){
    gdt_tss_entries[num].base_low    = (base & 0xFFFF);
    gdt_tss_entries[num].base_middle = (base >> 16) & 0xFF;
    gdt_tss_entries[num].base_high   = (base >> 24) & 0xFF;
    gdt_tss_entries[num].base_higher = (base >> 32) & 0xFFFFFFFF;

    // lower half of flags is actualy limit
    gdt_tss_entries[num].limit  = (limit & 0xFFFF);
    gdt_tss_entries[num].flags  = (limit >> 16) & 0x0F;

    gdt_tss_entries[num].flags |= (flags & 0x0F);

    gdt_tss_entries[num].access = access;
}