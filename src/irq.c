#include "pic.h"
#include <stdint.h>

// Function to read a byte from a port (keyboard I/O)
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

// Function to write a byte to a port
static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

// Keyboard IRQ1 handler in C
void irq1_handler_c(void) {
    volatile uint8_t scancode = inb(0x60); // ACK keyboard controller
    (void)scancode;                        // silence warning

    pic_send_eoi(1);                       // ACK PIC
}
// Timer IRQ0 handler in C (optional)
void irq0_handler_c() {
    // This would run every timer tick
    // TODO: increment tick counter, scheduler, etc.

    pic_send_eoi(0);
}
