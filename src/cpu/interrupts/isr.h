#ifndef ISR_H
#define ISR_H

#include <stdint.h>
#include <stdbool.h>

#define IDT_MAX_DESCRIPTORS 256

static bool vectors[IDT_MAX_DESCRIPTORS];

extern void* isr_stub_table[];

void idt_init(void);

#endif