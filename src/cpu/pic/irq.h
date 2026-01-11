#ifndef IRQ_H
#define IRQ_H

#include <stdint.h>

// C handlers called by assembly stubs
void irq0_handler_c(); // Timer
void irq1_handler_c(); // Keyboard


extern void irq1_handler();

#endif
