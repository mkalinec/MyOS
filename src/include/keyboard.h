#pragma once

#include <stddef.h>

#define KBD_DATA 0x60
#define KBD_STATUS 0x64


uint8_t kbd_read_scancode();
char scancode_to_ascii(uint8_t scancode, int shift);
void update_shift(uint8_t scancode);
char kbd_get_char();
static inline uint8_t inb(uint16_t port);
static inline void outb(uint16_t port, uint8_t val);