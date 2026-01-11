#ifndef PIC_H
#define PIC_H

#include <stdint.h>

// PIC ports
#define PIC1_COMMAND 0x20
#define PIC1_DATA    0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA    0xA1

// PIC initialization control words
#define ICW1_ICW4    0x01    // ICW4 (not) needed
#define ICW1_INIT    0x10    // Initialization - required!
#define ICW4_8086    0x01    // 8086/88 (MCS-80/85) mode

#define PIC_EOI      0x20    // End-of-interrupt command code

// Remap PIC vectors
void pic_remap(int offset1, int offset2);

// Mask and unmask IRQ lines
void pic_set_mask(uint8_t mask);
void pic_clear_mask(uint8_t irq);

// Send End-of-Interrupt signal
void pic_send_eoi(uint8_t irq);

#endif
