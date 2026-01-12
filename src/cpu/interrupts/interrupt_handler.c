#include <stdint.h>
#include "../../include/io.h"

/*
void keyboard_interrupt_handler_c(void)
{
    uint8_t scancode = inb(0x60);

    __asm__ __volatile__ (
    "movb $0x20, %%al\n\t"
    "outb %%al, $0x20"
    :
    :
    : "al"
    );
}
    */